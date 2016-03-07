.PHONY: all clean install

CFLAGS= -O0 -g -ffreestanding -Wall -Wextra -fstack-protector-all
CPPFLAGS= -DBITMAP_FRAME_ALLOCATOR -D__is_photon_kernel -D_TEXTMODE
LDFLAGS=
LIBS= -nostdlib -lgcc
INCLUDE= -I./ -I../../kernel -I../../include -I../../libc/include

OBJS=\
a20.o \
descriptor_tables.o \
gdt.o \
handlers.o \
idt.o \
interrupt.o \
pic.o \
pit.o \
pmm.o \
regs.o \
userspace.o \
vmm.o

all: libarch.a boot/boot.o

libarch.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS) $(INCLUDE)

%.o: %.S
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

%.o: %.asm
	$(NAS) -felf32 $< -o $@ -g

clean:
	rm $(OBJS)
	rm libarch.a

install:
	cp libarch.a ../../sysroot/usr/lib
	mkdir -p ../../sysroot/usr/include/i386
	cp ../../include/i386/*.* ../../sysroot/usr/include/i386
