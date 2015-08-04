# path
export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

# compilers, assembler and linker
export CC="i686-elf-gcc"
export AS="nasm"
export AR="i686-elf-ar"

# libc & libk flags and objects
export LIBC_OBJS="stdio/putchar.o stdio/printf.o stdio/getchar.o stdio/gets.o stdio/getch.o string/strlen.o string/strrev.o string/memsetw.o string/memset.o string/memcpy.o string/strcmp.o stdlib/itoa.o stdlib/intlen.o ordered_array.o"

export LIBK_OBJS="../libc/stdio/putchar.o ../libc/stdio/printf.o ../libc/stdio/getchar.o ../libc/stdio/gets.o ../libc/stdio/getch.o ../libc/string/strlen.o ../libc/string/strrev.o ../libc/string/memsetw.o ../libc/string/memset.o ../libc/string/memcpy.o ../libc/string/strcmp.o ../libc/stdlib/itoa.o ../libc/stdlib/intlen.o ../libc/ordered_array.o"

# kernel flags and objects
export INCLUDE="-I../sysroot/usr/include"
export LIBS="-L../sysroot/usr/lib -lsys -lgcc"
export CFLAGS="-c -std=gnu99 -ffreestanding -O2 -Wall -Wextra -g"
export ASMFLAGS="-felf32"
export LDFLAGS="-T init/linker.ld -ffreestanding -O2 -nostdlib"

export BOOT_OBJS="boot/boot.o"
export INIT_OBJS="init/main.o init/system.o init/time.o init/init.o"
export IO_OBJS="io/io.o io/vga.o io/tty.o io/syscalls.o"
export CPU_OBJS="cpu/gdt.o cpu/idt.o cpu/isr.o cpu/irq.o cpu/descriptor_tables.o cpu/interrupt.o cpu/task.o cpu/usermode.o"
export DEVICES_OBJS="devices/pit.o devices/keyboard.o"
export MEM_OBJS="mem/kheap.o mem/paging.o"
export SYS_OBJS="$CPU_OBJS $DEVICES_OBJS $MEM_OBJS $IO_OBJS"
