#ifndef _procfs_h
#define _procfs_h

#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

#include <fs/vfs.h>

#define PROC_STDIN        0
#define PROC_STDOUT       1
#define PROC_STDERR       2
#define KERNEL_CONF       3
#define KERNEL_DIRECTORY  4

typedef struct procfs_super_block {
    uint32_t sb_struct_size;
    uint32_t fh_struct_size;
} procfs_super_block_t;

typedef struct procfs_file_header {
    uint32_t inode;
    uint32_t flags;
    char name[32];
    uint32_t length;
    uint32_t type;

    union {
        uint32_t pid;
        uint32_t module_conf;
    };
} procfs_file_header_t;

void procfs_init();
struct inode *procfs_alloc_inode(uint32_t flags, uint32_t length);
void procfs_alloc(struct inode *node, uint32_t length);
int procfs_create(struct inode *parent, char *name, uint32_t flags);
int procfs_write(struct inode *node, size_t sz, int n, char *buf);
int procfs_read(struct inode *node, size_t sz, int n, char *buf);
int procfs_open(struct inode *node, uint32_t flags);
int procfs_close(struct inode *node);
int procfs_rewind(struct inode *node);

#endif
