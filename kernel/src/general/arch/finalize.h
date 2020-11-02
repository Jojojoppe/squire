#ifndef __H_ARCH_FINALIZE
#define __H_ARCH_FINALIZE 1

/**
 * @brief Release resources specific to booting squire
 *
 * If putchar uses a resource (such as serial) it should not use
 * it after finalize() is called so user space drivers could take
 * over
 */
void finalize();

/**
 * @brief Retake boot specific resources in case of fatal error
 *
 * In case of a fata error the boot resources (such as serial) are
 * re-initialized so squire can show the error log
 */
void finalize_fatal_error();

#endif
