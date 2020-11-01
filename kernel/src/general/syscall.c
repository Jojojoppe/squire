#include <general/syscall.h>
#include <squire_syscall.h>

#include <general/syscall_debug.h>
#include <general/syscall_memory.h>
#include <general/syscall_message.h>
#include <general/syscall_procthread.h>
#include <general/syscall_mutex.h>

#include <general/arch/proc.h>

unsigned int syscall(unsigned int opcode, void * param_block, size_t param_len){

    unsigned int returncode = SYSCALL_ERROR_GENERAL;
    // printf("[%3d,%3d] SYSCALL %08x\r\n", proc_proc_get_current()->id, proc_thread_get_current()->id, opcode);

    switch(opcode){

        case SQUIRE_SYSCALL_MEMORY:{
            if(param_len<sizeof(squire_syscall_memory_t))
                break;
            returncode = syscall_memory((squire_syscall_memory_t*)param_block);
        }break;

        case SQUIRE_SYSCALL_PROCTHREAD:{
            if(param_len<sizeof(squire_syscall_procthread_t))
                break;
            returncode = syscall_procthread((squire_syscall_procthread_t*)param_block);
        }break;

        case SQUIRE_SYSCALL_MESSAGE:{
            if(param_len<sizeof(squire_syscall_message_t))
                break;
            returncode = syscall_message((squire_syscall_message_t*)param_block);
        }break;

        case SQUIRE_SYSCALL_IO:
            break;

        case SQUIRE_SYSCALL_DEBUG:{
            if(param_len<sizeof(squire_syscall_debug_t))
                break;
            returncode = syscall_debug((squire_syscall_debug_t*)param_block);
        }break;

        case SQUIRE_SYSCALL_MUTEX:{
            if(param_len<sizeof(squire_syscall_mutex_t))
                break;
            returncode = syscall_mutex((squire_syscall_mutex_t*)param_block);        
        } break;

        case SQUIRE_SYSCALL_MISC:
            break;

        default:
            printf("UNKNOWN SYSCALL\r\n");
            break;

    }

    return returncode;
}
