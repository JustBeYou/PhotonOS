#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

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
    printk("WHEN MULTI-TASKING WILL BE AVAILABLE THIS SHOULD GO INTO KERNEL THREAD BASED SHELL AND KILL USER SHELL TASK.\n");
}

int cmd_limit = 6;

shell_cmd_t cmd_table[] = {
    {"help",  cmd_help},
    {"hw",    cmd_hw},
    {"time",  cmd_time},
    {"free",  cmd_free},
    {"clear", cmd_clear},
    {"reboot", cmd_reboot},
    {"dbg", cmd_dbg}
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
