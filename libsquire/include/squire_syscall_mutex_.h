#ifndef _H_SQUIRE_SYSCALL_MUTEX_
#define _H_SQUIRE_SYSCALL_MUTEX_ 1

#define SQUIRE_SYSCALL_MUTEX_INIT		0x00000030
#define SQUIRE_SYSCALL_MUTEX_LOCK		0x00000031
#define SQUIRE_SYSCALL_MUTEX_UNLOCK		0x00000032
#define SQUIRE_SYSCALL_MUTEX_DEINIT		0x00000033

typedef void * squire_mutex_t;

typedef struct{
	squire_mutex_t mutex;
} squire_params_mutex_t;

#if defined(__cplusplus)
extern "C" {
#endif

extern squire_mutex_t squire_syscall_mutex_init();
extern void squire_syscall_mutex_deinit(squire_mutex_t mutex);
extern void squire_syscall_mutex_lock(squire_mutex_t mutex);
extern void squire_syscall_mutex_unlock(squire_mutex_t mutex);

#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif
