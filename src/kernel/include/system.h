#ifndef _system_h
#define _system_h

#define LOW_TASK_TIME 1
#define NORMAL_TASK_TIME 10
#define HIGH_TASK_TIME 100

/* Standard headers */
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <multiboot.h>

/* Kernel Data Structures */
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

/* Kernel headers */

/* Arch i386 headers */
#include <gdt.h>
#include <idt.h>
#include <handlers.h>
#include <pit.h>
#include <pmm.h>
#include <vmm.h>
#include <pic.h>

/* General headers */
#include <icxxabi.h>
#include <io.h>
#include <keyboard.h>
#include <heap.h>
#include <task.h>
#include <time.h>
#include <vga.h>
#include <ui.h>
#include <shell.h>
#include <vfs.h>
#include <fdc.h>

/* Kernel defines */
#define OS_Name "PhotonOS"
#define Version "v0.0.1nowarn"
#define Relase_Date "14 January 2016"
#define Author "Feraru Mihail"

#define sti() asm volatile("sti")
#define cli() asm volatile("cli")
#define hlt() asm volatile("hlt")
#define keep_running() while(true) { hlt(); }

/* Kernel ASM variables */
extern uint32_t stack_top;
extern uint32_t stack_bottom;
extern uint32_t kernel_end;
extern uint32_t kernel_start;

/* Kernel ASM functions */
extern void enable_A20();
extern int detect_cpu(void);
extern void write_cr3();
extern void write_cr0();
extern uint32_t read_cr3();
extern uint32_t read_cr0();

/* Kernel system functions */
void panic(const char *msg, int line, char *file);
void reboot();
void system_wait(int ticks);

#endif
