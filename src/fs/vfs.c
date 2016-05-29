#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

#include <fs/vfs.h>
#include <fs/initrd.h>
#include <fs/mount.h>
#include <fs/fcntl.h>
#include <fs/procfs.h>
#include <kernel/heap.h>
#include <kernel/io.h>
#include <kernel/process.h>

/*** REMOVED BECAUSE IS UNSTABLE ***/
/*** TODO: GET STABLE VFS ***/

graph_node_t *vfs_root;
super_block_t *vfs_super_block;
inode_t *vfs_root_inode;

struct dentry **dlist;
struct file **opened_files;
int file_table_size;
int dlist_size;

Llist_t *inode_cache;
Llist_t *directory_cache;

char kernel_space_open;

char cwd[4096];
struct dentry *cwd_de;

// Initrd
extern initrd_super_block_t *initrd_sb;
extern initrd_file_header_t *fheader;
extern initrd_file_header_t *initrd_headers;
extern inode_t *initrd_nodes;

extern process_t *current_process;

int file_open(struct inode *inode, struct file *f)
{
    int fd = -1;
    char found = 0;
    struct file **op_f;
    int f_tbl_size;

    if (kernel_space_open) {
        op_f = opened_files;
        f_tbl_size = file_table_size;
    } else {
        op_f = current_process->opened_files;
        f_tbl_size = current_process->file_table_size;
    }

    for (fd = 0; fd < f_tbl_size; fd++) {
        if (op_f[fd] == NULL) {
            op_f[fd] = f;
            found = 1;
            break;
            }
    }

    if (!found) {
        f_tbl_size *= 2;
        op_f = krealloc(op_f, sizeof(struct file*) * f_tbl_size);
        for (int i = f_tbl_size; i < f_tbl_size; i++) {
            op_f[i] = NULL;
        }
        fd = f_tbl_size / 2;
        found = 1;
    }

    if (kernel_space_open) {
        file_table_size = f_tbl_size;
    } else {
        current_process->file_table_size = f_tbl_size;
    }

    if (found)
        f->f_dentry = get_dentry_by_inode(inode);
    if (kernel_space_open) {
        opened_files = op_f;
    } else {
        current_process->opened_files = op_f;
    }

    return fd;
}

size_t file_read(struct file *f, char *buf, size_t sz, loff_t *off)
{
    if (f->f_dentry == NULL)
        return -1;

    inode_t *inode = f->f_dentry->inode;
    inode->offset = (size_t) *off;
    return inode->read(inode, sz, 1, buf);
}

size_t file_write(struct file *f, const char *buf, size_t sz, loff_t *off)
{
    if (f->f_dentry == NULL) {
        return -1;
    }

    inode_t *inode = f->f_dentry->inode;
    inode->offset = (size_t) *off;
    return inode->write(inode, sz, 1, (char*)buf);
}

struct dentry *file_readdir(struct file *f, DIR *context)
{
    inode_t *inode = f->f_dentry->inode;
    int pos = context->pos++;
    size_t old_off = inode->offset;

    inode_rewind(inode);
    int i = 0;
    size_t addr;
    inode_read(inode, sizeof(size_t), 1, (char*) &addr);
    while (i < pos && addr != 0x0) {
        inode_read(inode, sizeof(size_t), 1, (char*) &addr);
        i++;
    }

    struct dentry *de = (struct dentry*) addr;
    inode->offset = old_off;

    return de;
}

struct file_operations fops = {
   .read  = file_read,
   .write = file_write,
   .open  = file_open,
   .readdir = file_readdir
};

