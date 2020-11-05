#include <general/arch/user_mem.h>
#include <general/mboot.h>
#include <general/kmalloc.h>
#include <general/kprintf.h>
#include <general/config.h>

int user_mem_arch_may_map_phys(void * physical, size_t length){
	// TODO CHECK RANGE!!!!
	return 0;
}
