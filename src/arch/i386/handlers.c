#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

#include <i386/idt.h>
#include <i386/handlers.h>
#include <kernel/io.h>
#include <kernel/syscalls.h>

int tick;
isr_t interrupt_handlers[256];

void std_handler(registers_t *regs)
{
    print_regs(regs);
    panic("Unsupported error occured.\n", __LINE__, __FILE__);
    return;
}

void init_irq()
{
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);

    idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E);
    idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E);
    idt_set_gate(34, (uint32_t)irq2, 0x08, 0x8E);
    idt_set_gate(35, (uint32_t)irq3, 0x08, 0x8E);
    idt_set_gate(36, (uint32_t)irq4, 0x08, 0x8E);
    idt_set_gate(37, (uint32_t)irq5, 0x08, 0x8E);
    idt_set_gate(38, (uint32_t)irq6, 0x08, 0x8E);
    idt_set_gate(39, (uint32_t)irq7, 0x08, 0x8E);
    idt_set_gate(40, (uint32_t)irq8, 0x08, 0x8E);
    idt_set_gate(41, (uint32_t)irq9, 0x08, 0x8E);
    idt_set_gate(42, (uint32_t)irq10, 0x08, 0x8E);
    idt_set_gate(43, (uint32_t)irq11, 0x08, 0x8E);
    idt_set_gate(44, (uint32_t)irq12, 0x08, 0x8E);
    idt_set_gate(45, (uint32_t)irq13, 0x08, 0x8E);
    idt_set_gate(46, (uint32_t)irq14, 0x08, 0x8E);
    idt_set_gate(47, (uint32_t)irq15, 0x08, 0x8E);
}

void irq_handler(registers_t *regs)
{
    // If interrupt involve the slave
    if (regs->int_no >= 40) {
        // Send reset signal.
        outb(0xA0, 0x20);
    }

    // Reset signal to master.
    outb(0x20, 0x20);

    if (interrupt_handlers[regs->int_no] != 0) {
        isr_t handler = interrupt_handlers[regs->int_no];
        handler(regs);
    }
}

static void zero_division_handler(__attribute__((unused)) registers_t *regs)
{
    print_regs(regs);
    panic("Division by 0!\n", __LINE__, __FILE__);
}

static void debugger_handler(__attribute__((unused)) registers_t *regs)
{
    print_regs(regs);
    panic("Debugger interrupt!\n", __LINE__, __FILE__);
}

static void nmi_handler(__attribute__((unused)) registers_t *regs)
{
    print_regs(regs);
    panic("Non maskable interrupt error.\n", __LINE__, __FILE__);
}

static void breakpoint_handler(__attribute__((unused)) registers_t *regs)
{
    print_regs(regs);
    panic("Breakpoint found!\n", __LINE__, __FILE__);
}

static void overflow_handler(__attribute__((unused)) registers_t *regs)
{
    print_regs(regs);
    panic("Overflow!\n", __LINE__, __FILE__);
}

static void bounds_handler(__attribute__((unused)) registers_t *regs)
{
    print_regs(regs);
    panic("Bounds interrupt!\n", __LINE__, __FILE__);
}

static void invalid_opcode_handler(__attribute__((unused)) registers_t *regs)
{
    print_regs(regs);
    panic("Opcode is invalid!\n", __LINE__, __FILE__);
}

static void coprocessor_notavail_handler(__attribute__((unused)) registers_t *regs)
{
    print_regs(regs);
    panic("Coprocessor is not found!\n", __LINE__, __FILE__);
}

static void double_fault_handler(__attribute__((unused)) registers_t *regs)
{
    print_regs(regs);
    panic("Double fault!\n", __LINE__, __FILE__);
}

static void coprocessor_overrun_handler(__attribute__((unused)) registers_t *regs)
{
    print_regs(regs);
    panic("Coprocessor segment overrun!\n", __LINE__, __FILE__);
}

static void invalid_tss_handler(__attribute__((unused)) registers_t *regs)
{
    print_regs(regs);
    panic("TSS is invalid!\n", __LINE__, __FILE__);
}

static void segment_not_present_handler(__attribute__((unused)) registers_t *regs)
{
    print_regs(regs);
    panic("Segment is not present!\n", __LINE__, __FILE__);
}

static void stack_fault_handler(__attribute__((unused)) registers_t *regs)
{
    print_regs(regs);
    panic("Stack fault!\n", __LINE__, __FILE__);
}

