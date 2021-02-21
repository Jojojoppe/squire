#ifndef __H_DDM
#define __H_DDM 1

#include <stdint.h>
#include <squire_ddm.h>

typedef struct ddm_driver_s{
    struct ddm_driver_s * next;
    squire_ddm_driver_t driver_info;
    unsigned int pid;
} ddm_driver_t;

typedef struct ddm_device_s{
    struct ddm_device_s * next;
    struct ddm_device_s * children;
    ddm_driver_t * driver;
    squire_ddm_device_type_t device_type;
    char id[64];
    char type[64];
} ddm_device_t;

int ddm_main(void * p);

#endif

