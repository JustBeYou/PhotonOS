.PHONY: all clean install install-headers install-libs

all: install install-headers libsys.a install-libs newos.elf

newos.elf: $(BOOT_OBJS) $(INIT_OBJS)
	$(CC) $(LDFLAGS) $(BOOT_OBJS) $(INIT_OBJS) $(LIBS) -o newos.elf

libsys.a: $(SYS_OBJS)
	$(AR) rcs $@ $(SYS_OBJS) $(LIBK_OBJS)

%.o: %.asm
	$(AS) $< -o $@ $(ASMFLAGS)
	
%.o: %.c
	$(CC) $< -o $@ $(CFLAGS) $(INCLUDE)

install:
	mkdir ../sysroot/usr
	mkdir ../sysroot/boot
	mkdir ../sysroot/boot/grub
	touch ../sysroot/boot/grub/grub.cfg
	echo "menuentry \"myos\" {\n\tmultiboot /boot/newos.elf\n}" > ../sysroot/boot/grub/grub.cfg
	
install-headers:
	mkdir ../sysroot/usr/include
	cp include/. ../sysroot/usr/include -R
	cp ../libc/include/. ../sysroot/usr/include -R
	
install-libs:
	mkdir ../sysroot/usr/lib
	mv libsys.a ../sysroot/usr/lib
	mv ../libc/libc.a ../sysroot/usr/lib
	
clean:
	rm -f $(INIT_OBJS) $(BOOT_OBJS) $(SYS_OBJS)
	rm -f newos.elf
