#ifndef _mount_h
#define _mount_h

#include <fs/vfs.h>

typedef struct mnt_table {
    struct dentry *de;
    super_block_t *sb;
} mnt_table_t;

void mount_init();
int mount_fs(super_block_t *sb, char *path, void (*mount)(super_block_t*, int, char*));

#endif
