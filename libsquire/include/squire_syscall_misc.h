#ifndef __H_SQUIRE_SYSCALL_MISC
#define __H_SQUIRE_SYSCALL_MISC 1

#ifndef __H_SQUIRE_SYSCALL
    #error Do not include squire_syscall_xxx.h files directly! Use squire.h or squire_syscall.h instead.
#else

#include <stddef.h>

/**
 * @brief Misc operations
 * 
 */
typedef enum SQUIRE_SYSCALL_MISC_OPERATION{
    /**
     * @brief Add simple timer
     * 
     * longvalue0:      elapsed time
     * value0:          id
     */
    SQUIRE_SYSCALL_MISC_OPERATION_TIMER_ADD,
	/**
	 * @brief Notify kernel that booting is done and boot resources can be released
	 *
	 * Can only be issued by init (PID1), otherwise SIGILL will be raised
	 */
	SQUIRE_SYSCALL_MISC_OPERATION_FINALIZE
} squire_syscall_misc_operation_t;

/**
 * @brief Parameter structure for misc operation
 * 
 */
typedef struct{
    squire_syscall_misc_operation_t         operation;      /** @brief opcode */
    int                                     flags;          /** @brief flags */
    unsigned int                            value0;
    unsigned long long                      longvalue0;
} squire_syscall_misc_t;

// SQUIRE SYSCALL WRAPPER FUNCTIONS
// --------------------------------
#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief Add a simple timer
 * 
 * @param time 
 * @param id 
 */
extern void squire_misc_timer_add(unsigned long long time, unsigned int id);

/**
 * @brief Get elapsed time from within a SIGTIM handler
 * 
 */
#define squire_misc_timer_get_elapsed() (((unsigned long long)squire_extraval0)|((unsigned long long)squire_extraval1 << 32))

/**
 * @brief Get timer id from within a SIGTIM handler
 * 
 */
#define squire_misc_timer_get_id() (squire_extraval2)


extern void squire_misc_finalize();

#endif
#endif
