#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

#include <fs/vfs.h>
#include <fs/initrd.h>
#include <kernel/heap.h>
#include <kernel/io.h>

/*** REMOVED BECAUSE IS UNSTABLE ***/
/*** TODO: GET STABLE VFS ***/

graph_node_t *vfs_root;
super_block_t *vfs_super_block;
inode_t *vfs_root_inode;
struct dentry *dlist;
Llist_t *opened_files;
Llist_t *inode_cache;
Llist_t *directory_cache;

// Initrd
extern initrd_super_block_t *sb;
extern initrd_file_header_t *fheader;
extern initrd_file_header_t *initrd_headers;
extern inode_t *initrd_nodes;

int file_open(struct inode *inode, struct file *f)
{
    Llist_t *file_list = opened_files;
    Llist_t *new_file = kmalloc(sizeof(Llist_t), 0, 0);
    if (file_list->data == NULL) {
        kfree(new_file);
        file_list->data = (void*) f;
        return 0;
    }

    int fd = 1;
    while (file_list->next != NULL) {
        file_list = file_list->next;
        fd++;
    }

    file *temp = (file*)file_list->data;

    file_list->next = new_file;
    new_file->prev = file_list;
    new_file->next = NULL;
    new_file->data = (void*) f;

    f->f_dentry = get_dentry_by_inode(inode);

    return fd;
}

int file_read(struct file *f, char *buf, size_t sz, loff_t *off)
{
    if (f->f_dentry == NULL)
        return -1;

    inode_t *inode = f->f_dentry->inode;
    inode->offset = (uint32_t) *off;
    return inode->read(inode, sz, 1, buf);
}

struct file_operations fops = {
   .read = file_read,
   .open = file_open,
};

void init_vfs()
{
    vfs_root = graph_create(NULL);
    vfs_super_block = (super_block_t*) kmalloc(sizeof(super_block_t), 0, 0);
    vfs_root_inode = initrd_nodes;
    inode_cache = NULL;
    directory_cache = NULL;

    vfs_super_block->device = NULL;
    vfs_super_block->inode = initrd_nodes;
    vfs_super_block->block = 0;
    vfs_super_block->fs_type = NULL;
    vfs_super_block->fs_data = (void*) sb;

    dlist = kmalloc(sizeof(struct dentry) * sb->files + 1, 0, 0);
    dlist[0].name = kmalloc(sizeof(char) * 2, 0, 0);
    memcpy(dlist[0].name, "/\0", 2);
    dlist[0].parent_dentry = NULL;
    dlist[0].inode = vfs_root_inode;
    for (int i = 1; i < sb->files; i++) {
        dlist[i].name = kmalloc(sizeof(char) * 32, 0, 0);
        memcpy(dlist[i].name, initrd_headers[i].name, strlen(initrd_headers[i].name));
        dlist[i].parent_dentry = dlist;
        dlist[i].inode = &initrd_nodes[i];
    }

    vfs_root->data = (void*) dlist;
    for (int i = 1; i < sb->files; i++) {
        graph_node_t *f = graph_create((void*) &dlist[i]);
        graph_add_node(vfs_root, f);
    }

    opened_files = Llist_create(NULL);

    char path[] = "/f1.txt";
    struct dentry *de = get_dentry_by_path(path);
}

struct dentry *get_dentry_by_inode(inode_t *inode)
{
    struct dentry *de = NULL;
    int i;
    for (i = 0; dlist[i].inode != inode; i++) {}
    de = &dlist[i];

    return de;
}

path_tokens *tokenize_path(char *path)
{
    if (path == NULL)
        return NULL;

    path_tokens *tokens = kmalloc(sizeof(path_tokens), 0, 0);

    size_t len = strlen(path);
    int tokens_n = 0;
    for (size_t i = 0; i < len; i++) {
        if (path[i] == PATH_SEPARATOR) {
            tokens_n++;
        }
    }
    tokens->n = tokens_n;

    tokens->tokens = kmalloc(sizeof(char*) * tokens_n, 0, 0);
    memset(tokens->tokens, 0, sizeof(char*) * tokens_n);
    for (size_t i = 0; i <= tokens_n; i++) {
        tokens->tokens[i] = kmalloc(sizeof(char) * 32, 0, 0);
        memset(tokens->tokens[i], 0, 32);
    }

    int i = 0;
    int j = 0;
    int token_i = 0;
    int in_word = 1;
    if (path[0] == '/'/* || path[0] == '~'*/) {
        tokens->tokens[0][0] = path[0];
        token_i++;
        i++;
    }

    for (; i < len; i++) {
        if (!in_word && path[i] != PATH_SEPARATOR) {
            in_word = 1;
        }

        if (in_word && path[i] != PATH_SEPARATOR) {
            tokens->tokens[token_i][j++] = path[i];
        } else if (in_word && path[i] == PATH_SEPARATOR) {
            token_i++;
            j = 0;
            in_word = 0;
        } else if (!in_word && path[i] == PATH_SEPARATOR) {
            for (size_t fi = 0; fi < tokens_n; fi++) {
                kfree(tokens->tokens[fi]);
            }
            kfree(tokens->tokens);
            return NULL;
        }
    }

    return tokens;
}

