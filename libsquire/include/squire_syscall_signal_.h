#ifndef _H_SQUIRE_SYSCALL_SIGNAL_
#define _H_SQUIRE_SYSCALL_SIGNAL_ 1

#define SQUIRE_SYSCALL_SIGNAL		0x00000040
#define SQUIRE_SYSCALL_SIGNAL_GET   0x00000041
#define SQUIRE_SYSCALL_RAISE        0x00000042

typedef struct squire_params_signal_s{
    void (*handler)(int);
    unsigned int tid;
    void * stack;
    unsigned int stack_length;
} squire_params_signal_t;

typedef struct squire_params_signal_get_s{
    unsigned int value;
} squire_params_signal_get_t;

#if defined(__cplusplus)
extern "C" {
#endif

extern void squire_syscall_signal(int (*handler)(int));
extern unsigned int squire_syscall_signal_get();

#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif

