#ifndef __H_SQUIRE_DDM
#define __H_SQUIRE_DDM 1

#include <stdint.h>

/*
 * Driver interface
 */

typedef struct{
    char type[64];
} squire_ddm_driver_supported_t;

typedef struct{
    // Basic driver info
    char name[64];                                          // Name of driver
    uint16_t version_major, version_minor;                  // Version of driver
    char parent_name[64];                                   // Type of parent driver
    uint16_t parent_version_major, parent_version_minor;    // Minimal parent driver version (same major, minimal minor)
    unsigned int box;                                       // Public box for communication
    unsigned int child_box;                                 // Box for driver-driver communication
    // Function callbacks
    void (*enum)(char * device, char * type);               // Enumerate device(bus) for sub devices
    void (*init)(char * device, char * type);               // Bind driver to device and initialize device
    void (*deinit)(char * device);                          // Deinitialize device and unbind driver
    size_t (*write)(char * data, size_t length, char * device);
    size_t (*read)(char * data, size_t length, char * device);
    size_t (*seek)(size_t position, char * device);
    void (*other)(void * msg);
    squire_ddm_driver_supported_t supported[32];
} squire_ddm_driver_t;

#define SQUIRE_DDM_DRIVER(DRIVER_INFO) \
    squire_ddm_driver_t * __ddm_driver_info = &DRIVER_INFO; \
    extern int squire_ddm_driver_main(int, char**); \
    int main(int argc, char ** argv){ \
        return squire_ddm_driver_main(argc, argv); \
        thrd_create(0, 0, 0); printf(""); \
    }

typedef enum{
    SQUIRE_DDM_DEVICE_TYPE_CHARACTER,
    SQUIRE_DDM_DEVICE_TYPE_BLOCK,
    SQUIRE_DDM_DEVICE_TYPE_NONE
} squire_ddm_device_type_t;

/*
 * ------
 */

/*
 * DDM interface
 */

#define SQUIRE_DDM_DRIVER_BOX 10
#define SQUIRE_DDM_USER_BOX 11
#define SQUIRE_DDM_PID 1

// Submessage type                                  // box: message direction
typedef enum{
    SQUIRE_DDM_SUBMESSAGE_REGISTER_DRIVER,          // DDM-DRIVER: DRIVER->DDM
    SQUIRE_DDM_SUBMESSAGE_INIT,                     // DDM-DRIVER: DDM->DRIVER
    SQUIRE_DDM_SUBMESSAGE_ENUM,                     // DDM-DRIVER: DDM->DRIVER
    SQUIRE_DDM_SUBMESSAGE_REGISTER_DEVICE,          // DDM-DRIVER: DRIVER->DDM
    SQUIRE_DDM_SUBMESSAGE_NONE                      // ALL: ALL
} squire_ddm_submessage_type_t;

// Main message header
typedef struct{
    uint32_t messages;
    uint32_t length;
} squire_ddm_message_header_t;

// After main message header submessages are concatenated. Each submessage has its own header
typedef struct{
    squire_ddm_submessage_type_t submessage_type;
    uint32_t length;
} squire_ddm_submessage_header_t;

/*
 * Content of submessages
 * ----------------------
 * 
 * + SQUIRE_DDM_SUBMESSAGE_REGISTER_DRIVER
 *      contains directly after the header a copy of the driver_info structure
 * + SQUIRE_DDM_SUBMESSAGE_INIT
 *      contains squire_ddm_submessage_init_t
 * + SQUIRE_DDM_SUBMESSAGE_ENUM
 *      contains squire_ddm_submessage_enum_t
 * + SQUIRE_DDM_SUBMESSAGE_REGISTER_DEVICE
 *      contains squire_ddm_submessage_register_device_t
 */

typedef struct{
    char * device[64];
    char * type[64];
    unsigned int parent_pid, parent_box;
} squire_ddm_submessage_init_t;

typedef struct{
    char * device[64];
    char * type[64];
} squire_ddm_submessage_enum_t;

typedef struct{
    char * device[64];
    char * type[64];
    char * parent[64];
    squire_ddm_device_type_t device_type;
} squire_ddm_submessage_register_device_t;

void squire_ddm_driver_register_device(char * device, char * type, squire_ddm_device_type_t device_type, char * parent);

/*
 * ------
 */

#endif