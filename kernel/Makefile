HOST?=$(shell ../default-host.sh)
HOSTARCH:=$(shell ../target-triplet-to-arch.sh $(HOST))

CFLAGS?=-O0 -g
CPPFLAGS?= 
LDFLAGS?=
LIBS?= -L../libc

DESTDIR?=
PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
BOOTDIR?=$(EXEC_PREFIX)/boot
INCLUDEDIR?=$(PREFIX)/include

CFLAGS:=$(CFLAGS) -ffreestanding -Wall -Wextra
CPPFLAGS:=$(CPPFLAGS) -DBITMAP_FRAME_ALLOCATOR -D__is_photon_kernel -D_TEXTMODE -Iinclude -Iarch/i386/include -I../libc/include
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS) -nostdlib -lk -lgcc

ARCHDIR:=arch/$(HOSTARCH)

include $(ARCHDIR)/make.config

CFLAGS:=$(CFLAGS) $(KERNEL_ARCH_CFLAGS)
CPFLAGS:=$(CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)
LDFLAGS:=$(LDFLAGS) $(KERNEL_ARCH_LDFLAGS)
LIBS:=$(LIBS) $(KERNEL_ARCH_LIBS)

OBJS:=\
init/icxxabi.o \
init/kernel_class.o \
$(KERNEL_ARCH_OBJS) \
system/cpu.o \
drivers/keyboard.o \
drivers/io.o \
drivers/vga.o \
init/init.o \
init/main.o \
system/system.o \
system/time.o \
system/task.o \
system/kheap.o \

CRTI_OBJ:=$(ARCHDIR)/crti.o
CRTBEGIN_OBJ:=$(shell $(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtend.o)
CRTN_OBJ:=$(ARCHDIR)/crtn.o

ALL_OUR_OBJS:=\
$(CRTI_OBJ) \
$(OBJS) \
$(CRTN_OBJ) \

OBJ_LINK_LIST:=\
$(CRTI_OBJ) \
$(CRTBEGIN_OBJ) \
$(OBJS) \
$(CRTEND_OBJ) \
$(CRTN_OBJ) \

all: photon.elf

.PHONY: all clean install install-headers install-kernel

photon.elf: $(OBJ_LINK_LIST) $(ARCHDIR)/linker.ld
	echo "Linking kernel..."
	$(CPP) -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(OBJ_LINK_LIST) $(LDFLAGS) $(LIBS)
	echo "Kernel linked!"

%.o: %.c
	$(CC) -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)
	
%.o: %.cpp
	i686-elf-g++ -c $< -o $@ -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -D__is_photon_kernel -Iinclude -Iarch/i386/include -I../libc/include

%.o: %.S
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)
	
%.o: %.asm
	$(NAS) -felf32 $< -o $@

clean:
	rm -f photon.elf $(OBJS) $(ALL_OUR_OBJS) *.o */*.o */*/*.o

install: install-headers install-kernel

install-headers:
	echo "Installing headers..."
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -RTv include $(DESTDIR)$(INCLUDEDIR)
	cp -RTv $(ARCHDIR)/include $(DESTDIR)$(INCLUDEDIR)
	echo "Headers installed!"

install-kernel: photon.elf
	echo "Installing kernel..."
	mkdir -p $(DESTDIR)$(BOOTDIR) 
	cp photon.elf $(DESTDIR)$(BOOTDIR)
	echo "Kernel installed!"
