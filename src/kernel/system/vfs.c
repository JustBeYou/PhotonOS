#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

/*** REMOVED BECAUSE IS UNSTABLE ***/
/*** TODO: GET STABLE VFS ***/

char *virtual_disk;

FILE *kernel_fd[MAX_KERNEL_FD];
graph_node_t *vfs_root;
super_block_t *vfs_super_block;
inode_t *vfs_root_inode;
Llist_t *inode_cache;
Llist_t *directory_cache;

void init_vfs()
{
    for (int i = 0; i < MAX_KERNEL_FD; i++) {
        kernel_fd[i] = NULL;
    }

    /* Virtual Disk Init */
    
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
