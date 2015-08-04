#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/photon.elf isodir/boot/photon.elf
tar --verbose --create --file isodir/boot/photon.initrd --directory=sysroot $(ls sysroot | grep -v boot)
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "photon" {
	multiboot /boot/photon.elf
	module /boot/photon.initrd
}
EOF
grub-mkrescue -o photon.iso isodir
