#ifndef _initrd_h
#define _initrd_h

#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

#include <fs/vfs.h>

/* Initrd file system */
#define INITRD_MAGIC 0xBEEFBEEF

typedef struct initrd_super_block {
    size_t magic;
    size_t sb_struct_size;
    size_t fh_struct_size;
    size_t files;
} initrd_super_block_t;

typedef struct initrd_file_header {
    size_t magic;
    size_t inode;
    size_t flags;
    char name[32];
    size_t length;
} initrd_file_header_t;

void initrd_load(size_t location);
void initrd_mount(super_block_t *sb, int index, char *path);
int initrd_read(struct inode *node, size_t sz, int n, char *buf);
int initrd_open(struct inode *node, size_t flags);
int initrd_close(struct inode *node);
int initrd_rewind(struct inode *node);

#endif
