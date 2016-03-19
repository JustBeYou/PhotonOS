# newlib setup
export PATH="/home/littlewho/opt/cross/bin:$PATH"
CURRDIR=$(pwd)
 
# make symlinks (a bad hack) to make newlib work
cd ~/opt/cross/bin/ # this is where the bootstrapped generic cross compiler toolchain (i686-elf-xxx) is installed in,
                # change this based on your development environment.
ln i686-elf-ar i686-photonos-ar
ln i686-elf-as i686-photonos-as
ln i686-elf-gcc i686-photonos-gcc
ln i686-elf-gcc i686-photonos-cc
ln i686-elf-ranlib i686-photonos-ranlib
 
# return
cd $CURRDIR

mkdir build-newlib
cd build-newlib
../newlib/configure --prefix=/usr --target=i686-photonos
sudo make all
sudo make install
