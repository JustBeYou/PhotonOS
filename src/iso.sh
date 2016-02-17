#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/usr
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp -r sysroot/usr/* isodir/usr
cp -r sysroot/boot/* isodir/boot/
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "photon" {
	multiboot /boot/photon.elf
    module /boot/initrd
}
EOF
grub-mkrescue -o photon.iso isodir
