#ifndef _H_SQUIRE_SYSCALL_PROCTHREAD_
#define _H_SQUIRE_SYSCALL_PROCTHREAD_ 1

#define SQUIRE_SYSCALL_THREAD		0x00000010
#define SQUIRE_SYSCALL_PROCESS		0x00000011
#define SQUIRE_SYSCALL_JOIN         0x00000012
#define SQUIRE_SYSCALL_EXIT			0x00000013

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



#if defined(__cplusplus)
extern "C" {
#endif

extern void * squire_syscall_mmap(void * address, size_t length, unsigned int flags);

#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif
