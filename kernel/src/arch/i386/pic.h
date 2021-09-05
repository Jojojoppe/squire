#ifndef __H_PIT
#define __H_PIT 1

#include "idt.h"

void pic_init();

void pic_ack();

__attribute__((interrupt)) void pic_empty_isr(isr_frame * frame);

#endif
