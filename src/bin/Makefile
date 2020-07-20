.PHONY: all clean install

all: initrd_creator

initrd_creator:
	gcc initrd_creator.c -o initrd_creator -std=c99

install: initrd_creator
	./initrd_creator f1.txt f2.txt help.txt colors.txt testfile.txt
	cp initrd ../sysroot/boot

clean:
	rm -f initrd_creator.o initrd_creator
