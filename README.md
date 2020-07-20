# PhotonOS
Small *nix-like kernel, written in C and a grain of x86 Assembly. It served me in the journey of understanding low level concepts about how computers work under the hood.
Credits to [Osdev Wiki](https://wiki.osdev.org/Expanded_Main_Page) for the best resources out there.
![](./Screenshot from 2020-07-20 13-58-06.png) ![](./Screenshot from 2020-07-20 13-58-39.png)

# Motivation
* Tried to join [The Edyptula Challenge](http://eudyptula-challenge.org/) and failed
* Could not understand what CPUs actually do, so I've decided to get closer with them :wink:
* C pointers were intimidating monsters to me... Until PhotonOS was born
* To roll your own Linux Distro was to mainstream :sunglasses:

# Features
* Text-mode video driver
* Keyboard, PIC drivers
* Memory management using paging
* User space support and a small version of LIBC
* System calls
* Multitasking (kind of supported, but it is not used anywhere other than tests)
* Virtual file system and initial RAM disk
* Some self testing procedures

# How to build it? 
You can find some older releases alredy built, but if you want to play with the last version you need to:

1. Download the cross compiler from https://drive.google.com/file/d/0Bw6lG3Ej2746STJaM2dNbC05elE/view?usp=sharing
2. Copy the cross compiler to ~/opt (it already has the desired directory structure)
3. Install GRUB 2 (for `grub2-mkrescue` command), xorriso and NASM
4. If you want to run it too, you will need `qemu-system-i386` 
5. `make && make install` should create the `photonos.iso` file
6. `make run` will start qemu with the PhotonOS image

**Don't try to run it directly with the kernel image (`photonos.elf`) as it is dependent on GRUB.**

# Feedback and contributing
It has been a dead project for the past years, but if you have any feedback or if you are interested about learning OS development, don't hesitate to drop me an email at mihailferaru2000@gmail.com or contact me on [Linkedin](https://www.linkedin.com/in/mihail-feraru/)