void init_vfs()
{
    /*
    FILESYSTEM STRUCTURE
    *** <name> (<type>) ***

    / (procfs)
    |
    ------ /mnt (procfs)
    |        |
    |        ------ /mnt/initrd (initrd)
    |
    ------ /proc (procfs)

    */
    procfs_init();
    // Root directory
    struct inode *root_inode = procfs_alloc_inode(FS_DIRECTORY, sizeof(void*) * 10);
    procfs_file_header_t *root_header = (procfs_file_header_t*) root_inode->block;
    root_header->inode = root_inode->inode;
    root_header->flags = FS_DIRECTORY;
    memcpy(root_header->name, "/\0", 2);
    root_header->type = KERNEL_DIRECTORY;

    vfs_root = graph_create(NULL);
    vfs_super_block = (super_block_t*) kmalloc(sizeof(super_block_t), 0, 0);
    vfs_root_inode = root_inode;
    inode_cache = NULL;
    directory_cache = NULL;

    vfs_super_block->device = NULL;
    vfs_super_block->inode = vfs_root_inode;
    vfs_super_block->block = 0;
    vfs_super_block->fs_type = NULL;
    vfs_super_block->fs_data = (void*) NULL;

    // mnt directory
    struct inode *mnt_inode = procfs_alloc_inode(FS_DIRECTORY, sizeof(void*) * 10);
    procfs_file_header_t *mnt_header = (procfs_file_header_t*) mnt_inode->block;
    mnt_header->inode = mnt_inode->inode;
    mnt_header->flags = FS_DIRECTORY;
    memcpy(mnt_header->name, "mnt\0", 4);
    mnt_header->type = KERNEL_DIRECTORY;

    // proc directory
    struct inode *proc_inode = procfs_alloc_inode(FS_DIRECTORY, sizeof(void*) * 10);
    procfs_file_header_t *proc_header = (procfs_file_header_t*) proc_inode->block;
    proc_header->inode = proc_inode->inode;
    proc_header->flags = FS_DIRECTORY;
    memcpy(proc_header->name, "proc\0", 5);
    proc_header->type = KERNEL_DIRECTORY;

    // initrd inode
    struct inode *initrd_inode = procfs_alloc_inode(FS_DIRECTORY, sizeof(void*) * 10);
    procfs_file_header_t *initrd_header = (procfs_file_header_t*) initrd_inode->block;
    initrd_header->inode = initrd_inode->inode;
    initrd_header->flags = FS_DIRECTORY;
    memcpy(initrd_header->name, "initrd\0", 7);
    initrd_header->type = KERNEL_DIRECTORY;

    opened_files = kmalloc(sizeof(struct file*) * DEFAULT_F_TBL_SIZE, 0, 0);
    file_table_size = DEFAULT_F_TBL_SIZE;

    for (int i = 0; i < file_table_size; i++) {
        opened_files[i] = NULL;
    }
    kernel_space_open = 1;

    dlist = kmalloc(sizeof(struct dentry*) * DEFAULT_DLIST_SIZE, 0, 0);
    dlist_size = DEFAULT_DLIST_SIZE;


    for (int i = 0; i < dlist_size; i++) {
        dlist[i] = NULL;
    }
    struct dentry *temp_de = kmalloc(sizeof(struct dentry), 0, 0);
    graph_node_t *temp_g = NULL;
    size_t addr = 0;

    temp_de->name = kmalloc(sizeof(char) * 2, 0, 0);
    memcpy(temp_de->name, "/\0", 2);
    temp_de->parent_dentry = NULL;
    temp_de->inode = root_inode;
    add_dentry(temp_de);
    vfs_root->data = (void*) temp_de;

    temp_de = kmalloc(sizeof(struct dentry), 0, 0);
    temp_de->name = kmalloc(sizeof(char) * 4, 0, 0);
    memcpy(temp_de->name, "mnt\0", 4);
    temp_de->parent_dentry = dlist[0];
    temp_de->inode = mnt_inode;
    add_dentry(temp_de);
    temp_g = graph_create((void*) temp_de);
    graph_add_node(vfs_root, temp_g);
    addr = (size_t) temp_de;
    inode_write(root_inode, sizeof(size_t), 1, (char*) &addr);

    temp_de = kmalloc(sizeof(struct dentry), 0, 0);
    temp_de->name = kmalloc(sizeof(char) * 5, 0, 0);
    memcpy(temp_de->name, "proc\0", 5);
    temp_de->parent_dentry = dlist[0];
    temp_de->inode = proc_inode;
    add_dentry(temp_de);
    temp_g = graph_create((void*) temp_de);
    graph_add_node(vfs_root, temp_g);
    addr = (size_t) temp_de;
    inode_write(root_inode, sizeof(size_t), 1, (char*) &addr);

    temp_de = kmalloc(sizeof(struct dentry), 0, 0);
    temp_de->name = kmalloc(sizeof(char) * 7, 0, 0);
    memcpy(temp_de->name, "initrd\0", 7);
    temp_de->parent_dentry = get_dentry_by_path("/mnt");
    temp_de->inode = initrd_inode;
    add_dentry(temp_de);

    temp_g = graph_create((void*) temp_de);
    graph_node_t *mnt_node = get_node_by_path("/mnt");
    graph_add_node(mnt_node, temp_g);

    addr = (size_t) temp_de;
    inode_write(mnt_inode, sizeof(size_t), 1, (char*) &addr);
    super_block_t *initrd_gsb = (super_block_t*) kmalloc(sizeof(super_block_t), 0, 0);

    initrd_gsb->device = NULL;
    initrd_gsb->inode = initrd_inode;
    initrd_gsb->block = 0;
    initrd_gsb->fs_type = NULL;
    initrd_gsb->fs_data = (void*) initrd_sb;
    mount_fs(initrd_gsb, "/mnt/initrd", initrd_mount);
    memcpy(cwd, "/\0", 2);
    cwd_de = dlist[0];
}

void add_dentry(struct dentry *de)
{
    char found = 0;
    int i = 0;

    for (i = 0; i < dlist_size; i++) {
        if (dlist[i] == NULL) {
            found = 1;
            break;
        }
    }

    if (found) {
        dlist[i] = de;
    } else {
        dlist_size *= 2;
        dlist = krealloc(dlist, sizeof(struct dentry*));

        for (i = dlist_size / 2; i < dlist_size; i++) {
            dlist[i] = NULL;
        }
        dlist[dlist_size / 2] = de;
    }
}

