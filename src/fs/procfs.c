#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

#include <fs/procfs.h>
#include <fs/vfs.h>
#include <kernel/io.h>
#include <kernel/heap.h>

int inode_id;
procfs_super_block_t procfs_sb;

extern char cwd[4096];
extern struct dentry *cwd_de;

void procfs_init()
{
    inode_id = 0;
    procfs_sb.sb_struct_size = sizeof(procfs_super_block_t);
    procfs_sb.fh_struct_size = sizeof(procfs_file_header_t);
}

struct inode *procfs_alloc_inode(uint32_t flags, uint32_t length)
{
    struct inode *node = kmalloc(sizeof(struct inode), 0, 0);
    inode_init(node, flags, inode_id++, length, 0, 0, 0, PROCFS_TYPE);
    procfs_alloc(node, length);
    node->write = &procfs_write;
    node->read = &procfs_read;
    node->rewind = &procfs_rewind;
    node->open = &procfs_open;
    node->close = &procfs_close;
    node->create = &procfs_create;

    return node;
}

void procfs_alloc(struct inode *node, uint32_t length)
{
    if (!node->block || !node->length) {
        node->block = kmalloc(length + procfs_sb.fh_struct_size, 0, 0);
        node->length = length + procfs_sb.fh_struct_size;

        void *addr = (void*) node->block;
        memset(addr, 0, length + procfs_sb.fh_struct_size);
    } else {
        void *addr = (void*) node->block;
        node->block = krealloc(addr, length + procfs_sb.fh_struct_size);
        node->length = length + procfs_sb.fh_struct_size;
    }
    procfs_file_header_t *addr = (procfs_file_header_t*) node->block;
    addr->length = length;
}

int procfs_create(struct inode *parent, char *name, uint32_t flags)
{
    struct dentry *parent_de = get_dentry_by_inode(parent);
    if (parent_de == NULL) {
        return -1;
    }

    inode_t *new_inode = procfs_alloc_inode(flags, 1024);
    struct dentry *new_de = kmalloc(sizeof(struct dentry), 0, 0);

    size_t namelen = strlen(name);
    new_de->name = kmalloc(sizeof(char) * namelen + 1, 0, 0);
    memcpy(new_de->name, name, namelen);
    new_de->name[namelen] = '\0';

    new_de->parent_dentry = parent_de;
    new_de->inode = new_inode;
    add_dentry(new_de);

    uint32_t addr = &new_de;
    inode_rewind(parent);
    uint32_t temp_addr;
    inode_read(parent, sizeof(uint32_t), 1, (char*) temp_addr);
    while (temp_addr != 0x0) {
        inode_read(parent, sizeof(uint32_t), 1, (char*) temp_addr);
    }
    parent->offset += sizeof(uint32_t);

    inode_write(parent, sizeof(uint32_t), 1, (char*) addr);

    graph_node_t *new_node = graph_create((void*) new_de);
    graph_node_t *parent_node = get_node_by_path(cwd);
    graph_add_node(parent_node, new_node);

    return 0;
}

int procfs_write(struct inode *node, size_t sz, int n, char *buf)
{
    uint32_t i;
    uint32_t phys = node->block + node->offset + procfs_sb.fh_struct_size;
    uint32_t limit = node->block + node->length + procfs_sb.fh_struct_size;

    if (limit - phys < sz * n) {
        procfs_alloc(node, node->length + sz * n);
        phys = node->block + node->offset + procfs_sb.fh_struct_size;
        limit = node->block + node->length + procfs_sb.fh_struct_size;
    }

    for (i = 0; i < sz * n && phys + i < limit; i++) {
        ((char*) phys)[i] = buf[i];
    }
    node->offset += i;
    return i;
}

int procfs_read(struct inode *node, size_t sz, int n, char *buf)
{
    uint32_t i;
    uint32_t phys = node->block + node->offset + procfs_sb.fh_struct_size;
    uint32_t limit = node->block + node->length + procfs_sb.fh_struct_size;

    for (i = 0; i < sz * n && phys + i < limit; i++) {
        buf[i] = ((char*) phys)[i];
    }
    node->offset += i;
    return i;
}

int procfs_open(struct inode *node, uint32_t flags)
{
    return 0;
}

int procfs_close(struct inode *node)
{
    return 0;
}

int procfs_rewind(struct inode *node)
{
    node->offset = 0;
    return 0;
}
