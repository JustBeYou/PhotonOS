#ifndef _system_h
#define _system_h

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <multiboot.h>

#define OS_Name "PhotonOS"
#define Version "v0.0.1cpp"
#define Relase_Date "16 July 2015"
#define Author "Feraru Mihail"

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
} registers_t; // registers for interrupts

typedef struct g_regs
{
    uint32_t edi;
    uint32_t esi;
    uint32_t edx;
    uint32_t ecx;
    uint32_t ebx;
    uint32_t eax;
} g_regs_t; // general registers for system calls

typedef struct t_regs {
    uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, cr3;
} t_regs_t; // task registers

typedef void (*isr_t)(registers_t*);
void register_interrupt_handler(uint8_t n, isr_t handler);

typedef void* type_t;
extern size_t kernel_end;
extern size_t kernel_start;
extern uint32_t stack_top;
extern uint32_t stack_bottom;
extern uint32_t init_esp;

extern char user[20];
extern char machine[30];

void panic(const char *msg, int line, char *file);
void reboot();
void shell(char *str);
void prompt();

#endif
