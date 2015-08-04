#ifndef _system_h
#define _system_h

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define OS_Name "NewOS"
#define Version "alpha-2"
#define Relase_Date "23 March 2015"
#define Author "Feraru Mihail"

#define PIC1		0x20		
#define PIC2		0xA0		
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

#define sti() asm volatile("sti")
#define cli() asm volatile("cli")
#define hlt() asm volatile("hlt")
#define keep_running() while(true) { hlt(); }

#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

typedef struct registers
{
    uint32_t ds;                  // Data segment selector
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
    uint32_t int_no, err_code;    // Interrupt number and error code (if applicable)
    uint32_t eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
} registers_t;

typedef void (*isr_t)(registers_t*);
void register_interrupt_handler(uint8_t n, isr_t handler);

typedef void* type_t;
extern size_t kernel_end;
extern size_t kernel_start;

char user[20];
char machine[30];

void panic(char *msg, int line, char *file);
void exec(char *str);
void prompt();

#endif
