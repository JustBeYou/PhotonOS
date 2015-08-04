#include <io.h>
#include <stdint.h>
#include <stddef.h>
#include <idt.h>
#include <isr.h>
#include <irq.h>
#include <string.h>
#include <system.h>

static void unhandled_interrupt(__attribute__((unused)) registers_t *regs)
{
	print_regs(regs);
	panic("Interrupt unhandled!", __LINE__, __FILE__);
}

void init_idt()
{
    idt_ptr.limit = sizeof(idt_entry_t) * 256 -1;
    idt_ptr.base  = (uint32_t)&idt_entries;

    memset(&idt_entries, 0, sizeof(idt_entry_t)*256);

	for (int n = 0; n < 256; n++) {
    	register_interrupt_handler(n, &unhandled_interrupt);
	}

	init_isr();
	init_irq();

    idt_flush((uint32_t)&idt_ptr);
}

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
   idt_entries[num].base_lo = base & 0xFFFF;
   idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

   idt_entries[num].sel     = sel;
   idt_entries[num].always0 = 0;
   // We must uncomment the OR below when we get to using user-mode.
   // It sets the interrupt gate's privilege level to 3.
   idt_entries[num].flags   = flags /* | 0x60 */;
} 
