set -e
.	./configure.sh

mkdir sysroot

cd libc
make clean
make

cd ..
cd kernel
make clean
make
