#include <fs/vfs.h>
#include <fs/mount.h>
#include <kernel/io.h>

mnt_table_t mounted_fs[256];

void mount_init()
{
    for (int i = 0; i < 256; i++) {
        mounted_fs[i].sb = NULL;
        mounted_fs[i].de = NULL;
    }
}

int mount_fs(super_block_t *sb, char *path, void (*mount)(super_block_t*, int, char*))
{
    int i;
    char found = 0;
    for (i = 0; i < 256; i++) {
        if (mounted_fs[i].sb == NULL) {
            found = 1;
            mount(sb, i, path);
            break;
        }
    }

    if (found) {
        return i;
    }

    printk("You can't mount more than 256 filesystems.\n");
    return -1;
}
