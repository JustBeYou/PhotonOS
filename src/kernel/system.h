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
    uint32_t cr3; // +0 bytes
    uint32_t cr2; // +4 bytes
    uint32_t gs; // +8 bytes
    uint32_t fs; // +12 bytes
    uint32_t es; // +16 bytes
    uint32_t ds; // +20 bytes
    uint32_t edi; // +24 bytes
    uint32_t esi; // +28 bytes
    uint32_t ebp; // +32 bytes
    uint32_t kesp; // +36 bytes
    uint32_t ebx; // +40 bytes
    uint32_t edx; // +44 bytes
    uint32_t ecx; // +48 bytes
    uint32_t eax; // +52 bytes
    uint32_t int_no; // +56 bytes
    uint32_t err_code; // +60 bytes
    uint32_t eip; // +64 bytes
    uint32_t eflags;// +68 bytes
    uint32_t cs; // +72 bytes
    uint32_t uesp; // +76 bytes
    uint32_t ss; // +80 bytes
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