int index_dentry(struct dentry *de)
{
    int i = -1;

    for (int j = 0; j < dlist_size; j++) {
        if (dlist[j] == de) {
            i = j;
        }
    }

    return i;
}

struct dentry *get_dentry_by_inode(inode_t *inode)
{
    struct dentry *de = NULL;
    int i;
    for (i = 0; dlist[i]->inode != inode && dlist[i] != NULL; i++) {
    }
    de = dlist[i];

    return de;
}

void destroy_tokens(path_tokens *tokens)
{
    for (int i = 0; i < tokens->n; i++) {
        kfree(tokens->tokens[i]);
    }
    kfree(tokens->tokens);
    kfree(tokens);
}

path_tokens *tokenize_path(char *path)
{
    path_tokens *tokens = kmalloc(sizeof(path_tokens), 0, 0);
    tokens->n = 0;

    int tokens_n = 0;
    if (path[0] == '/') {
        tokens_n = 1;
    } else {
        kfree(tokens);
        return NULL;
    }
    for (int i = 0; path[i] != '\0'; i++) {
        if (path[i] == PATH_SEPARATOR && path[i + 1] != '\0') {
            tokens_n++;
        }
    }
    tokens->n = tokens_n;

    tokens->tokens = kmalloc(sizeof(char*) * tokens_n, 0, 0);
    for (int i = 0; i < tokens_n; i++) {
        tokens->tokens[i] = kmalloc(sizeof(char) * 256, 0, 0);
        memset(tokens->tokens[i], 0, sizeof(char) * 256);
    }

    tokens->tokens[0][0] = '/';

    char in_word = 1;
    char word[256];
    word[0] = '\0';
    int i, j, k;

    for (i = 1, j = 0, k = 1; path[i] != '\0'; i++) {
        if (path[i + 1] == '\0') {
            word[j] = path[i];
            j++;
            word[j] = '\0';
            memcpy(tokens->tokens[k], word, j);
        } else if (path[i] == PATH_SEPARATOR && in_word) {
            word[j] = '\0';
            memcpy(tokens->tokens[k], word, j);
            k++;
            j = 0;
            word[0] = '\0';
            in_word = 0;
        } else if (path[i] == PATH_SEPARATOR && !in_word) {
            destroy_tokens(tokens);
            return NULL;
        } else if (path[i] != PATH_SEPARATOR && in_word) {
            word[j] = path[i];
            j++;
        } else {
            word[j] = path[i];
            j++;
            in_word = 1;
        }
    }

    return tokens;
}

graph_node_t *get_node_by_tokens(path_tokens *tokens)
{
    graph_node_t *found_node = NULL;
    graph_node_t *node = NULL;
    if (!strcmp(tokens->tokens[0], "/")) {
        found_node = vfs_root;
        node = vfs_root;

        if (tokens->n == 1) {
            return found_node;
        }
    } else {
        return NULL;
    }
    for (int i = 1; i < tokens->n; i++) {
        int has_dentry = 0;
        struct dentry *temp_dentry = NULL;
        graph_node_t *gnode = NULL;
        Llist_t *nodes = node->nodes;
        while (nodes != NULL) {
            gnode = (graph_node_t*) nodes->data;
            temp_dentry = (struct dentry*) gnode->data;
            if (!strcmp(temp_dentry->name, tokens->tokens[i])) {
                found_node = gnode;
                node = gnode;
                has_dentry = 1;
                break;
            }

            nodes = nodes->next;
        }

        if (!has_dentry) {
            return NULL;
        }
    }

    return found_node;
}

struct dentry *get_dentry_by_tokens(path_tokens *tokens)
{
    graph_node_t *node = get_node_by_tokens(tokens);
    if (node == NULL) {
        return NULL;
    }
    struct dentry *de = (struct dentry*) node->data;
    return de;
}

struct dentry *get_dentry_by_path(char *path)
{
    path_tokens *tokens = tokenize_path(path);
    if (tokens == NULL) {
        return NULL;
    }
    struct dentry *de = get_dentry_by_tokens(tokens);
    destroy_tokens(tokens);
    return de;
}

graph_node_t *get_node_by_path(char *path)
{
    path_tokens *tokens = tokenize_path(path);
    if (tokens == NULL) {
        return NULL;
    }
    graph_node_t *gn = get_node_by_tokens(tokens);
    destroy_tokens(tokens);

    return gn;
}

int inode_init(struct inode *node, size_t flags, size_t id, size_t length,
                size_t nblocks, size_t blocksz, size_t block, size_t fsid)
{
    node->fsid = fsid;
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

int inode_open(inode_t *node, size_t flags)
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

int inode_create(inode_t *node, char *name, size_t flags)
{
    return 0;
}

int inode_mkdir(inode_t *node, char *name, size_t flags)
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

int inode_chmod(inode_t *node, size_t mode)
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
