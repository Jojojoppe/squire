#ifndef __PROC_H
#define __PROC_H 1
/**
 * @file
 * @section DESCRIPTION
 * The process manager
 */

// The ARCH interface
#include <general/arch/proc.h>
#include <general/config.h>

typedef struct {
    unsigned int cr3;
} proc_proc_arch_data_t;

typedef struct {
    void * kstack;
    unsigned int tss_esp0;
    unsigned char fpudata[128];
} proc_thread_arch_data_t;


#endif