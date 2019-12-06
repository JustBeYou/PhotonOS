.PHONY: all clean install

CFLAGS= -O0 -g -ffreestanding -Wall -Wextra -fstack-protector-all
CPPFLAGS= -fno-exceptions -fno-rtti -DBITMAP_FRAME_ALLOCATOR -D__is_photon_kernel -D_SERIALMODE #-D_TEXTMODE
LDFLAGS= -L../libc
LIBS= -nostdlib -lgcc -lk
INCLUDE= -I./ -I../include -I../kernel -I../libc/include

OBJS=\
icxxabi.o \
kernel_class.o \
main.o \

all: $(OBJS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS) $(INCLUDE)

%.o: %.cpp
	$(CPP) -c $< -o $@ $(CFLAGS) $(CPPFLAGS) $(INCLUDE)

clean:
	rm -rf $(OBJS)

install:
	cp ./*.h ../sysroot/usr/include
