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
    virtual_disk = calloc(0x5000);
    phos_super_block_t *vd_super_block = calloc(sizeof(phos_super_block_t));
    phos_file_header_t *vd_root_file = calloc(sizeof(phos_file_header_t));
    
    vd_super_block->magic = PHFS_SUPER_MAGIC;
    vd_super_block->length = 0x5000;
    vd_super_block->free_space = 0x5000;
    vd_super_block->c_inode = 0;
    vd_super_block->nfiles = 0;
    
    vd_root_file->magic = PHFS_FILE_MAGIC;
    vd_root_file->inode = 0;
    memcpy(vd_root_file->name, "root\0", 5);
    vd_root_file->type = PHFS_FILE;
    vd_root_file->offset = sizeof(phos_super_block_t);
    vd_root_file->length = 0;
    
    memcpy(virtual_disk, vd_super_block, sizeof(phos_super_block_t));
    memcpy(virtual_disk + sizeof(phos_super_block_t), vd_root_file, sizeof(phos_file_header_t));
    
    /* Virtual File System Init */
    vfs_root = graph_create(NULL);
    vfs_super_block = NULL; // TODO: implement super block
    vfs_root_inode = malloc(sizeof(inode_t));
    inode_cache = Llist_create();
    directory_cache = Llist_create();
    
    vfs_root->data = (void*) vfs_root_inode;
    
    vfs_root_inode->device = (void*) virtual_disk;
    vfs_root_inode->mask = 0;
    vfs_root_inode->uid = 0;
    vfs_root_inode->gid = 0;
    vfs_root_inode->flags = FS_DIRECTORY;
    vfs_root_inode->inode = 0;
    vfs_root_inode->length = 0;
    vfs_root_inode->block = 4096;
    vfs_root_inode->open_flags = 0;
    vfs_root_inode->lock = 0;
    vfs_root_inode->offset = 0;
    vfs_root_inode->atime = 0;
    vfs_root_inode->mtime = 0;
    vfs_root_inode->ctime = 0;
    vfs_root_inode->link = NULL;
    vfs_root_inode->refcount = 1;
    vfs_root_inode->nlink = 0;
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

/* Photon File System */
int phfs_read(struct inode *node, size_t sz, int n, char *buf)
{

}

int phfs_write(struct inode *node, size_t sz, int n, char *buf)
{

}

int phfs_open(struct inode *node, uint32_t flags)
{

}

int phfs_close(struct inode *node)
{

}

int phfs_rewind(struct inode *node)
{

}
