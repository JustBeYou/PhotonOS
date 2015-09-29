#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

void cmd_help()
{
    printk("""Help:\n \
-> hw - print hardware info\n \
-> color - change color of the screen\n \
-> time - display the time\n \
-> free - display info about memory\n \
-> clear - clear screen\n \
""");
}

void cmd_hw()
{

}

void cmd_color()
{

}

void cmd_time()
{
    print_time();
}

void cmd_free()
{
    
}

void cmd_clear()
{
    clear_vga();
}

int cmd_limit = 6;

shell_cmd_t cmd_table[] = {
    {"help",  cmd_help},
    {"hw",    cmd_hw},
    {"color", cmd_color},
    {"time",  cmd_time},
    {"free",  cmd_free},
    {"clear", cmd_clear}
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
