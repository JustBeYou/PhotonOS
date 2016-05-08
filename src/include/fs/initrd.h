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
    uint32_t magic;
    uint32_t sb_struct_size;
    uint32_t fh_struct_size;
    uint32_t files;
} initrd_super_block_t;

typedef struct initrd_file_header {
    uint32_t magic;
    uint32_t inode;
    uint32_t flags;
    char name[32];
    uint32_t length;
} initrd_file_header_t;

void initrd_load(uint32_t location);
void initrd_mount(super_block_t *sb, int index, char *path);
int initrd_read(struct inode *node, size_t sz, int n, char *buf);
int initrd_open(struct inode *node, uint32_t flags);
int initrd_close(struct inode *node);
int initrd_rewind(struct inode *node);

#endif
