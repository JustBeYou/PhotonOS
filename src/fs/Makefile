.PHONY: all clean install

CFLAGS= -O0 -g -ffreestanding -Wall -Wextra -fstack-protector-all
CPPFLAGS= -DBITMAP_FRAME_ALLOCATOR -D__is_photon_kernel -D_TEXTMODE
LDFLAGS=
LIBS= -nostdlib -lgcc -lk
INCLUDE= -I./ -I../include -I../kernel -I../libc/include

OBJS=\
vfs.o \
initrd.o

all: libfs.a

libfs.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS) $(INCLUDE)

clean:
	rm -f $(OBJS)
	rm libfs.a

install:
	cp libfs.a ../sysroot/usr/lib
	mkdir -p ../sysroot/usr/include/fs
	cp ../include/fs/*.* ../sysroot/usr/include/fs
