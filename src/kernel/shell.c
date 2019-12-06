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
    int fd = kopen("/mnt/initrd/help.txt", O_RDONLY);
    char c;

    int newlines = 0;
    while (kread(fd, &c, 1) != 0) {
        printk("%c", c);
        if (c == '\n') {
            newlines++;
        }

        if (newlines >= 10) {
            getchark();
            newlines = 0;
        }
    }
    kclose(fd);
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
    if (cmd_args[0] == '\0') {
        printk("You must specify a file name.\n");
        return ;
    }

    DIR *d = kopendir(cmd_args);
    if (d != NULL) {
        printk("Directory already exists.\n");
        kclosedir(d);
        return ;
    }

    int fd = kopen(cmd_args, O_RDONLY);
    if (fd != -1) {
        printk("There is a file with same name.\n");
        kclose(fd);
        return ;
    }

    int ret = kmkdir(cmd_args);
    if (ret == -1) {
        printk("An error occured.\n");
        return ;
    }
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
    if (cmd_args[0] == '\0') {
        printk("You must specify a file name and a text message.\n");
        return ;
    }

    int space_pos = 0;
    for (int i = 0; cmd_args[i] != '\0'; i++) {
        if (cmd_args[i] == ' ') {
            space_pos = i;
            break;
        }
    }

    if (space_pos == 0) {
        printk("You must enter a text message to write.\n");
        return ;
    }

    char filename[256];
    cmd_args[space_pos] = '\0';

    memset(filename, 0, 256);
    strcpy(filename, cmd_args);

    char message[2048];
    memset(message, 0 , 2048);
    strcpy(message, &cmd_args[space_pos + 1]);
    size_t message_len = strlen(message);
    if (message_len == 0) {
        printk("You must enter a text message to write.\n");
        return ;
    }

    int fd = kopen(filename, O_WRONLY);
    kwrite(fd, message, message_len);
    kclose(fd);
}

void cmd_read()
{
    if (cmd_args[0] == '\0') {
        printk("You must specify a file name.\n");
        return ;
    }

    int fd = kopen(cmd_args, O_RDONLY);
    if (fd == -1) {
        printk("File doesn't exist.\n");
        return ;
    }
    char message[4096];
    memset(message, 0, 4096);

    while (kread(fd, message, 4096) != 0) {
        printk("%s", message);
        memset(message, 0, 4096);
    }
    printk("\n");
    kclose(fd);
}

void cmd_append()
{
    return ;

    if (cmd_args[0] == '\0') {
        printk("You must specify a file name and a text message.\n");
        return ;
    }

    int space_pos = 0;
    for (int i = 0; cmd_args[i] != '\0'; i++) {
        if (cmd_args[i] == ' ') {
            space_pos = i;
            break;
        }
    }

    char filename[256];
    cmd_args[space_pos] = '\0';

    memset(filename, 0, 256);
    strcpy(filename, cmd_args);

    char message[2048];
    memset(message, 0 , 2048);
    strcpy(message, &cmd_args[space_pos + 1]);
    size_t message_len = strlen(message);
    if (message_len == 0) {
        printk("You must enter a text message to write.\n");
        return ;
    }

    int fd = kopen(filename, O_APPEND);
    kwrite(fd, message, message_len);
    kclose(fd);
}

void cmd_echo()
{
    printk("%s\n", cmd_args);
}

extern char custom_prompt;
extern char prompt_msg[256];

void cmd_prompt()
{
    if (cmd_args[0] == '\0')
        custom_prompt = 0;
    else {
        memset(prompt_msg, 0, 256);
        memcpy(prompt_msg, cmd_args, 255);
        custom_prompt = 1;
    }
}

void cmd_color()
{
    int fd = kopen("/mnt/initrd/colors.txt", O_RDONLY);
    printk("%d\n", fd);
    char buf[512];
    memset(buf, 0, 512);
    while (kread(fd, buf, 512) != 0) {
        printk("%s", buf);
        memset(buf, 0, 512);
        kread(fd, buf, 512);
    }
    kclose(fd);

    printk("\n");
    char color1[3];
    int color_code1;

    char color2[3];
    int color_code2;

    printk("Enter foreground color: ");
    getsk(color1);
    color_code1 = atoi(color1);
    if (color_code1 < 0 && color_code1 > 15) {
        printk("Not valid!\n");
        return ;
    }

    printk("Enter background color: ");
    getsk(color2);
    color_code2 = atoi(color2);
    if (color_code2 < 0 && color_code2 > 15) {
        printk("Not valid!\n");
        return ;
    }

    vga_setcolor(color_code1, color_code2);
    clear_vga();
}

