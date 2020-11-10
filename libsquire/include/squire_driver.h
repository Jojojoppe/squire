#ifndef __H_SQUIRE_DRIVER
#define __H_SQUIRE_DRIVER 1

#include <stdint.h>

// Driver specification
// --------------------
#define DRIVER_FUNCTIONS_INIT		1			// Initialize device
#define DRIVER_FUNCTIONS_DEINIT		2			// Deinitialize device
#define DRIVER_FUNCTIONS_ENUM		4			// Enumerate device
typedef struct{
	uint8_t id[64];								// Identifier of a device
	uint32_t functions;							// Accepted functions by the device
} squire_driver_supported_t __attribute__((__packed__));
typedef struct{
	char name[64];								// Name of the driver
	uint16_t version_major, version_minor;		// Version of the driver
	uint32_t simple_box;						// The simple message box the driver listens to
	squire_driver_supported_t supported[];		// List of supported devices, end with zero entry
} squire_driver_t __attribute__((__packed__));
// --------------------

// Driver message format
// ---------------------
// in messages
#define SUBMESSAGE_TYPE_I_FUNCTION			1	// The driver must execute a function
// out messages
#define SUBMESSAGE_TYPE_O_REGDEVICE			2	// Register a device
#define SUBMESSAGE_TYPE_O_INFODRIVER		4	// Send driver information
#define SUBMESSAGE_TYPE_O_ERROR				8	// Send a driver error

// Submessage content structures
// ---
typedef struct{
	uint8_t id[64];								// Identifier of the device
	uint32_t instance;							// Instance of the device
	uint32_t function;							// The function to execute
} squire_driver_submessage_function_t __attribute__((__packed__));

typedef struct{
	squire_driver_t driverinfo;					// The driver information structure
} squire_driver_submessage_infodriver_t __attribute__((__packed__));

typedef struct{
	
} squire_driver_submessage_error_t __attribute__((__packed__));

typedef struct{
	uint8_t parent[64];							// Id of the parent device
	uint32_t parent_instance;
	uint8_t id[64];								// Identification of the device
	uint32_t instance;							// Instance of the device
} squire_driver_submessage_device_t __attribute__((__packed__));
// ---

typedef struct{
	char name[64];								// The name of the driver for which the message is intended (zero's for message to device manager)
	uint32_t type;								// The submessage type
	uint32_t size;								// Submessage size
	uint8_t content[];
} squire_driver_submessage_t __attribute__((__packed__));
typedef struct{
	uint8_t amount_messages;					// The amount of submessages the message contains
	squire_driver_submessage_t messages[];
} squire_driver_message_t __attribute__((__packed__));
// ---------------------


// Macro functions for driver development
typedef struct{
	squire_driver_t * driver_info;
	void (*function_callback)(unsigned int from, squire_driver_submessage_function_t *);
} SQUIRE_DRIVER_INFO;

extern unsigned int device_manager_pid;
extern unsigned int device_manager_box;

#define DRIVER(DRIVER_INFO) \
	SQUIRE_DRIVER_INFO * _driver_info = &DRIVER_INFO ; \
	extern int driver_main(int, char**); \
	int main(int argc, char ** argv){ \
		atoi("0"); strcpy("",""); strcmp("",""); \
		return driver_main(argc, argv); \
	}

#endif
