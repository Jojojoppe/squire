#ifndef __H_WAIT
#define __H_WAIT 1

#include <general/kill.h>

/**
 * @brief Wait for a child process to end
 * 
 * @param retval Pointer to store the return value in
 * @param pid PID of the child, 0 for all childs
 * @return Way of exit of child
 */
kill_reason_t wait(unsigned int * retval, unsigned int pid);

#endif