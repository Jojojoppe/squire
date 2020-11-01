#ifndef __H_SQUIRE_SYSCALL_MEMORY
#define __H_SQUIRE_SYSCALL_MEMORY 1

#ifndef __H_SQUIRE_SYSCALL
    #error Do not include squire_syscall_xxx.h files directly! Use squire.h or squire_syscall.h instead.
#else

#include <stddef.h>

// Operation flags
#define MMAP_EXEC       1           /** @brief Region is executable */
#define MMAP_READ       2           /** @brief Region is readable */
#define MMAP_WRITE      4           /** @brief Region is writable */

/**
 * @brief Memory operations
 * 
 */
typedef enum SQUIRE_SYSCALL_MEMORY_OPERATION{
    /**
     * @brief Map memory in address space of process
     * 
     * Allocates and maps memory in processes address space. The access protection is set
     * accordingly to the first three bits of the flags: (MMAP_EXEC, MMAP_READ, MMAP_WRITE)
     * address0:        Base of region to allocate
     * length0:         Length of retion to allocate
     * After execution address0 contains the starting address of the allocation and length0
     * contains the length of the allocation
     */
    SQUIRE_SYSCALL_MEMORY_OPERATION_MMAP
} squire_syscall_memory_operation_t;

/**
 * @brief Parameter structure for memory operation
 * 
 */
typedef struct{
    squire_syscall_memory_operation_t       operation;      /** @brief opcode */
    int                                     flags;          /** @brief flags */
    void *                                  address0;       /** @brief first address */
    size_t                                  length0;        /** @brief first length */
} squire_syscall_memory_t;

// SQUIRE SYSCALL WRAPPER FUNCTIONS
// --------------------------------
#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief Allocate a region of memory
 * 
 * @param addr Start of region to allocate
 * @param length Length of region to allocate
 * @param flags Access permission (MMAP_* flags)
 * @return void* Base address of allocated region
 */
extern void * squire_memory_mmap(void * addr, size_t length, int flags);

#if defined(__cplusplus)
} /* extern "C" */
#endif
#endif
#endif