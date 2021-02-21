#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include <squire.h>
#include <squire_ddm.h>

#include "cmos.h"
#include "pci.h"

void INTRhandler(int sig){
	unsigned int intr_id = squire_extraval0;
	if(intr_id==RTC_INTR){
		x86_generic_CMOS_INTR();
	}
}

void enumerate(char * device, char * type){
	if(!strcmp(type, "ROOT")){
		// Enumerate motherboard -> register directly connected devices
		squire_ddm_driver_register_device("PCI_ROOT", "PCI", SQUIRE_DDM_DEVICE_TYPE_NONE, "_");
		squire_ddm_driver_register_device("CMOS", "CMOS", SQUIRE_DDM_DEVICE_TYPE_NONE, "_");
	}else if(!strcmp(device, "PCI_ROOT")){
		x86_generic_PCI_enum(device);
	}
}

void init(char * device, char * type){
	if(!strcmp(type, "ROOT")){
		signal(SIGINTR, INTRhandler);
	}else if(!strcmp(type, "CMOS")){
		x86_generic_CMOS_init();
	}else if(!strcmp(device, "PCI_ROOT")){
		x86_generic_PCI_init(device);
	}
}

squire_ddm_driver_t driver_info = {
	"x86_generic", 1, 0,
	"\0", 0, 0,
	1, 2,
	enumerate,
	init,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	{
		{"ROOT"},
		{"PCI"},
		{"CMOS"}
	}
};
SQUIRE_DDM_DRIVER(driver_info);