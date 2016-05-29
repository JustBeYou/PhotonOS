.PHONY: all clean install

CFLAGS= -O0 -g -ffreestanding -Wall -Wextra -fstack-protector-all
CPPFLAGS= -DBITMAP_FRAME_ALLOCATOR -D__is_photon_kernel -D_TEXTMODE
LDFLAGS=
LIBS= -nostdlib -lgcc
INCLUDE= -I./ -I../include -I../kernel -I../libc/include

OBJS=\
keyboard.o \

all: libdrivers.a

libdrivers.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS) $(INCLUDE)

clean:
	rm $(OBJS)
	rm libdrivers.a

install:
	cp libdrivers.a ../sysroot/usr/lib
	mkdir -p ../sysroot/usr/include/drivers
	cp ../include/drivers/*.* ../sysroot/usr/include/drivers
