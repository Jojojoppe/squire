#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include <squire.h>
#include <squire_ddm.h>

#define DRIVER_IMPLEMENTATION
#include <drivers/pci.h>

#include "ide.h"

squire_ddm_driver_t pci_driver;

void INTRhandler(int sig){
	unsigned int intr_id = squire_extraval0;
	if(ATA_PRIMARY_INTR==ATA_SECONDARY_INTR){
		// COMBINED IDE IRQ
	}else{
		if(intr_id==ATA_PRIMARY_INTR){
			// IDE PRIMARY IRQ
		}else if(intr_id==ATA_SECONDARY_INTR){
			// IDE SECONDARY IRQ
		}
	}
}

void enumerate(char * device, char * type){
	// ONLY ENUMERATE IDE BUS CONTROLLER, NOT DRIVES ITSELF
	if(!memcmp("IDE", type, 3)){
		return;
	}

	ide_initialize(ATA_PRIMARY_IO, ATA_PRIMARY_DCR_AS, ATA_SECONDARY_IO, ATA_SECONDARY_DCR_AS, 0);

	// Register devices
    for(int i=0; i<4; i++){
        if(ide_devices[i].available){
			char dname[64];
			char dtype[64];
			memset(dname, 0, 64);
			memset(dtype, 0, 64);
			sprintf(dname, "IDE-%d/%d", ide_devices[i].channel, ide_devices[i].drive);
			switch(ide_devices[i].type){
				case ATA_TYPE_PATA: strcpy(dtype, "IDE-PATA"); break;
				case ATA_TYPE_PATAPI: strcpy(dtype, "IDE-PATAPI"); break;
				case ATA_TYPE_SATA: strcpy(dtype, "IDE-SATA"); break;
				case ATA_TYPE_SATAPI: strcpy(dtype, "IDE-SATAPI"); break;
				default: continue;
			}
			squire_ddm_driver_register_device(dname, dtype, SQUIRE_DDM_DEVICE_TYPE_BLOCK, device);
        }
    }
}

void init(char * device, char * type){
	// ONLY INITIALIZE IDE HOST CONTROLLER, NOT DRIVE ITSELF
	if(!memcmp("IDE", type, 3)){
		return;
	}

	if(!pci_driver.pid){
		// If the IDE driver does not know the PCI driver yet, ask for it
		squire_ddm_driver_request_parent(&pci_driver);
		printf("IDE] parent driver '%s' on %d:%d\r\n", pci_driver.name, pci_driver.pid, pci_driver.child_box);
	}else{
		printf("IDE] driver can only control one IDE controller\r\n");
		return;
	}

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

	// Get IDE PCI settings
	uint8_t prog = config.b[9];
	if((prog&0x01)==1){
		// ATA_PRIMARY in PCI native mode
		ATA_PRIMARY_IO = regions.base[0];
		ATA_PRIMARY_DCR_AS  = regions.base[1];
		ATA_PRIMARY_INTR = config.b[0x3c];
		printf("IDE] ATA_PRIMARY in native PCI mode\r\n");
	}
	if((prog&0x04)==4){
		// ATA_SECONDARY in PCI native mode
		ATA_SECONDARY_IO = regions.base[0];
		ATA_SECONDARY_DCR_AS  = regions.base[1];
		ATA_SECONDARY_INTR = config.b[0x3c];
		printf("IDE] ATA_SECONDARY in native PCI mode\r\n");
	}

	// Register IO ports
	squire_io_register_port(ATA_PRIMARY_IO, 8, IO_PORT_READ|IO_PORT_WRITE);
	squire_io_register_port(ATA_SECONDARY_IO, 8, IO_PORT_READ|IO_PORT_WRITE);
	squire_io_register_port(ATA_PRIMARY_DCR_AS, 8, IO_PORT_READ|IO_PORT_WRITE);
	squire_io_register_port(ATA_SECONDARY_DCR_AS, 8, IO_PORT_READ|IO_PORT_WRITE);

	// Register interrupt
	signal(SIGINTR, INTRhandler);
	squire_io_register_isr(ATA_PRIMARY_INTR);
	if(ATA_PRIMARY_INTR!=ATA_SECONDARY_INTR){
		squire_io_register_isr(ATA_SECONDARY_INTR);
	}
}

squire_ddm_driver_t driver_info = {
	"x86_ide", 1, 0,
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
		{"PCI:01-01-8f"},
		{"IDE-PATA"},
		{"IDE-PATAPI"},
		{"IDE-SATA"},
		{"IDE-SATAPI"}
	}
};
SQUIRE_DDM_DRIVER(driver_info);
