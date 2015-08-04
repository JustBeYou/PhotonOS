cp kernel/newos.elf sysroot/boot
grub-mkrescue -o newos.iso sysroot
