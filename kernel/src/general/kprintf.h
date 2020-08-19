#ifndef __H_KPRINTF
#define __H_KPRINTF 1
/**
 * @file
 * @section DESCRIPTION
 * Provides the kernel with simple output
 */

#include <general/stdarg.h>

int printf(char * fmt, ...);
int sprintf(char * buf, char * fmt, ...);

void hexDump (const char * desc, const void * addr, const int len);

#endif