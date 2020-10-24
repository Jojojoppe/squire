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

/**
 * @brief Execute in user thread
 * 
 * Sets up a user stack and kernel stack and jumps to ring 3. Paramters are stored
 * in archtitecture specific way. This function is used as return address for the
 * start of a user thread
 */
void proc_user_exec();

#endif