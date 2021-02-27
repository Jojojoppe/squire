#ifndef __H_X86_GENERIC_CMOS
#define __H_X86_GENERIC_CMOS 1

#include <squire.h>

#define RTC_INTR 0x28
#define RTC_IO_PORT_INDEX 0x70
#define RTC_IO_PORT_DATA 0x71

void x86_generic_CMOS_INTR();

void x86_generic_CMOS_init();

#endif