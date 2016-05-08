CFLAGS= -O2 -g -Wall -Wextra -Iinclude -I../include -I../kernel
CPPFLAGS= -D__is_photon_libc -Iinclude -I../include -I../kernel
LIBK_CFLAGS= -ffreestanding -Iinclude -I../include -I../kernel
LIBK_CPPFLAGS= -D__is_photon_kernel -Iinclude -I../include -I../kernel

ARCHDIR= arch/i386

include $(ARCHDIR)/make.config

CFLAGS:=$(CFLAGS) $(ARCH_CFLAGS)
CPFLAGS:=$(CPPFLAGS) $(ARCH_CPPFLAGS)
LIBK_CFLAGS:=$(LIBK_CFLAGS) $(KERNEL_ARCH_CFLAGS)
LIBK_CPFLAGS:=$(LIBK_CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)

FREEOBJS:=\
$(ARCH_FREEOBJS) \
stdlib/abort.o \
stdlib/intlen.o \
stdlib/atoi.o \
stdlib/itoa.o \
stdlib/malloc.o \
stdlib/realloc.o \
stdlib/calloc.o \
stdlib/free.o \
string/memcmp.o \
string/memcpy.o \
string/memmove.o \
string/memset.o \
string/memsetw.o \
string/strcmp.o \
string/strlen.o \
string/strrev.o \
phapi/list.o \
phapi/linked_list.o \
phapi/bitmap.o \
phapi/graph.o \
phapi/stack.o \

HOSTEDOBJS:=\
$(ARCH_HOSTEDOBJS) \
stdlib/exit.o \
stdio/getch.o \
stdio/getchar.o \
stdio/gets.o \
stdio/printf.o \
stdio/putchar.o \
stdio/puts.o \

OBJS:=\
$(FREEOBJS) \
$(HOSTEDOBJS) \

LIBK_OBJS:=$(FREEOBJS:.o=.libk.o)

BINARIES=libg.a libk.a $(ARCHDIR)/crt0.o $(ARCHDIR)/crti.o $(ARCHDIR)/crtn.o # add libc.a when user space is ready

all: $(BINARIES)

.PHONY: all clean install install-headers install-libs

libc.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

libg.a:
	$(AR) rcs $@

libk.a: $(LIBK_OBJS)
	$(AR) rcs $@ $(LIBK_OBJS)

%.o: %.c
	$(CC) -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

%.o: %.S
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

%.o: %.asm
	$(NAS) -felf32 $< -o $@

%.libk.o: %.c
	$(CC) -c $< -o $@ -std=gnu11 $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

%.libk.o: %.S
	$(CC) -c $< -o $@ $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

%.libk.o: %.asm
	$(NAS) -felf32 $< -o $@

clean:
	rm -f $(BINARIES) $(OBJS) $(LIBK_OBJS) *.o */*.o */*/*.o

install:
	# cp libc.a ../sysroot/usr/lib
	cp libg.a ../sysroot/usr/lib
	cp libk.a ../sysroot/usr/lib
	cp -r include/* ../sysroot/usr/include
