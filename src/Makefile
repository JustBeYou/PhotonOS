.PHONY: all rm_elf compile clean install run

export NAS=nasm
export AR=~/opt/cross/bin/i686-elf-ar
export AS=~/opt/cross/bin/i686-elf-as
export CC=~/opt/cross/bin/i686-elf-gcc
export CPP=~/opt/cross/bin/i686-elf-g++

CFLAGS= -O0 -g -ffreestanding -Wall -Wextra -fstack-protector-all
LIBS= -Llibc -Larch/i386 -Ldrivers -Lfs -Lkernel
LDFLAGS= -nostdlib -lk -lgcc -larch -ldrivers -lfs -lkernel

OBJS=\
init/icxxabi.o \
init/kernel_class.o \
init/main.o \
arch/i386/boot/boot.o

all: rm_elf compile photon.elf

rm_elf:
	rm -rf photon.elf

photon.elf:
	$(CPP) -T arch/i386/linker.ld -o $@ $(OBJS) $(CFLAGS) $(LDFLAGS) $(LIBS) libc/libk.a

compile:
	cd libc && make && cd ../
	cd kernel && make && cd ../
	cd fs && make && cd ../
	cd drivers && make && cd ../
	cd init && make && cd ../
	cd bin && make && cd ../
	cd arch/i386 && make && cd ../../

clean:
	rm -rf sysroot
	rm -f photon.iso
	rm -f photon.elf
	rm -f bin/initrd
	rm -rf isodir

	cd kernel && make clean && cd ../
	cd fs && make clean && cd ../
	cd drivers && make clean && cd ../
	cd init && make clean && cd ../
	cd bin && make clean && cd ../
	cd arch/i386 && make clean && cd ../../
	cd libc && make clean && cd ../

install:
	rm -rf sysroot
	rm -rf isodir
	rm -f photon.iso

	mkdir -p sysroot
	mkdir -p sysroot/usr
	mkdir -p sysroot/usr/lib
	mkdir -p sysroot/usr/include
	mkdir -p sysroot/boot
	cp photon.elf sysroot/boot

	cd kernel && make install && cd ../
	cd fs && make install && cd ../
	cd drivers && make install && cd ../
	cd init && make install && cd ../
	cd bin && make install && cd ../
	cd arch/i386 && make install && cd ../../
	cd libc && make install && cd ../

	./iso.sh

run:
	qemu-system-i386 -m 20M -nographic -cdrom photon.iso