void cmd_test_drivers()
{
    printk("[INFO] KEYBOARD TEST\n");
    printk("[INFO] Type following characters: 123abcZYX@^*()[]:|\n");
    char msg[512];
    memset(msg, 0, 512);

    getsk(msg);
    if (!strcmp(msg, "123abcZYX@^*()[]:|")) {
        printk("[DONE] Test passed\n");
    } else {
        printk("[ERROR] Strings aren't the same.\n");
    }
}

void cmd_test_fs()
{
    printk("[INFO] TEST FILESYSTEM.\n");
    printk("[INFO] Get '/mnt/initrd'\n");
    struct dentry *de = get_dentry_by_path("/mnt/initrd");
    if (de != NULL) {
        printk("[DONE] Test passed.\n");
    } else {
        printk("[ERROR] \n"); return ;
    } getchark();

    printk("[INFO] Read '/mnt/initrd/testfile.txt'\n");
    int fd = kopen("/mnt/initrd/testfile.txt", O_RDONLY);
    char msg[512];
    memset(msg, 0, 512);
    kread(fd, msg, 13);
    kclose(fd);
    if (!strcmp(msg, "1112223334445")) {
        printk("[DONE] Test passed.\n");
    } else {
        printk("[ERROR] \n"); return ;
    } getchark();

    printk("[INFO] Get '/null/null'\n");
    de = get_dentry_by_path("/null/null");
    if (de == NULL) {
        printk("[DONE] Test passed.\n");
    } else {
        printk("[ERROR] \n"); return ;
    } getchark();

    printk("[INFO] Open '/null/null'\n");
    fd = kopen("/null/null", O_RDONLY);
    if (fd == -1) {
        printk("[DONE] Test passed.\n");
    } else {
        printk("[ERROR] \n"); return ;
    } getchark();

    printk("[INFO] Open and create 'proc/file.txt'\n");
    fd = kopen("proc/file.txt", O_WRONLY);
    kclose(fd);
    if (fd != -1) {
        printk("[DONE] Test passed.\n");
    } else {
        printk("[ERROR] \n"); return ;
    } getchark();

    printk("[INFO] Write file 'proc/file.txt'\n");
    fd = kopen("proc/file.txt", O_WRONLY);
    int ret = kwrite(fd, msg, 13);
    kclose(fd);
    if (ret == 13) {
        printk("[DONE] Test passed.\n");
    } else {
        printk("[ERROR] \n"); return ;
    } getchark();

    printk("[INFO] Read file 'proc/file.txt'\n");
    fd = kopen("proc/file.txt", O_RDONLY);
    kread(fd, msg, 13);
    kclose(fd);
    if (!strcmp(msg, "1112223334445")) {
        printk("[DONE] Test passed.\n");
    } else {
        printk("[ERROR] \n"); return ;
    } getchark();

    printk("[INFO] Make directory 'proc/dir'\n");
    kmkdir("proc/dir");
    if (get_dentry_by_path("proc/dir") != NULL) {
        printk("[DONE] Test passed.\n");
    } else {
        printk("[ERROR] \n"); return ;
    } getchark();

    printk("[INFO] Make directory 'proc/dir/dir'\n");
    kmkdir("proc/dir/dir");
    if (get_dentry_by_path("proc/dir/dir") != NULL) {
        printk("[DONE] Test passed.\n");
    } else {
        printk("[ERROR] \n"); return ;
    } getchark();

    printk("[INFO] Make file 'proc/dir/dir/file.txt'\n");
    fd = kopen("proc/dir/dir/file.txt", O_RDWR);
    if (fd != -1) {
        printk("[DONE] Test passed.\n");
    } else {
        printk("[ERROR] \n"); return ;
    } getchark();

    printk("[INFO] Write file 'proc/dir/dir/file.txt'\n");
    ret = kwrite(fd, "aaa", 3);
    kclose(fd);
    if (ret == 3) {
        printk("[DONE] Test passed.\n");
    } else {
        printk("[ERROR] \n"); return ;
    } getchark();

    printk("[INFO] List directory 'proc/dir/dir'\n");
    DIR *d = kopendir("proc/dir/dir");
    de = kreaddir(d);
    kclosedir(d);
    if (de == get_dentry_by_path("proc/dir/dir/file.txt")) {
        printk("[DONE] Test passed.\n");
    } else {
        printk("[ERROR] \n"); return ;
    } getchark();

    printk("[DONE] All tests passed.\n");

}

