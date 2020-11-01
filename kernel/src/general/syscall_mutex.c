#include <general/syscall_mutex.h>
#include <general/syscall.h>

#include <general/kprintf.h>
#include <general/mutex.h>

int syscall_mutex(squire_syscall_mutex_t * params){
    switch(params->operation){

        case SQUIRE_SYSCALL_MUTEX_OPERATION_CREATE:{
            params->mtx0 = (void*) mutex_create();
        } break;

        case SQUIRE_SYSCALL_MUTEX_OPERATION_DESTROY:{
            mutex_destroy((mutex_t*)params->mtx0);
        } break;

        case SQUIRE_SYSCALL_MUTEX_OPERATION_LOCK:{
            if(params->mtx0){
                mutex_lock((mutex_t*)params->mtx0);
            }
        } break;

        case SQUIRE_SYSCALL_MUTEX_OPERATION_UNLOCK:{
            if(params->mtx0){
                mutex_unlock((mutex_t*)params->mtx0);
            }
        } break;

        case SQUIRE_SYSCALL_MUTEX_OPERATION_STATUS:{
            if(params->mtx0){
                params->value0 = mutex_get((mutex_t*)params->mtx0);
            }else{
                params->value0 = -1;
            }
        } break;

        default:
            return SYSCALL_ERROR_OPERATION;
    }
    return SYSCALL_ERROR_NOERROR;
}