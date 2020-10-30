#ifndef _H_SQUIRE_SYSCALL_SIMPLE_
#define _H_SQUIRE_SYSCALL_SIMPLE_ 1

#define SQUIRE_SYSCALL_SIMPLE_SEND	0x00000020
#define SQUIRE_SYSCALL_SIMPLE_RECV	0x00000021

struct squire_params_simple_send_s{
	unsigned int 	to;
	void *			data;
	size_t			length;
	unsigned int	status;
};
typedef struct squire_params_simple_send_s squire_params_simple_send_t;

struct squire_params_simple_recv_s{
	unsigned int	from;
	void *			buffer;
	size_t			length;
	unsigned int	status;
	unsigned char	blocked;
};
typedef struct squire_params_simple_recv_s squire_params_simple_recv_t;

#if defined(__cplusplus)
extern "C" {
#endif

extern unsigned int squire_syscall_simple_send(unsigned int to, size_t length, void * data);
extern unsigned int squire_syscall_simple_recv(void * buffer, size_t * length, unsigned int * from);
extern unsigned int squire_syscall_simple_recv_blocked(void * buffer, size_t * length, unsigned int * from);

#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif
