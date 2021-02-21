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
	pci_config_t config;
	pci_get_config(&pci_driver, &config);
	printf("Config read\r\n");
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
		{"PCI:01-01-80"},
		{"PCI:01-01-8a"},
	}
};
SQUIRE_DDM_DRIVER(driver_info);