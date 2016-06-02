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
    size_t cr3; // +0 bytes
    size_t cr2; // +4 bytes
    size_t gs; // +8 bytes
    size_t fs; // +12 bytes
    size_t es; // +16 bytes
    size_t ds; // +20 bytes
    size_t edi; // +24 bytes
    size_t esi; // +28 bytes
    size_t ebp; // +32 bytes
    size_t kesp; // +36 bytes
    size_t ebx; // +40 bytes
    size_t edx; // +44 bytes
    size_t ecx; // +48 bytes
    size_t eax; // +52 bytes
    size_t int_no; // +56 bytes
    size_t err_code; // +60 bytes
    size_t eip; // +64 bytes
    size_t eflags;// +68 bytes
    size_t cs; // +72 bytes
    size_t uesp; // +76 bytes
    size_t ss; // +80 bytes
} registers_t; // registers for interrupts

typedef void (*isr_t)(registers_t*);
void register_interrupt_handler(uint8_t n, isr_t handler);

typedef void* type_t;
typedef unsigned short mode_t;
typedef long long loff_t;

/* Kernel defines */
#define OS_Name "PhotonOS"
#define Version "v0.0.3"
#define Relase_Date "1 June 2016"
#define Author "Feraru Mihail"

#define sti() asm volatile("sti")
#define cli() asm volatile("cli")
#define hlt() asm volatile("hlt")
#define keep_running() while(true) { hlt(); }

/* Kernel ASM variables */
extern size_t stack_top;
extern size_t stack_bottom;
extern size_t kernel_end;
extern size_t kernel_start;

/* Kernel ASM functions */
extern void enable_A20();
extern int detect_cpu(void);
extern void write_cr3();
extern void write_cr0();
extern size_t read_cr3();
extern size_t read_cr2();
extern size_t read_cr0();
extern size_t read_eflags();
extern size_t read_ss();
extern size_t read_cs();
extern size_t read_ebp();
extern void jmp_to_usermode();
extern void switch_context();
extern void save_context();
extern void switch_mm();
extern void switch_to();
extern void syscall_handler(registers_t *regs);

/* Kernel system functions */
void panic(const char *msg, int line, char *file);
void reboot();
void system_wait(int ticks);

// Break point functions
void break_point_1();
void break_point_2();
void break_point_3();
void break_point_4();
void break_point_5();

#endif
