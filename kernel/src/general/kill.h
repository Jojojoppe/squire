#ifndef __H_KILL
#define __H_KILL

typedef enum KILL_REASON{
    KILL_REASON_TERM = 0,
    KILL_REASON_KILL,
    KILL_REASON_ABRT,
    KILL_REASON_FPE,
    KILL_REASON_ILL,
    KILL_REASON_SEGV,
    KILL_REASON_ETC,
    KILL_REASON_INT,
} kill_reason_t;

typedef struct signal_s{
    kill_reason_t value;
    struct kill_reason_t * next;
    unsigned int source_tid;
} signal_t;

/**
 * @brief Kill a process
 * 
 * @param pid Process to kill, 0 if current process
 * @param reason 
 */
void kill(unsigned int pid, kill_reason_t reason);

/**
 * @brief Exit current running process (uses KILL_REASON_TERM)
 * 
 * @param retval
 */
void exit(unsigned int retval);

#endif