void cmd_test_kernel()
{
    printk("[INFO] TEST KERNEL\n");
    printk("[INFO] Test allocation.\n");
    int *a = kmalloc(sizeof(int), 0, 0);
    char *msg = kmalloc(512, 0, 0);
    *a = 5;
    if (*a == 5 && msg != NULL) {
        printk("[DONE] Test passed.\n");
    } else {
        printk("[ERROR] \n"); return ;
    } getchark();

    printk("[INFO] Test free.\n");
    kfree(a);
    kfree(msg);
    if (*(a - 0x13) != 1) {
        printk("[DONE] Test passed.\n");
    } else {
        printk("[ERROR] \n"); return ;
    } getchark();

    printk("[INFO] Test shell.\n");
    shell("touch aaa.txt");
    if (get_dentry_by_path("aaa.txt") != NULL) {
        printk("[DONE] Test passed.\n");
    } else {
        printk("[ERROR] \n"); return ;
    } getchark();

    printk("[DONE] All tests passed.\n");
}

void cmd_test_libc()
{
    printk("[INFO] TEST LIBC\n");
    char msg[512];
    printk("[INFO] Zero string.\n");
    memset(msg, 0, 512);
    if (msg[0] == 0) {
        printk("[DONE] Test passed.\n");
    } else {
        printk("[ERROR] \n"); return ;
    } getchark();
    printk("[INFO] Copy 'Hello guys'.\n");
    strcpy(msg, "Hello guys");
    if (!strcmp(msg, "Hello guys")) {
        printk("[DONE] Test passed.\n");
    } else {
        printk("[ERROR] \n"); return ;
    } getchark();
    printk("[INFO] Replace first 3 letters with 'abc'.\n");
    memcpy(msg, "abc", 3);
    if (!strcmp(msg, "abclo guys")) {
        printk("[DONE] Test passed.\n");
    } else {
        printk("[ERROR] \n"); return ;
    } getchark();
    printk("[INFO] Reverse string.\n");
    strrev(msg);
    if (!strcmp(msg, "syug olcba")) {
        printk("[DONE] Test passed.\n");
    } else {
        printk("[ERROR] \n"); return ;
    } getchark();
    printk("[INFO] String: %s\n", msg);

    printk("[DONE] All tests passed.\n");
}

void cmd_test_os()
{
    printk("[INFO] Test OS.\n");

    cmd_test_kernel();
    cmd_test_drivers();
    cmd_test_fs();
    cmd_test_libc();
    cmd_test_syscall();

    printk("[INFO] OS was tested.\n");
}

extern int alarm_time;
extern char alarm_msg[4096];

void cmd_alarm()
{
    if (cmd_args[0] == 0) {
        printk("You need to supply time.\n");
    }

    int t = atoi(cmd_args);
    printk("Enter a message: ");
    char msg[4096];
    memset(msg, 0, 4096);
    getsk(msg);

    alarm_time = t;
    strcpy(alarm_msg, msg);
}

void cmd_info()
{
    printk("%s %s (%s) by %s. Copyright C 2016 %s. All rights reserved.\n", OS_Name, Version, Relase_Date, Author, Author);
    getchark();
    shell("time");
    getchark();
    shell("hw");
    getchark();
    shell("free");
    getchark();
    shell("top");
    getchark();
}

void cmd_halt()
{
    panic("Kernel exited.\n", __LINE__, __FILE__);
}

int cmd_limit = 30;

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
    {"mkdir", cmd_mkdir},
    {"touch", cmd_touch},
    {"write", cmd_write},
    {"read", cmd_read},
    {"append", cmd_append},
    {"echo", cmd_echo},
    {"prompt", cmd_prompt},
    {"color", cmd_color},
    {"test_read", cmd_test_read},
    {"test_write", cmd_test_write},
    {"test_syscall", cmd_test_syscall},
    {"test_drivers", cmd_test_drivers},
    {"test_fs", cmd_test_fs},
    {"test_kernel", cmd_test_kernel},
    {"test_libc", cmd_test_libc},
    {"test_os", cmd_test_os},
    {"alarm", cmd_alarm},
    {"info", cmd_info},
    {"halt", cmd_halt}
};

int shell(char *cmd)
{
    if (cmd[0] == ' ')
        return -1;

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
