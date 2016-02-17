// This is not a part of the OS!
// This program creates a initrd file for PhotonOS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

#define INITRD_MAGIC 0xBEEFBEEF

typedef struct initrd_super_block {
    uint32_t magic;
    uint32_t sb_struct_size;
    uint32_t fh_struct_size;
} initrd_super_block_t;

typedef struct initrd_file_header {
    uint32_t inode;
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
    fwrite(sb, sb->sb_struct_size, 1, initrd);

    int inode = 0;
    for (int i = 1; i < argc; i++) {
        FILE *f = fopen(argv[i], "rb");
        fseek(f, 0L, SEEK_END);
        size_t sz = ftell(f);
        fseek(f, 0L, SEEK_SET);

        initrd_file_header_t *fh = malloc(sizeof(initrd_file_header_t));
        fh->inode = inode++;
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
    fclose(initrd);

    return 0;
}
