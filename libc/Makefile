.PHONY: all clean

all: libc.a

libc.a: $(LIBC_OBJS)
	$(AR) rcs $@ $(LIBC_OBJS)
	
%.o: %.c
	$(CC) -c $< -o $@ -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Iinclude/ -I../kernel/include -g
	
clean:
	rm -f $(LIBC_OBJS)
