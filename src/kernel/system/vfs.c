#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

fs_node_t *kernel_fd[MAX_KERNEL_FD];
fs_node_t *root_node;
vfs_entry_t *vfs_root;

void init_vfs()
{
    for (int i = 0; i < MAX_KERNEL_FD; i++) {
        kernel_fd[i] = NULL;
    } 
}

int fs_read(fs_node_t **node, size_t sz, int n, char *buf)
{
    return 0;
}

int fs_get_fd(fs_node_t *node)
{
    return 0;
}

int fs_write(fs_node_t **node, size_t sz, int n, char *buf)
{
    return 0;
}

int fs_rewind(fs_node_t *node)
{
    return 0;
}

int fs_open(fs_node_t *node, uint32_t flags)
{
    return 0;
}

int fs_close(fs_node_t *node)
{
    return 0;
}

struct dirent *fs_readdir(fs_node_t *node)
{
    return NULL;
}

struct fs_node *fs_finddir(fs_node_t *node, char *name)
{
    return NULL;
}

int fs_create(fs_node_t *node, char *name, uint32_t flags)
{
    return 0;
}

int fs_mkdir(fs_node_t *node, char *name, uint32_t flags)
{
    return 0;
}

int fs_ioctl(fs_node_t *node, int req, void *argp)
{
    return 0;
}

size_t fs_getsize(fs_node_t *node)
{
    return node->length;
}

int fs_chmod(fs_node_t *node, uint32_t mode)
{
    return 0;
}

int fs_unlink(fs_node_t *node)
{
    return 0;
}

int fs_symlink(fs_node_t *node, struct fs_node *link)
{
    return 0;
}

int fs_readlink(fs_node_t *node)
{
    return 0;
}

