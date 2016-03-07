#ifndef _shell_h
#define _shell_h

#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

typedef struct shell_cmd {
    char *name;
    void (*func)(void);
} shell_cmd_t;

int shell(char *cmd);

#endif
