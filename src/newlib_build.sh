# newlib setup
export PATH="/home/littlewho/opt/cross/bin:$PATH"
CURRDIR=$(pwd)

mkdir build-newlib
cd build-newlib
../newlib/configure --prefix=/usr --target=i686-photonos
make all
make DESTDIR=/home/littlewho/Desktop/sourcecode/photon/devel/src/sysroot install
