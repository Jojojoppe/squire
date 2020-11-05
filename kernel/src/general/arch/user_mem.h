#ifndef __H_ARCH_USER_MEM
#define __H_ARCH_USER_MEM 1

#include <general/stdint.h>
#include <general/stddef.h>

/**
 * @brief Check if user may map a physical range in its user space
 *
 * @param physical Base address of psysical range
 * @param length Length of physical range
 * @return int Zero if allowed
 */
int user_mem_arch_may_map_phys(void * physical, size_t length);

#endif
