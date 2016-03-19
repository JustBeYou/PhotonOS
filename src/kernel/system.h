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
#include <kernel/multiboot.h>

/* Kernel Data Structures */
typedef struct registers
{
    uint32_t cr3;
    uint32_t cr2;
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t kesp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t int_no;
    uint32_t err_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t uesp;
    uint32_t ss;
} registers_t; // registers for interrupts

typedef void (*isr_t)(registers_t*);
void register_interrupt_handler(uint8_t n, isr_t handler);

typedef void* type_t;
typedef unsigned short mode_t;
typedef long long loff_t;

/* Kernel defines */
#define OS_Name "PhotonOS"
#define Version "v0.0.2"
#define Relase_Date "7 March 2016"
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
extern uint32_t read_cr2();
extern uint32_t read_cr0();
extern uint32_t read_eflags();
extern uint32_t read_ss();
extern uint32_t read_cs();
extern uint32_t read_ebp();
extern void jmp_to_usermode();
extern void switch_context();
extern void save_context();
extern void switch_mm();
extern void switch_to();

/* Kernel system functions */
void panic(const char *msg, int line, char *file);
void reboot();
void system_wait(int ticks);

#endif
