#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include <squire.h>
#include <squire_ddm.h>

void enumerate(char * device, char * type){
}

void init(char * device, char * type){
}

squire_ddm_driver_t driver_info = {
	"generic_ide", 1, 0,
	"PCI", 1, 0,		// Need a PCI bus driver of at least version 1.0
	1, 2,
	enumerate,
	init,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	{
		{"PCI:01-01-00"},
		{"PCI:01-01-80"},
		{"PCI:01-01-8a"},
	}
};
SQUIRE_DDM_DRIVER(driver_info);