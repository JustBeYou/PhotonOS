PhotonOS X-MAS Challenge
===

## How to build this? 
1. Download the cross compiler from https://drive.google.com/file/d/0Bw6lG3Ej2746STJaM2dNbC05elE/view?usp=sharing
2. Copy the cross compiler to ~/opt (it already has the desired directory structure)
3. Install GRUB 2 (for `grub2-mkrescue` command), xorriso and NASM
4. If you want to run it too, you will need `qemu-system-i386` 
5. `make && make install` should create the `photonos.iso` file
6. `make run` will start qemu with the PhotonOS image

## Observations
1. Don't try to run it directly with the kernel image (`photonos.elf`) as it is dependent on GRUB.
2. It should work fine over the network with pwntools, netcat or telnet (`\r` or `\n` are both valid line terminators)
