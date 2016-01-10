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
    
    return 1;    
}

int fs_read(fs_node_t **node, size_t sz, int n, char *buf)
{
    char *file_start = (char*) FILE_BEGIN((*node)) + (*node)->count;
    char *file_end = (char*) FILE_END((*node));
    int count = 0;
    
    for (size_t i = 0, max = sz * n; i < max; i++) {
        if (&(file_start[i]) == file_end) {
            break;
        }
        
        buf[i] = file_start[i];
        (*node)->count++;
        count++;
    }

    return count;
}

int fs_get_fd(fs_node_t *node)
{
    for (int i = 0; i < MAX_KERNEL_FD; i++) {
        if (kernel_fd[i] == node) {
            return i;
        }
    }
    return -1;
}

int fs_write(fs_node_t **node, size_t sz, int n, char *buf)
{
    char *file_start = (char*) FILE_BEGIN((*node)) + (*node)->count;
    char *file_end = (char*) FILE_END((*node));
    int fd = fs_get_fd((*node));
    int count = 0;
    int toadd = 0;
    
    if (sz * n > (*node)->length) {
        toadd = (size_t) file_start + sz * n - (size_t) file_end;
        (*node) = krealloc((*node), 
                        sizeof((*node)) + (*node)->length + toadd);
        (*node)->length += toadd;
        file_start = (char*) FILE_BEGIN((*node)) + (*node)->count;
        file_end = (char*) FILE_END((*node));
        kernel_fd[fd] = (*node);
    }
    
    for (size_t i = 0, max = sz * n; i < max; i++) {
        if (&(file_start[i]) == file_end) {
            break;
        }
        
        file_start[i] = buf[i];
        (*node)->count++;
        count++;
    }
    
    return count;
}

int fs_rewind(fs_node_t *node)
{
    node->count = 0;
    return 0;
}

int fs_open(fs_node_t *node, uint32_t flags)
{
    for (int i = 0; i < MAX_KERNEL_FD; i++) {
        if (kernel_fd[i] == NULL) {
            kernel_fd[i] = node;
            node->open_flags = flags;
            node->refcount++;
            node->count = 0;
            break;
        }
    }
    return -1;
}

int fs_close(fs_node_t *node)
{
    sti();
    for (int i = 0; i < MAX_KERNEL_FD; i++) {
        getchar();
        if (kernel_fd[i] == node) {
            if (node->refcount != 0) {
                kernel_fd[i] = NULL;
                node->count = 0;
                node->refcount--;
                node->open_flags = 0;
                break;
            }
        }
    }
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
    memcpy(node->name, name, strlen(name) + 1);
    node->device = NULL; // Working on this
    node->mask = 0;
    node->uid = 0;
    node->gid = 0;
    node->flags = flags;
    node->inode = 0;
    node->length = 0;
    node->impl = 0;
    node->open_flags = 0;
    node->count = 0;
    node->atime = 0;
    node->mtime = 0;
    node->ctime = 0;
    node->link = NULL;
    node->refcount = 0;
    node->nlink = 0;
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

