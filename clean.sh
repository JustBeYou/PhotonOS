set -e
.	./configure.sh

rm -f -r sysroot
cd kernel
make clean
cd ..
cd libc
make clean
cd ..
rm osdev.iso
