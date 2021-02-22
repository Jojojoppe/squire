#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include <squire.h>
#include <squire_ddm.h>

#define DRIVER_IMPLEMENTATION
#include <drivers/pci.h>

squire_ddm_driver_t pci_driver;

void enumerate(char * device, char * type){
}

void init(char * device, char * type){
	if(!pci_driver.pid){
		// If the IDE driver does not know the PCI driver yet, ask for it
		squire_ddm_driver_request_parent(&pci_driver);
		printf("IDE] parent driver '%s' on %d:%d\r\n", pci_driver.name, pci_driver.pid, pci_driver.child_box);
	}

	// Initialize IDE device

	// Register driver at pci driver
	if(pci_register_driver(device, &pci_driver)){
		printf("IDE] error registering at pci driver for device %s\r\n", device);
		return;
	}
	// Read full config space
	pci_config_t config;
	if(pci_get_config(device, &pci_driver, &config)){
		printf("IDE] error reading configuration space of %s\r\n", device);
		return;
	}
	printf("IDE] configuration space of %s read: device %04x:%04x\r\n", device, config.w[0], config.w[1]);
	// Read MMIO regions
	pci_regions_t regions;
	if(pci_get_regions(device, &pci_driver, &regions)){
		printf("IDE] error reading MMIO regions of %s\r\n", device);
		return;
	}
	for(int i=0; i<6; i++){
		printf("IDE] BAR%d %08x[%08x] %08x\r\n", i, regions.base[i], regions.length[i], regions.flags[i]);
	}
}

squire_ddm_driver_t driver_info = {
	"generic_ide", 1, 0,
	"PCI", 1, 0,		// Need a PCI bus driver of at least version 1.0
	0, 1, 2,
	NULL,
	enumerate,
	init,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	{
		{"PCI:01-01-00"},
		{"PCI:01-01-05"},
		{"PCI:01-01-0a"},
		{"PCI:01-01-0f"},
		{"PCI:01-01-80"},
		{"PCI:01-01-85"},
		{"PCI:01-01-8a"},
		{"PCI:01-01-8f"}
	}
};
SQUIRE_DDM_DRIVER(driver_info);