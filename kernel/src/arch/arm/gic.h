#ifndef __H_GIC
#define __H_GIC 1

#include <stddef.h>

void gic_init();

void gic_enable_interrupt(unsigned char number);

void gic_disable_interrupt(unsigned char number);

void gic_set_priority(unsigned char number, unsigned char priority);

#define GIC_TARGET_CPU0 1
#define GIC_TARGET_CPU1 2

void gic_set_target(unsigned char number, unsigned char target);

unsigned int gic_ack_interrupt();

void gic_end_interrupt(unsigned char number);

#define GIC_SGI_SPECIFIC 0
#define GIC_SGI_ALLOTHER 1
#define GIC_SGI_SELF 2

void gic_sgi(unsigned char target, unsigned char cpu_interface, unsigned char irq);

#endif
