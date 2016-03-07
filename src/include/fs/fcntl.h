#ifndef _fcntl_h
#define _fcntl_h

#include <system.h>

#define O_RDONLY  0x0
#define O_WRONLY  0x1
#define O_RDWR    0x2
#define O_EXCL    0x4
#define O_CREAT 0x8
#define O_EXECL 0x10
#define O_NOCITY 0x20
#define O_TRUNC 0x40
#define O_APPEND 0x80
#define O_DSYNC 0x200
#define O_NONBLOCK 0x400
#define O_RSYNC 0x800
#define O_SYNC 0x1000
#define O_ACCMODE 0x2000

typedef unsigned short mode_t;

int creat(const char *, mode_t);
int fcntl(int, int, ...);
int open(const char *, int, ...);

#endif
