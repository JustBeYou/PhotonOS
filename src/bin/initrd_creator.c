// This is not a part of the OS!
// This program creates a initrd file for PhotonOS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

#define INITRD_MAGIC 0xBEEFBEEF

#define FS_FILE       0x01
#define FS_DIRECTORY  0x02
#define FS_DEVICE     0x04
#define FS_PIPE       0x08
#define FS_SYMLINK    0x10
#define FS_MOUNTPOINT 0x20
#define FS_META       0x40

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

int main(int argc, char **argv)
{
    FILE *initrd = fopen("initrd", "wb");
    initrd_super_block_t *sb = malloc(sizeof(initrd_super_block_t));
    sb->magic = INITRD_MAGIC;
    sb->sb_struct_size = sizeof(initrd_super_block_t);
    sb->fh_struct_size = sizeof(initrd_file_header_t);
    sb->files = argc;
    fwrite(sb, sb->sb_struct_size, 1, initrd);

    int inode = 0;

    initrd_file_header_t *root_dir = malloc(sizeof(initrd_file_header_t));
    root_dir->magic = INITRD_MAGIC;
    root_dir->inode = inode++;
    root_dir->flags = FS_DIRECTORY;
    memset(root_dir->name, 0, 32);
    strcpy(root_dir->name, "/");
    root_dir->length = sizeof(int) * (argc - 1);
    fwrite(root_dir, sb->fh_struct_size, 1, initrd);
    for (int i = 0, tmp = inode; i < argc - 1; i++) {
        fwrite((void*) &tmp, sizeof(int), 1, initrd);
        tmp++;
    }

    for (int i = 1; i < argc; i++) {
        FILE *f = fopen(argv[i], "rb");
        fseek(f, 0L, SEEK_END);
        size_t sz = ftell(f);
        fseek(f, 0L, SEEK_SET);

        initrd_file_header_t *fh = malloc(sizeof(initrd_file_header_t));
        fh->magic = INITRD_MAGIC;
        fh->flags = FS_FILE;
        fh->inode = inode++;
        memset(fh->name, 0, 32);
        strcpy(fh->name, argv[i]);
        fh->length = sz;

        char *fcontent = malloc(sz);
        fread(fcontent, sz, 1, f);

        fwrite(fh, sb->fh_struct_size, 1, initrd);
        fwrite(fcontent, sz, 1, initrd);

        free(fh);
        free(fcontent);
        fclose(f);
    }

    free(sb);
    free(root_dir);
    fclose(initrd);

    return 0;
}
