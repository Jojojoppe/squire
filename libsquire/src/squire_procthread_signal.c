#include <squire_syscall.h>

extern void * malloc(size_t);
extern void free(void *);

unsigned int squire_extraval0;
unsigned int squire_extraval1;
unsigned int squire_extraval2;
unsigned int squire_extraval3;

struct __squire_procthread_signal_handler_params_s__{
    void (*handler)(int);
    void * stack;
    size_t stack_length;
    unsigned int tid;
};

int __squire_procthread_signal_handler__(void * p){
    struct __squire_procthread_signal_handler_params_s__ * s = (struct __squire_procthread_signal_handler_params_s__*)p;
    while(1){
        squire_syscall_procthread_t params;
        params.operation = SQUIRE_SYSCALL_PROCTHREAD_OPERATION_GETSIG;
        squire_syscall(SQUIRE_SYSCALL_PROCTHREAD, sizeof(params), &params);
        // Set extra values
        squire_extraval0 = params.value1;
        squire_extraval1 = params.value2;
        squire_extraval2 = params.value3;
        squire_extraval3 = params.value4;
        if(s->handler)
            s->handler(params.value0);
    }
    free(p);
    squire_procthread_exit_thread(0);
}

void squire_procthread_signal(void (*handler)(int)){
    struct __squire_procthread_signal_handler_params_s__ * p = (struct __squire_procthread_signal_handler_params_s__*)malloc(sizeof(struct __squire_procthread_signal_handler_params_s__));
    p->handler = handler;
    p->stack = malloc(1024);
    p->stack_length = 1024;
    p->tid = squire_procthread_create_thread(__squire_procthread_signal_handler__, p->stack, p->stack_length, THREAD_QUEUE_PRIORITY, p);
    squire_syscall_procthread_t params;
    params.operation = SQUIRE_SYSCALL_PROCTHREAD_OPERATION_SIGNAL;
    params.tid0 = p->tid;
    squire_syscall(SQUIRE_SYSCALL_PROCTHREAD, sizeof(params), &params);
}