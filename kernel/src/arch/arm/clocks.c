#include "../../common/arch/clocks.h"
#include "../../common/kprintf.h"
#include "gic.h"
#include "cpu.h"

#define Private_Timer_Load				POINTER_TO_REGISTER(PERIPH_VIRT_BASE+0x0600)
#define Private_Timer_Counter			POINTER_TO_REGISTER(PERIPH_VIRT_BASE+0x0604)
#define Private_Timer_Control			POINTER_TO_REGISTER(PERIPH_VIRT_BASE+0x0608)
#define Private_Timer_Interrupt_Status	POINTER_TO_REGISTER(PERIPH_VIRT_BASE+0x060c)

#define PT_ENABLE 1
#define PT_AUTO_RELOAD 2
#define PT_IRQ_ENABLE 4

void arch_sysclock_init(){
	gic_enable_interrupt(29);
	gic_set_priority(29, 0);
	gic_set_target(29, GIC_TARGET_CPU0);

	Private_Timer_Load = 0x000cffff;
	Private_Timer_Control = PT_IRQ_ENABLE | PT_AUTO_RELOAD;
}

void arch_sysclock_enable(){
	Private_Timer_Control |= PT_ENABLE;
}

void arch_sysclock_disable(){
	Private_Timer_Control &= ~PT_ENABLE;
}

void arch_sysclock_isr(){
	Private_Timer_Interrupt_Status |= 1;
    sysclock_cb();
}