static void gpf_handler(__attribute__((unused)) registers_t *regs)
{
    print_regs(regs);
    panic("General protection fault!\n", __LINE__, __FILE__);
}

/* Page fault handler is implemented in paging code. */

static void reserved_handler(__attribute__((unused)) registers_t *regs)
{
    print_regs(regs);
    panic("Reserved?\n", __LINE__, __FILE__);
}

static void math_fault_handler(__attribute__((unused)) registers_t *regs)
{
    print_regs(regs);
    panic("Math fault!\n", __LINE__, __FILE__);
}

static void align_check_handler(__attribute__((unused)) registers_t *regs)
{
    print_regs(regs);
    panic("Alignment check!\n", __LINE__, __FILE__);
}

static void machine_check_handler(__attribute__((unused)) registers_t *regs)
{
    print_regs(regs);
    panic("Machine check!\n", __LINE__, __FILE__);
}

static void simd_handler(__attribute__((unused)) registers_t *regs)
{
    print_regs(regs);
    panic("SIMD error!\n", __LINE__, __FILE__);
}

void init_isr()
{
    idt_set_gate( 0, (uint32_t)isr0 , 0x08, 0x8E);
    idt_set_gate( 1, (uint32_t)isr1 , 0x08, 0x8E);
    idt_set_gate( 2, (uint32_t)isr2 , 0x08, 0x8E);
    idt_set_gate( 3, (uint32_t)isr3 , 0x08, 0x8E);
    idt_set_gate( 4, (uint32_t)isr4 , 0x08, 0x8E);
    idt_set_gate( 5, (uint32_t)isr5 , 0x08, 0x8E);
    idt_set_gate( 6, (uint32_t)isr6 , 0x08, 0x8E);
    idt_set_gate( 7, (uint32_t)isr7 , 0x08, 0x8E);
    idt_set_gate( 8, (uint32_t)isr8 , 0x08, 0x8E);
    idt_set_gate( 9, (uint32_t)isr9 , 0x08, 0x8E);
    idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E);
    idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
    idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E);
    idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
    idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);
    idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
    idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E);
    idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
    idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E);
    idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);
    idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E);
    idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E);
    idt_set_gate(22, (uint32_t)isr22, 0x08, 0x8E);
    idt_set_gate(23, (uint32_t)isr23, 0x08, 0x8E);
    idt_set_gate(24, (uint32_t)isr24, 0x08, 0x8E);
    idt_set_gate(25, (uint32_t)isr25, 0x08, 0x8E);
    idt_set_gate(26, (uint32_t)isr26, 0x08, 0x8E);
    idt_set_gate(27, (uint32_t)isr27, 0x08, 0x8E);
    idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8E);
    idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8E);
    idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E);
    idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E);
    idt_set_gate(128, (uint32_t)isr128, 0x08, 0x8E);

    for (int i = 0; i < 256; i++) {
        register_interrupt_handler(i, &std_handler);
    }

    register_interrupt_handler(0, &zero_division_handler);
    register_interrupt_handler(1, &debugger_handler);
    register_interrupt_handler(2, &nmi_handler);
    register_interrupt_handler(3, &breakpoint_handler);
    register_interrupt_handler(4, &overflow_handler);
    register_interrupt_handler(5, &bounds_handler);
    register_interrupt_handler(6, &invalid_opcode_handler);
    register_interrupt_handler(7, &coprocessor_notavail_handler);
    register_interrupt_handler(8, &double_fault_handler);
    register_interrupt_handler(9, &coprocessor_overrun_handler);
    register_interrupt_handler(10, &invalid_tss_handler);
    register_interrupt_handler(11, &segment_not_present_handler);
    register_interrupt_handler(12, &stack_fault_handler);
    register_interrupt_handler(13, &gpf_handler);
    register_interrupt_handler(15, &reserved_handler);
    register_interrupt_handler(16, &math_fault_handler);
    register_interrupt_handler(17, &align_check_handler);
    register_interrupt_handler(18, &machine_check_handler);
    register_interrupt_handler(19, &simd_handler);
    register_interrupt_handler(128, &syscall_handler);
}

// This gets called from our ASM interrupt handler stub.
void isr_handler(registers_t *regs)
{
    interrupt_handlers[regs->int_no](regs);
}

void register_interrupt_handler(uint8_t n, isr_t handler)
{
    interrupt_handlers[n] = handler;
}
