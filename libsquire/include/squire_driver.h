#ifndef __H_SQUIRE_DRIVER
#define __H_SQUIRE_DRIVER 1

// Driver specification
// --------------------
#define DRIVER_FUNCTIONS_INIT		1			// Initialize device
#define DRIVER_FUNCTIONS_DEINIT		2			// Deinitialize device
#define DRIVER_FUNCTIONS_ENUM		4			// Enumerate device
typedef struct{
	char name[64];								// Name of the device
	uint8_t id[64];								// Identifier of the device
} squire_device_t;
typedef struct{
	char name[64];								// Name of the driver
	uint16_t version_major, version_minor;		// Version of the driver
	uint64_t functions;							// Accepted functions by the driver
	squire_device_t devices[];					// The devices. End with zero'ed entry
} squire_driver_t;
// --------------------

// Driver message format
// ---------------------
// in messages
#define SUBMESSAGE_TYPE_I_FUNCTION			1	// The driver must execute a function
// out messages
#define SUBMESSAGE_TYPE_O_REGDEVICE			2	// Register a device
typedef struct{
	uint64_t function;							// The function to execute
	uint8_t id[64];								// Identifier of the device
	uint16_t paramlen;							// Length of the parameter field
	uint8_t param[];							// The parameters
} squire_driver_submessage_function_t;
typedef struct{
	uint32_t size;								// Size of the message
	char name[64];								// The name of the driver for which the message is intended (zero's for message to device manager)
	uint32_t type;								// The submessage type
	union{
		squire_driver_submessage_function_t function;
	} content;									// The content of the message
} squire_driver_submessage_t;
typedef struct{
	uint8_t amount_messages;					// The amount of submessages the message contains
	squire_driver_submessage_t messages[];
} squire_driver_message_t;
// ---------------------



#endif
