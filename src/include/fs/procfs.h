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
    size_t sb_struct_size;
    size_t fh_struct_size;
} procfs_super_block_t;

typedef struct procfs_file_header {
    size_t inode;
    size_t flags;
    char name[32];
    size_t length;
    size_t type;

    union {
        size_t pid;
        size_t module_conf;
    };
} procfs_file_header_t;

void procfs_init();
struct inode *procfs_alloc_inode(size_t flags, size_t length);
void procfs_alloc(struct inode *node, size_t length);
int procfs_create(struct inode *parent, char *name, size_t flags);
int procfs_mkdir(struct inode *parent, char *name);
int procfs_write(struct inode *node, size_t sz, int n, char *buf);
int procfs_read(struct inode *node, size_t sz, int n, char *buf);
int procfs_open(struct inode *node, size_t flags);
int procfs_close(struct inode *node);
int procfs_rewind(struct inode *node);

#endif
