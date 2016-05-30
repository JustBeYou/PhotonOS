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
#include <kernel/syscalls.h>
#include <fs/fcntl.h>
#include <fs/vfs.h>

char cmd_args[4096];

extern int detect_cpu();

extern size_t free_frames;
extern size_t mapped_pages;
extern mem_heap_t *kernel_heap;
extern size_t nframes;

size_t get_memory()
{
    return ((nframes * 4) / 1024);
}

size_t get_mapped_mem()
{
    return ((mapped_pages * 4) / 1024);
}

void cmd_help()
{
    int fd = open("/mnt/initrd/help.txt", O_RDONLY);
    char c;

    int newlines = 0;
    while (read(fd, &c, 1) != 0) {
        printk("%c", c);
        if (c == '\n') {
            newlines++;
        }

        if (newlines >= 10) {
            kb_read_char();
            newlines = 0;
        }
    }
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
    //int fd1 = open("/mnt/initrd/f1.txt", O_RDONLY);
    int fd2 = kopen("/mnt/initrd/f2.txt", O_RDONLY);
    int fd3 = kopen("/mnt/initrd/help.txt", O_RDONLY);

    char buf[512];
    memset(buf, 0, 512);

    //read(fd1, buf, 512);
    //printk("/mnt/initrd/f1.txt (%d): %s\n", fd1, buf);
    //memset(buf, 0, 512);
    kread(fd2, buf, 512);
    printk("/mnt/initrd/f2.txt (%d): %s\n", fd2, buf);
    memset(buf, 0, 512);
    kread(fd3, buf, 512);
    printk("/mnt/initrd/help.txt (%d): %s\n", fd3, buf);
    memset(buf, 0, 512);

    //close(fd1);
    kclose(fd2);
    kclose(fd3);
}

void cmd_test_write()
{
    int fd = kopen("/test_file.txt", O_RDWR);

    char buf[512];
    memset(buf, 0, 512);
    memcpy(buf, "This is a message.\0", strlen("This is a message.\0") + 1);

    printk("Buffer: \"%s\" will be written to the file.\n");
    kwrite(fd, buf, strlen(buf));
    krewind(fd);

    memset(buf, 0, 512);
    kread(fd, buf, 512);
    printk("File was written with: %s\n", buf);
    krewind(fd);
    kclose(fd);
}

void cmd_test_syscall()
{
    printk("System calls test\n");
    size_t ret = syscall(0, 0, 0, 0, 0, 0);
    printk("Syscall returned: %d == %d (%d)\n", ret, 123456789, ret == 123456789);
}

extern char cwd[4096];

void cmd_ls()
{
    char target[4096];

    if (cmd_args[0] == '\0') {
        memcpy(target, cwd, strlen(cwd) + 1);
    } else {
        memcpy(target, cmd_args, strlen(cmd_args) + 1);
    }

    DIR *d = kopendir(target);
    if (d == NULL) {
        printk("This isn't a directory.\n");
        return ;
    }

    struct dentry *de = kreaddir(d);

    while (de != NULL) {
        printk("%s ", de->name);
        de = kreaddir(d);
    }
    printk("\n");
    kclosedir(d);
}

void cmd_cd()
{
    if (cmd_args[0] == '\0' || (cmd_args[0] == '/' && cmd_args[1] == '\0')) {
        set_cwd("/");
    } else {
        struct dentry *temp_de = get_dentry_by_path(cmd_args);
        if (temp_de == NULL || temp_de->inode->flags != FS_DIRECTORY) {
            printk("Path: '%s' doesn't exist or isn't a directory.\n", cmd_args);
            return ;
        }
        if (cmd_args[0] != '/') {
            relative_to_absolute(cmd_args);
        }
        set_cwd(cmd_args);
    }
}

void cmd_mkdir()
{

}

void cmd_touch()
{
    if (cmd_args[0] == '\0') {
        printk("You must specify a file name.\n");
        return ;
    }
    int fd = kopen(cmd_args, O_RDONLY);
    if (fd != -1) {
        printk("File already exists.\n");
        kclose(fd);
        return ;
    }

    fd = kopen(cmd_args, O_RDWR);
    kclose(fd);
}

void cmd_write()
{

}

void cmd_echo()
{
    printk("%s\n", cmd_args);
}

void cmd_test_drivers()
{

}

void cmd_test_fs()
{

}

void cmd_test_kernel()
{

}

void cmd_test_libc()
{

}

void cmd_test_os()
{

}

int cmd_limit = 23;

shell_cmd_t cmd_table[] = {
    {"help",  cmd_help},
    {"hw",    cmd_hw},
    {"time",  cmd_time},
    {"free",  cmd_free},
    {"clear", cmd_clear},
    {"reboot", cmd_reboot},
    {"dbg", cmd_dbg}, // not implemented
    {"shutdown", cmd_shutdown},
    {"top", cmd_top},
    {"ls", cmd_ls},
    {"cd", cmd_cd},
    {"mkdir", cmd_mkdir}, // not implemented
    {"touch", cmd_touch}, // not implemented
    {"write", cmd_write}, // not implemented
    {"echo", cmd_echo},
    {"test_read", cmd_test_read},
    {"test_write", cmd_test_write},
    {"test_syscall", cmd_test_syscall},
    {"test_drivers", cmd_test_drivers}, // not implemented
    {"test_fs", cmd_test_fs}, // not implemented
    {"test_kernel", cmd_test_kernel}, // not implemented
    {"test_libc", cmd_test_libc}, // not implemented
    {"test_os", cmd_test_os} // not implemented
};

int shell(char *cmd)
{
    cmd_args[0] = '\0';
    char word[256];
    memset(word, 0, 256);

    int i;
    char c;
    for (i = 0, c = cmd[i]; c != '\0' && c != ' '; i++) {
        c = cmd[i];
    }
    memcpy(word, cmd, i - 1);
    word[i] = '\0';

    if (cmd[i] != '\0') {
        size_t len = strlen((cmd + i));
        memcpy(cmd_args, (cmd + i), len);
        cmd_args[len] = '\0';
    } else {
        cmd_args[0] = '\0';
    }

    for (int i = 0; i < cmd_limit; i++) {
        if (!strcmp(word, cmd_table[i].name)) {
            cmd_table[i].func();
            return 0;
        }
    }
    return 1;
}