struct dentry *get_dentry_by_tokens(path_tokens *tokens)
{
    struct dentry *de = NULL;
    graph_node_t *node = NULL;
    if (!strcmp(tokens->tokens[0], "/")) {
        de = &dlist[0];
        node = vfs_root;
    } else {
        return NULL;
    }

    for (int i = 1; i <= tokens->n; i++) {
        int has_dentry = 0;
        struct dentry *temp_dentry = NULL;
        graph_node_t *gnode = NULL;
        Llist_t *nodes = node->nodes;
        while (nodes != NULL) {
            gnode = (graph_node_t*) nodes->data;
            temp_dentry = (struct dentry*) gnode->data;
            if (!strcmp(temp_dentry->name, tokens->tokens[i])) {
                de = temp_dentry;
                has_dentry = 1;
            }

            nodes = nodes->next;
        }

        if (!has_dentry) {
            return NULL;
        }
    }

    return de;
}

struct dentry *get_dentry_by_path(char *path)
{
    path_tokens *tokens = tokenize_path(path);
    if (tokens == NULL) {
        return NULL;
    }
    struct dentry *de = get_dentry_by_tokens(tokens);
    for (size_t fi = 0; fi < tokens->n; fi++) {
        kfree(tokens->tokens[fi]);
    }
    kfree(tokens->tokens);

    return de;
}

int inode_init(struct inode *node, uint32_t flags, uint32_t id, uint32_t length,
                uint32_t nblocks, uint32_t blocksz, uint32_t block)
{
    node->flags = flags;
    node->inode = id;
    node->length = length;
    node->nblocks = nblocks;
    node->blocksz = blocksz;
    node->block = block;
    node->lock = 0;
    node->offset = 0;
    node->open_flags = 0;
    node->link = NULL;
    node->refcount = 0;
    node->nlink = 0;
    return 0;
}

int inode_read(inode_t *node, size_t sz, int n, char *buf)
{
    if (!node)
        return -1;

    if (node->read) {
        int ret = node->read(node, sz, n, buf);
        return ret;
    } else {
        return -1;
    }
}


int inode_write(inode_t *node, size_t sz, int n, char *buf)
{
    if (!node)
        return -1;

    if (node->write) {
        int ret = node->write(node, sz, n, buf);
        return ret;
    }

    return -1;
}

int inode_rewind(inode_t *node)
{
    if (!node)
        return -1;

    node->rewind(node);
    return 0;
}

int inode_open(inode_t *node, uint32_t flags)
{
    if (!node)
        return -1;

    if (node->refcount >= 0) {
        node->refcount++;
    }

    if (node->open) {
        int ret = node->open(node, flags);
        return ret;
    }

    return -1;
}

int inode_close(inode_t *node)
{
    if (node == vfs_root_inode) {
        printk("[WARN] Attempt to close the root.\n");
        return -1;
    }

    if (!node) {
        return -1;
    }

    if (node->refcount == -1) {
        return -1;
    }

    node->refcount--;
    if (node->refcount == 0) {
        if (node->close) {
            node->close(node);
        }
        free(node);
    }
    return 0;
}

struct dentry *inode_readdir(inode_t *node)
{
    return NULL;
}

struct inode *inode_finddir(inode_t *node, char *name)
{
    return NULL;
}

int inode_create(inode_t *node, char *name, uint32_t flags)
{
    return 0;
}

int inode_mkdir(inode_t *node, char *name, uint32_t flags)
{
    return 0;
}

int inode_ioctl(inode_t *node, int req, void *argp)
{
    return 0;
}

size_t inode_getsize(inode_t *node)
{
    return node->length;
}

int inode_chmod(inode_t *node, uint32_t mode)
{
    return 0;
}

int inode_unlink(inode_t *node)
{
    return 0;
}

int inode_symlink(inode_t *node, struct inode *link)
{
    return 0;
}

int inode_readlink(inode_t *node)
{
    return 0;
}
