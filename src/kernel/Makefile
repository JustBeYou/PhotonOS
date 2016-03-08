.PHONY: all clean install

CFLAGS= -O0 -g -ffreestanding -Wall -Wextra -fstack-protector-all
CPPFLAGS= -DBITMAP_FRAME_ALLOCATOR -D__is_photon_kernel -D_TEXTMODE
LDFLAGS=
LIBS= -nostdlib -lgcc
INCLUDE= -I./ -I../include -I../libc/include

OBJS=\
cpu.o \
heap.o \
io.o \
process.o \
shell.o \
stack_protector.o \
syscalls.o \
system.o \
time.o \
ui.o \
vga.o \

all: libkernel.a

libkernel.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS) $(INCLUDE)

clean:
	rm -f $(OBJS)
	rm libkernel.a

install:
	cp libkernel.a ../sysroot/usr/lib
	mkdir -p ../sysroot/usr/include/kernel
	cp ../include/kernel/*.* ../sysroot/usr/include/kernel
	cp ./*.h ../sysroot/usr/include
