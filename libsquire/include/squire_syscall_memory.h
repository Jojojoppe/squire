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
     * address0:        Base of region to allocate. Zero If kernel may decide
     * length0:         Length of retion to allocate
     * After execution address0 contains the starting address of the allocation and length0
     * contains the length of the allocation
     */
    SQUIRE_SYSCALL_MEMORY_OPERATION_MMAP,
	/**
	 * @brief Map a specific physical memory range
	 *
	 * address0:		Base of virtual memory to map to. Zero if kernel may decide
	 * address1:		Base of physical memory to map
	 * length0:			Length of region
	 * After execution address0 contains the starting address of the allocation and length contains the lenght of the allocation. Length0 and address0 will be zero if mapping is unsuccessful
	 */
	SQUIRE_SYSCALL_MEMORY_OPERATION_MMAP_PHYS,
	/**
	 * @brief Create a shared memory region
	 *
	 * address0:		Base of virtual memory to map to. Zero if kernel may decide
	 * length0:			Length of region
	 * id0:				ID of shared memory region
	 * After execution address0 contains the starting address of the allocation and length0
	 * contains the length of the allocation
	 */
	SQUIRE_SYSCALL_MEMORY_OPERATION_CREATE_SHARED,
	/**
	 * @brief Map a shared memory region
	 * address0:		Base of virtual memory to map to. Zero if kernel may decide
	 * pid0:			PID of the owner
	 * id0:				ID of shared memory region
	 * After execution address0 contains the starting address of the allocation and length0
	 * contains the length of the allocation
	 */
	SQUIRE_SYSCALL_MEMORY_OPERATION_MAP_SHARED
} squire_syscall_memory_operation_t;

/**
 * @brief Parameter structure for memory operation
 * 
 */
typedef struct{
    squire_syscall_memory_operation_t       operation;      /** @brief opcode */
    int                                     flags;          /** @brief flags */
    void *                                  address0;       /** @brief first address */
    void *                                  address1;       /** @brief second address */
    size_t                                  length0;        /** @brief first length */
    size_t                                  length1;        /** @brief second length */
	char									id0[32];
	unsigned int							pid0;
} squire_syscall_memory_t;

// SQUIRE SYSCALL WRAPPER FUNCTIONS
// --------------------------------
#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief Allocate a region of memory
 * 
 * @param addr Start of region to allocate, zero if kernel may decide
 * @param length Length of region to allocate
 * @param flags Access permission (MMAP_* flags)
 * @return void* Base address of allocated region
 */
extern void * squire_memory_mmap(void * addr, size_t length, int flags);

/**
 * @brief Map a physical region of memory
 *
 * @param addr Start of virtual memory to map to, zero if kernel may decide
 * @param phys Start of physical memory to map
 * @param length Length of region to map
 * @param flags Access permission (MMAP_* flags)
 * @return void* Base address of allocated region
 */
extern void * squire_memory_mmap_phys(void * addr, void * phys, size_t length, int flags);

extern void * squire_memory_create_shared(void * addr, size_t length, char id[32], int flags);

extern void * squire_memory_map_shared(void * addr, unsigned int pid, char id[32], int flags);

#if defined(__cplusplus)
} /* extern "C" */
#endif
#endif
#endif
