#ifndef __H_SYSCALL
#define __H_SYSCALL 1

#include <general/stdint.h>
#include <squire.h>

enum SYSCALL_ERROR{
    SYSCALL_ERROR_NOERROR = 0,
    SYSCALL_ERROR_GENERAL = -1,
    SYSCALL_ERROR_PARAMS = -2,
    SYSCALL_ERROR_OPERATION = -3
};

unsigned syscall(unsigned int opcode, void * param_block, size_t param_len);

#endif
