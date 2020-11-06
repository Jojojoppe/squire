#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include <squire.h>
#include <squire_driver.h>

void function_callback(unsigned int from, squire_driver_submessage_function_t * func){
	printf("Function requested %d for device %s\r\n", func->function, func->id);
}

squire_driver_t driver_x86_generic = {
	"x86_generic",																	// Name of the driver
	1,0,																			// Version of the driver
	0,																				// Simple message box the driver listens to
	{
		{"x86_generic", DRIVER_FUNCTIONS_INIT|DRIVER_FUNCTIONS_ENUM|DRIVER_FUNCTIONS_DEINIT},
		{"x86_generic_PCI", DRIVER_FUNCTIONS_INIT|DRIVER_FUNCTIONS_ENUM|DRIVER_FUNCTIONS_DEINIT},
		{0},
	}																				// Supported device ID's
};
SQUIRE_DRIVER_INFO driver_info = {
	&driver_x86_generic,
	function_callback,
};
DRIVER(driver_info)
