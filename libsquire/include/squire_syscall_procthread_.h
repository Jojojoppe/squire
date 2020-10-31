#ifndef _H_SQUIRE_SYSCALL_PROCTHREAD_
#define _H_SQUIRE_SYSCALL_PROCTHREAD_ 1

#define SQUIRE_SYSCALL_THREAD		0x00000010
#define SQUIRE_SYSCALL_PROCESS		0x00000011
#define SQUIRE_SYSCALL_JOIN         0x00000012
#define SQUIRE_SYSCALL_EXIT			0x00000013
#define SQUIRE_SYSCALL_YIELD		0x00000014
#define SQUIRE_SYSCALL_WAIT			0x00000015
#define SQUIRE_SYSCALL_KILL			0x00000016
#define SQUIRE_SYSCALL_GETID		0x00000017
#define SQUIRE_SYSCALL_THREAD_EXIT	0x00000018

struct squire_params_thread_s{
	void 			(*entry)(void);
	void *			stack_base;
	size_t			stack_length;
	unsigned int	flags;
};
typedef struct squire_params_thread_s squire_params_thread_t;

struct squire_params_process_s{
	void *			elf_start;
	size_t			elf_length;
	int 			argc;
	void *			param_data;
	size_t			param_data_size;
	unsigned int	pid;
};
typedef struct squire_params_process_s squire_params_process_t;

struct squire_params_join_s{
	unsigned int	id;
	int				retval;
};
typedef struct squire_params_join_s squire_params_join_t;

struct squire_params_exit_s{
	int				retval;
};
typedef struct squire_params_exit_s squire_params_exit_t;

struct squire_params_wait_s{
	unsigned int	pid;
	unsigned int	reason;
	unsigned int 	retval;
};
typedef struct squire_params_wait_s squire_params_wait_t;

struct squire_params_kill_s{
	unsigned int	pid;
	unsigned int	reason;
};
typedef struct squire_params_kill_s squire_params_kill_t;

struct squire_params_getid_s{
	unsigned int	pid;
	unsigned int	tid;
};
typedef struct squire_params_getid_s squire_params_getid_t;

#if defined(__cplusplus)
extern "C" {
#endif

extern unsigned int squire_syscall_thread(void (*entry)(void), void * stack_base, size_t stack_length, unsigned int flags, void * param);
extern unsigned int squire_syscall_process(void * elf_start, size_t elf_length, int argc, char ** argv);
extern int squire_syscall_join(unsigned int id);
extern void squire_syscall_exit(int retval);
extern void squire_syscall_thread_exit(int retval);
extern void squire_syscall_yield();
extern unsigned int squire_syscall_wait(unsigned int * retval, unsigned int pid);
extern void squire_syscall_kill(unsigned int pid, unsigned int reason);
extern void squire_syscall_getid(unsigned int * pid, unsigned int * tid);

#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif
