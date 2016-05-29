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
#include <fs/mount.h>

initrd_super_block_t *initrd_sb;
initrd_file_header_t *fheader;
initrd_file_header_t *initrd_headers;
inode_t *initrd_nodes;

extern mnt_table_t mounted_fs[256];

/* Initrd File System */
void initrd_load(size_t location)
{
    initrd_sb = (initrd_super_block_t*) location;
    fheader = NULL;
    initrd_nodes = kmalloc(sizeof(inode_t) * initrd_sb->files, 0, 0);
    initrd_headers = kmalloc(sizeof(initrd_file_header_t) * initrd_sb->files, 0, 0);

    location += initrd_sb->sb_struct_size;
    for (size_t i = 0; i < initrd_sb->files; i++) {
        fheader = (initrd_file_header_t*) location;
        memcpy(&(initrd_headers[i]), fheader, sizeof(initrd_file_header_t));
        size_t block_addr = (size_t) fheader + initrd_sb->fh_struct_size;
        inode_init(&initrd_nodes[i], fheader->flags, fheader->inode,
                    fheader->length, 0, 0, block_addr, INITRD_TYPE);
        initrd_nodes[i].open = &initrd_open;
        initrd_nodes[i].read = &initrd_read;
        initrd_nodes[i].rewind = &initrd_rewind;
        initrd_nodes[i].close = &initrd_close;

        location += (size_t) sizeof(initrd_file_header_t);
        location += (size_t) fheader->length;
    }
}

void initrd_mount(super_block_t *sb, int index, char *path)
{
    struct dentry *de = get_dentry_by_path(path);
    if (de == NULL) {
        return;
    }

    initrd_super_block_t *sb_data = (initrd_super_block_t*) sb->fs_data;
    struct inode *node = de->inode;

    mounted_fs[index].de = de;
    mounted_fs[index].sb = sb;

    graph_node_t *path_node = get_node_by_path(path);

    for (size_t i = 1; i < sb_data->files; i++) {
        struct dentry *temp_de = kmalloc(sizeof(struct dentry), 0, 0);
        temp_de->name = kmalloc(sizeof(char) * 32, 0, 0);
        memcpy(temp_de->name, initrd_headers[i].name, strlen(initrd_headers[i].name));
        temp_de->parent_dentry = de;
        temp_de->inode = &initrd_nodes[i];
        add_dentry(temp_de);
    }

    for (size_t i = 1; i < sb_data->files; i++) {
        struct dentry *temp_de = get_dentry_by_inode(&initrd_nodes[i]);
        size_t addr = (size_t) temp_de;
        inode_write(node, sizeof(size_t), 1, (char*) &addr);

        graph_node_t *temp_node = graph_create((void*) temp_de);
        graph_add_node(path_node, temp_node);
    }
}

int initrd_read(struct inode *node, size_t sz, int n, char *buf)
{
    size_t i;
    size_t phys = node->block + node->offset;
    size_t limit = node->block + node->length;

    for (i = 0; i < sz * n && phys + i < limit; i++) {
        buf[i] = ((char*) phys)[i];
    }
    node->offset += i;
    return i;
}

int initrd_open(struct inode *node, size_t flags)
{
    printk("unused %x %x\n", node, flags);
    return -1;
}

int initrd_close(struct inode *node)
{
    printk("unused %x\n", node);
    return -1;
}

int initrd_rewind(struct inode *node)
{
    node->offset = 0;
    return 0;
}
