#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

#include <kernel/io.h>
#include <kernel/process.h>
#include <kernel/shell.h>
#include <kernel/time.h>
#include <kernel/ui.h>
#include <kernel/vga.h>
#include <kernel/heap.h>
#include <fs/fcntl.h>
#include <fs/vfs.h>

extern int detect_cpu();

extern uint32_t free_frames;
extern uint32_t mapped_pages;
extern mem_heap_t *kernel_heap;
extern uint32_t nframes;

uint32_t get_memory()
{
    return ((nframes * 4) / 1024);
}

uint32_t get_mapped_mem()
{
    return ((mapped_pages * 4) / 1024);
}

void cmd_help()
{
    printk("""Help:\n \
-> hw - print hardware info\n \
-> time - display the time\n \
-> free - display info about memory\n \
-> clear - clear screen\n \
-> reboot - reboot the PC\n \
-> shutdown - shutdown the PC\n \
-> top - print processes info\n \
-> read - test read function\n \
""");
}

void cmd_hw()
{
    detect_cpu();
}

void cmd_time()
{
    print_time();
}

void cmd_free()
{
    printk("Toatal memory: %d MiB (%d pages)\n", get_memory(), nframes);
    printk("Mapped memory: %d MiB (%d pages) Free memory: %d MiB\n",
            get_mapped_mem(),
            mapped_pages,
            get_memory() - get_mapped_mem());
    printk("Usable memory (allocated only for kernel): %d MiB = %d Bytes\n",
            kernel_heap->mem_size / 1024 / 1024,
            kernel_heap->mem_size);
    printk("Usable memory (allocated for user-space): %d MiB (%s)\n",
            0,
            "User-space isn't available!");
    printk("Allocated memory: %d MiB Free memory: %d MiB\n",
            kernel_heap->mem_used / 1024 / 1024,
            kernel_heap->mem_free / 1024 / 1024);
    printk("Allocated memory: %d Bytes Free memory: %d Bytes\n",
            kernel_heap->mem_used,
            kernel_heap->mem_free);
}

void cmd_clear()
{
    clear_vga();
    logo();
}

void cmd_reboot()
{
    clear_vga();
    printk("System will go down and reboot...\n");
    reboot();
}

void cmd_dbg()
{

}

void cmd_shutdown()
{
    printk("System go down...\n");
    outw(0xB004, 0x2000);
}

extern process_t *start_process;

void cmd_top()
{
    printk("Running processes:\n");
    process_t *tmp_process = start_process;
    while (tmp_process) {
        printk("PID: %x Name: %s Stacks: %x %x\n",
            tmp_process->pid,
            tmp_process->name,
            tmp_process->kern_stack,
            tmp_process->user_stack);
        tmp_process = tmp_process->next;
    }
}

void cmd_test_read()
{
    int fd1 = kopen("/f1.txt", O_RDONLY);
    int fd2 = kopen("/f2.txt", O_RDONLY);
    int fd3 = kopen("/help.txt", O_RDONLY);

    char buf[512];
    memset(buf, 0, 512);

    kread(fd1, buf, 512);
    printk("f1.txt: %s\n", buf);
    memset(buf, 0, 512);
    kread(fd2, buf, 512);
    printk("f2.txt: %s\n", buf);
    memset(buf, 0, 512);
    kread(fd3, buf, 512);
    printk("help.txt: %s\n", buf);
    memset(buf, 0, 512);
}

int cmd_limit = 10;

shell_cmd_t cmd_table[] = {
    {"help",  cmd_help},
    {"hw",    cmd_hw},
    {"time",  cmd_time},
    {"free",  cmd_free},
    {"clear", cmd_clear},
    {"reboot", cmd_reboot},
    {"dbg", cmd_dbg},
    {"shutdown", cmd_shutdown},
    {"top", cmd_top},
    {"test_read", cmd_test_read}
};

int shell(char *cmd)
{
    for (int i = 0; i < cmd_limit; i++) {
        if (!strcmp(cmd, cmd_table[i].name)) {
            cmd_table[i].func();
            return 0;
        }
    }
    return 1;
}
