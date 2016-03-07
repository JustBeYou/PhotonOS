#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

#include <fs/vfs.h>
#include <fs/initrd.h>

/*** REMOVED BECAUSE IS UNSTABLE ***/
/*** TODO: GET STABLE VFS ***/

graph_node_t *vfs_root;
super_block_t *vfs_super_block;
inode_t *vfs_root_inode;
Llist_t *inode_cache;
Llist_t *directory_cache;

// Initrd
extern initrd_super_block_t *sb;
extern initrd_file_header_t *fheader;
extern inode_t *initrd_nodes;

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

    vfs_root->data = (void*) vfs_root_inode;
    for (int i = 1; i < sb->files; i++) {
        graph_node_t *f = graph_create((void*) &initrd_nodes[i]);
        graph_add_node(vfs_root, f);
    }
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

struct dirent *inode_readdir(inode_t *node)
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
