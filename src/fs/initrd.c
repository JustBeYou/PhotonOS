#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>
#include <fs/vfs.h>

#include <kernel/heap.h>
#include <kernel/io.h>
#include <fs/initrd.h>
#include <fs/vfs.h>

initrd_super_block_t *sb;
initrd_file_header_t *fheader;
initrd_file_header_t *initrd_headers;
inode_t *initrd_nodes;

/* Initrd File System */
void initrd_load(uint32_t location)
{
    sb = (initrd_super_block_t*) location;
    fheader = NULL;
    initrd_nodes = kmalloc(sizeof(inode_t) * sb->files, 0, 0);
    initrd_headers = kmalloc(sizeof(initrd_file_header_t) * sb->files, 0, 0);

    location += sb->sb_struct_size;
    for (int i = 0; i < sb->files; i++) {
        fheader = (initrd_file_header_t*) location;
        memcpy(&(initrd_headers[i]), fheader, sizeof(initrd_file_header_t));
        uint32_t block_addr = (uint32_t) fheader + sb->fh_struct_size;
        inode_init(&initrd_nodes[i], fheader->flags, fheader->inode,
                    fheader->length, 0, 0, block_addr);
        initrd_nodes[i].open = &initrd_open;
        initrd_nodes[i].read = &initrd_read;
        initrd_nodes[i].rewind = &initrd_rewind;
        initrd_nodes[i].close = &initrd_close;

        location += (uint32_t) sizeof(initrd_file_header_t);
        location += (uint32_t) fheader->length;
    }
}

int initrd_read(struct inode *node, size_t sz, int n, char *buf)
{
    uint32_t i;
    uint32_t phys = node->block + node->offset;
    uint32_t limit = node->block + node->length;

    for (i = 0; i < sz * n && phys + i < limit; i++) {
        buf[i] = ((char*) phys)[i];
    }
    node->offset += i;
    return i;
}

int initrd_open(struct inode *node, uint32_t flags)
{

}

int initrd_close(struct inode *node)
{

}

int initrd_rewind(struct inode *node)
{
    node->offset = 0;
    return 0;
}
