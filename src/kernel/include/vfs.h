#ifndef _vfs_h
#define _vfs_h

#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

#define PATH_SEPARATOR '/'
#define PATH_SEPARATOR_STRING "/"
#define PATH_UP ".."
#define PATH_CURRENT "."

#define FS_FILE       0x01
#define FS_DIRECTORY  0x02
#define FS_DEVICE     0x04
#define FS_PIPE       0x08
#define FS_SYMLINK    0x10
#define FS_MOUNTPOINT 0x20
#define FS_META       0x40

#define O_RDONLY  0x01
#define O_WRONLY  0x02
#define O_RDWR    0x04
#define O_APPEND  0x08
#define O_EXCL    0x10

#define MAX_KERNEL_FD 256

struct fs_node;
struct dirent;
struct stat;
struct vfs_entry;

typedef int (*fs_read_t)(struct fs_node *node, size_t sz, int n, char *buf);
typedef int (*fs_write_t)(struct fs_node *node, size_t sz, int n, char *buf);
typedef int (*fs_rewind_t)(struct fs_node *node);
typedef int (*fs_open_t)(struct fs_node *node, uint32_t flags);
typedef int (*fs_close_t)(struct fs_node *node);
typedef struct dirent *(*fs_readdir_t)(struct fs_node *node);
typedef struct fs_node *(*fs_finddir_t)(struct fs_node *node, char *name);
typedef int (*fs_create_t)(struct fs_node *parent, char *name, uint32_t flags);
typedef int (*fs_mkdir_t)(struct fs_node *parent, char *name, uint32_t flags);
typedef int (*fs_ioctl_t)(struct fs_node *node, int req, void *argp);
typedef size_t (*fs_getsize_t)(struct fs_node *node);
typedef int (*fs_chmod_t)(struct fs_node *node, uint32_t mode);
typedef int (*fs_unlink_t)(struct fs_node *node);
typedef int (*fs_symlink_t)(struct fs_node *node, struct fs_node *link);
typedef int (*fs_readlink_t)(struct fs_node *node);

typedef struct fs_node {
    char name[256];       // Node name
    void *device;         // Device where node is found
    uint32_t mask;        // Node mask
    uint32_t uid;         // User ID
    uint32_t gid;         // Group ID
    uint32_t flags;       // Node flags
    uint32_t inode;       // Node ID
    size_t length;        // Node content length
    uint32_t impl;
    uint32_t open_flags;  // Flags for open
    size_t count;         // Cursor position
    
    uint32_t atime;       // Accessed time
    uint32_t mtime;       // Modified time
    uint32_t ctime;       // Created time
    
    // Functions
    fs_read_t      read;
    fs_write_t     write;
    fs_rewind_t    rewind;
    fs_open_t      open;
    fs_close_t     close;
    fs_readdir_t   readdir;
    fs_finddir_t   finddir;
    fs_create_t    create;
    fs_mkdir_t     mkdir;
    fs_ioctl_t     ioctl;
    fs_getsize_t   getsize;
    fs_chmod_t     chmod;
    fs_unlink_t    unlink;
    fs_symlink_t   symlink;
    fs_readlink_t  readlink;
    
    // Link and reference
    struct fs_node *link;
    int32_t refcount;
    uint32_t nlink;
} fs_node_t;

struct dirent {
    uint32_t ino;
    char name[256];
};

struct stat {
    uint16_t st_dev;
    uint16_t st_ino;
    uint32_t st_mode;
    uint16_t st_nlink;
    uint16_t st_uid;
    uint16_t st_gid;
    uint16_t st_rdev;
    uint32_t st_size;
    uint32_t st_atime;
    uint32_t __unused1;
    uint32_t st_mtime;
    uint32_t __unused2;
    uint32_t st_ctime;
    uint32_t __unused3;
};

typedef struct vfs_entry {
    char *name;
    fs_node_t *node;
} vfs_entry_t;

void init_vfs();
int fs_read(struct fs_node **node, size_t sz, int n, char *buf);
int fs_get_fd(fs_node_t *node); // Not in struct.
int fs_write(struct fs_node **node, size_t sz, int n, char *buf);
int fs_rewind(struct fs_node *node);
int fs_open(struct fs_node *node, uint32_t flags);
int fs_close(struct fs_node *node);
struct dirent *fs_readdir(struct fs_node *node);
struct fs_node *fs_finddir(struct fs_node *node, char *name);
int fs_create(struct fs_node *node, char *name, uint32_t flags);
int fs_mkdir(struct fs_node *node, char *name, uint32_t flags);
int fs_ioctl(struct fs_node *node, int req, void *argp);
size_t fs_getsize(struct fs_node *node);
int fs_chmod(struct fs_node *node, uint32_t mode);
int fs_unlink(struct fs_node *node);
int fs_symlink(struct fs_node *node, struct fs_node *link);
int fs_readlink(struct fs_node *node);

#endif
