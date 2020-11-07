#ifndef __H_X86_GENERIC_RTC
#define __H_X86_GENERIC_RTC 1

#include <squire.h>
#include <squire_driver.h>

#define RTC_INTR 0x28
#define RTC_IO_PORT_INDEX 0x70
#define RTC_IO_PORT_DATA 0x71

void x86_generic_RCT_INTR();

void x86_generic_RTC_function_callback(unsigned int from, squire_driver_submessage_function_t * func);

#endif
