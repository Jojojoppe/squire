#ifndef _H_SQUIRE_SYSCALL_LOG_
#define _H_SQUIRE_SYSCALL_LOG_ 1

#define SQUIRE_SYSCALL_LOG			0x10000000

struct squire_params_log_s{
	char *			data;
	size_t			length;
};
typedef struct squire_params_log_s squire_params_log_t;

#if defined(__cplusplus)
extern "C" {
#endif

extern void squire_syscall_log(char * data, size_t length);

#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif
