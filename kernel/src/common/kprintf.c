#include "kprintf.h"
#include "config.h"
#include "arch/spinlock.h"
#include "arch/debugprint.h"

#include <stddef.h>

static unsigned char kernel_log_debug_out = 0;

static arch_spinlock_t kernel_log_lock;

// Layout of kernel log file
struct kernel_log_block_s{
	// All characters but last field which is a pointer to the next block
	char log[KERNEL_LOG_LENGTH-sizeof(struct kernel_log_block_s*)];
	struct kernel_log_block_s * next;
};

// Start of kernel log file
struct kernel_log_block_s kernel_log_first_block = {0};

// Current kernel log block
struct kernel_log_block_s * kernel_log_block_current = NULL;
// Current character in kernel log block
unsigned int kernel_log_block_current_char = 0;

// Print a character to kernel log
// print to boot debug device if enabled
void kputchar(char c){

	if(kernel_log_block_current_char<KERNEL_LOG_LENGTH-sizeof(struct kernel_log_block_s*)-1){
		kernel_log_block_current->log[kernel_log_block_current_char++] = c;
	}else{
		// Not enough space in current log block: create a new one and link to current one
		// TODO implement creating new kernel log block
	}

	#if KERNEL_LOG_PRINT_ENABLE
	if(kernel_log_debug_out==1){
		arch_debugprint_putc(c);
	}
	#endif
}

static int simple_vsprintf(char **out, char *format, va_list ap);

// The kernel debug print function
int kprintf(char *fmt, ...){
	va_list ap;
	int r;

	arch_spinlock_lock(&kernel_log_lock);

	va_start(ap, fmt);
	r = simple_vsprintf(NULL, fmt, ap);
	va_end(ap);

	arch_spinlock_unlock(&kernel_log_lock);

	return r;
}

// Initialize kernel debug log
void kdebuglog_init(){
	kernel_log_block_current = &kernel_log_first_block;
	kernel_log_block_current_char = 0;

	#if KERNEL_LOG_PRINT_ENABLE
	kernel_log_debug_out = 1;
	arch_debugprint_init();
	#endif

	arch_spinlock_init(&kernel_log_lock);
}

// --------------------------------
// PRINTF STUFF
// --------------------------------

static void simple_outputchar(char **str, char c){
	if (str) {
		**str = c;
		++(*str);
	} else {
		kputchar(c);
	}
}

enum flags {
	PAD_ZERO	= 1,
	PAD_RIGHT	= 2,
};

/*
#define MAX_PRECISION    50
#define IsNaN(n) (n != n)
static void print_double_float(double val, unsigned int precision){
    unsigned int cur_prec = 1;
	if(precision==0)
		precision = MAX_PRECISION;

    // if the user-defined precision is out-of-bounds, normalize it
    if(precision > MAX_PRECISION)
        precision = MAX_PRECISION;

    // if it's negative, show it!
    if(val < 0)
    {
		kprintf("-");
       
        // change to a positive value
        val = -val;
    }

    // check to see if it is Not-a-Number
    if(IsNaN(val))
    {
		kprintf("NaN");
        return;
    }
   
    // print the integer part of the floating point
	kprintf("%d", (int)val);
   
    // if precision == 0, only print the integer part
    if(!precision)
        return;
   
    // now on to the decimal potion
	kprintf(".");
   
    // remove the integer part
    val -= (double)((int)val);
   
    // on every iteration, make sure there are still decimal places left that are non-zero,
    // and make sure we're still within the user-defined precision range.
    while(val > (double)((int)val) && cur_prec++ < precision+1)
    {
        // move the next decimal into the integer portion and print it
        val *= 10;
		kprintf("%d", (int)val);
       
        // if the value is == the floored value (integer portion),
		// then there are no more decimal places that are non-zero. 
        if(val == (double)((int)val))
            return;
       
        // subtract the integer portion
        val -= (double)((int)val);
    }
}
*/

static int prints(char **out, const char *string, int width, int flags){
	int pc = 0, padchar = ' ';

	if (width > 0) {
		int len = 0;
		const char *ptr;
		for (ptr = string; *ptr; ++ptr) ++len;
		if (len >= width) width = 0;
		else width -= len;
		if (flags & PAD_ZERO)
			padchar = '0';
	}
	if (!(flags & PAD_RIGHT)) {
		for ( ; width > 0; --width) {
			simple_outputchar(out, padchar);
			++pc;
		}
	}
	for ( ; *string ; ++string) {
		simple_outputchar(out, *string);
		++pc;
	}
	for ( ; width > 0; --width) {
		simple_outputchar(out, padchar);
		++pc;
	}

	return pc;
}

#define PRINT_BUF_LEN 64
static int simple_outputi(char **out, long long i, int base, int sign, int width, int flags, int letbase){
	char print_buf[PRINT_BUF_LEN];
	char *s;
	int t, neg = 0, pc = 0;
	unsigned long long u = i;

	if (i == 0) {
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return prints(out, print_buf, width, flags);
	}

	if (sign && base == 10 && i < 0) {
		neg = 1;
		u = -i;
	}

	s = print_buf + PRINT_BUF_LEN-1;
	*s = '\0';

	while (u) {
		t = u % base;
		if( t >= 10 )
			t += letbase - '0' - 10;
		*--s = t + '0';
		u /= base;
	}

	if (neg) {
		if( width && (flags & PAD_ZERO) ) {
			simple_outputchar (out, '-');
			++pc;
			--width;
		}
		else {
			*--s = '-';
		}
	}

	return pc + prints (out, s, width, flags);
}

static int simple_vsprintf(char **out, char *format, va_list ap){
	int width, flags;
	int pc = 0;
	char scr[2];
	union {
		char c;
		char *s;
		int i;
		unsigned int u;
		long li;
		unsigned long lu;
		long long lli;
		unsigned long long llu;
		short hi;
		unsigned short hu;
		signed char hhi;
		unsigned char hhu;
		void *p;
		double d;
	} u;

	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;
			width = flags = 0;
			if (*format == '\0')
				break;
			if (*format == '%')
				goto out;
			if (*format == '-') {
				++format;
				flags = PAD_RIGHT;
			}
			while (*format == '0') {
				++format;
				flags |= PAD_ZERO;
			}
			if (*format == '*') {
				width = va_arg(ap, int);
				format++;
			} else {
				for ( ; *format >= '0' && *format <= '9'; ++format) {
					width *= 10;
					width += *format - '0';
				}
			}
			switch (*format) {
				case('d'):
					u.i = va_arg(ap, int);
					pc += simple_outputi(out, u.i, 10, 1, width, flags, 'a');
					break;

				case('u'):
					u.u = va_arg(ap, unsigned int);
					pc += simple_outputi(out, u.u, 10, 0, width, flags, 'a');
					break;

				case('x'):
					u.u = va_arg(ap, unsigned int);
					pc += simple_outputi(out, u.u, 16, 0, width, flags, 'a');
					break;

				case('X'):
					u.u = va_arg(ap, unsigned int);
					pc += simple_outputi(out, u.u, 16, 0, width, flags, 'A');
					break;

				case('c'):
					u.c = va_arg(ap, int);
					scr[0] = u.c;
					scr[1] = '\0';
					pc += prints(out, scr, width, flags);
					break;

				//case('f'):
				//	u.d = va_arg(ap, double);
				//	print_double_float(u.d, width);
				//	break;

				case('s'):
					u.s = va_arg(ap, char *);
					pc += prints(out, u.s ? u.s : "(null)", width, flags);
					break;
				case('l'):
					++format;
					switch (*format) {
						case('d'):
							u.li = va_arg(ap, long);
							pc += simple_outputi(out, u.li, 10, 1, width, flags, 'a');
							break;

						case('u'):
							u.lu = va_arg(ap, unsigned long);
							pc += simple_outputi(out, u.lu, 10, 0, width, flags, 'a');
							break;

						case('x'):
							u.lu = va_arg(ap, unsigned long);
							pc += simple_outputi(out, u.lu, 16, 0, width, flags, 'a');
							break;

						case('X'):
							u.lu = va_arg(ap, unsigned long);
							pc += simple_outputi(out, u.lu, 16, 0, width, flags, 'A');
							break;

						case('l'):
							++format;
							switch (*format) {
								case('d'):
									u.lli = va_arg(ap, long long);
									pc += simple_outputi(out, u.lli, 10, 1, width, flags, 'a');
									break;

								case('u'):
									u.llu = va_arg(ap, unsigned long long);
									pc += simple_outputi(out, u.llu, 10, 0, width, flags, 'a');
									break;

								case('x'):
									u.llu = va_arg(ap, unsigned long long);
									pc += simple_outputi(out, u.llu, 16, 0, width, flags, 'a');
									break;

								case('X'):
									u.llu = va_arg(ap, unsigned long long);
									pc += simple_outputi(out, u.llu, 16, 0, width, flags, 'A');
									break;

								default:
									break;
							}
							break;
						default:
							break;
					}
					break;
				case('h'):
					++format;
					switch (*format) {
						case('d'):
							u.hi = va_arg(ap, int);
							pc += simple_outputi(out, u.hi, 10, 1, width, flags, 'a');
							break;

						case('u'):
							u.hu = va_arg(ap, unsigned int);
							pc += simple_outputi(out, u.lli, 10, 0, width, flags, 'a');
							break;

						case('x'):
							u.hu = va_arg(ap, unsigned int);
							pc += simple_outputi(out, u.lli, 16, 0, width, flags, 'a');
							break;

						case('X'):
							u.hu = va_arg(ap, unsigned int);
							pc += simple_outputi(out, u.lli, 16, 0, width, flags, 'A');
							break;

						case('h'):
							++format;
							switch (*format) {
								case('d'):
									u.hhi = va_arg(ap, int);
									pc += simple_outputi(out, u.hhi, 10, 1, width, flags, 'a');
									break;

								case('u'):
									u.hhu = va_arg(ap, unsigned int);
									pc += simple_outputi(out, u.lli, 10, 0, width, flags, 'a');
									break;

								case('x'):
									u.hhu = va_arg(ap, unsigned int);
									pc += simple_outputi(out, u.lli, 16, 0, width, flags, 'a');
									break;

								case('X'):
									u.hhu = va_arg(ap, unsigned int);
									pc += simple_outputi(out, u.lli, 16, 0, width, flags, 'A');
									break;

								default:
									break;
							}
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
		}
		else {
out:
			simple_outputchar (out, *format);
			++pc;
		}
	}
	if (out) **out = '\0';
	return pc;
}

void khexdump(void * addr, unsigned int length){
	int i=0;
	unsigned char * d = (unsigned char*)addr;
	kprintf("+----------+-------------------------+----------+\r\n");
	for(i=0; i<length;){
		kprintf("| %08x | ", addr + i);
		int i_now = i;
		int i_end = i+8;
		if(i_end>=length) i_end = length;
		for(; i<i_end; i++){
			kprintf("%02x ", d[i]);
		}
		if(i_now+8>=length) for(;i<i_now+8; i++){
			kprintf("   ");
		}
		kprintf("| ");
		i_end = i_now+8;
		if(i_end>=length) i_end = length;
		for(i=i_now; i<i_end; i++){
			if(d[i]>=' ' && d[i]<='~')
				kprintf("%c", d[i]);
			else
				kprintf(".");
		}
		if(i_now+8>=length) for(;i<i_now+8; i++){
			kprintf(" ");
		}
		kprintf(" |\r\n");
	}
	kprintf("+----------+-------------------------+----------+\r\n");
}
