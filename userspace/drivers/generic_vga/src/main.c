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

typedef struct vga_device_s{
	struct vga_drvice_s * next;
	char device[64];
	uint8_t * vga_memory;
	unsigned int vga_memory_size;
} vga_device_t;

vga_device_t * vga_devices;

void init(char * device, char * type){
	if(!pci_driver.pid){
		// If the VGA driver does not know the PCI driver yet, ask for it
		squire_ddm_driver_request_parent(&pci_driver);
		printf("VGA] parent driver '%s' on %d:%d\r\n", pci_driver.name, pci_driver.pid, pci_driver.child_box);
	}

	// Initialize VGA device

	// Register driver at pci driver
	if(pci_register_driver(device, &pci_driver)){
		printf("VGA] error registering at pci driver for device %s\r\n", device);
		return;
	}
	// Read full config space
	pci_config_t config;
	if(pci_get_config(device, &pci_driver, &config)){
		printf("VGA] error reading configuration space of %s\r\n", device);
		return;
	}
	printf("VGA] configuration space of %s read: device %04x:%04x\r\n", device, config.w[0], config.w[1]);
	// Read MMIO regions
	pci_regions_t regions;
	if(pci_get_regions(device, &pci_driver, &regions)){
		printf("VGA] error reading MMIO regions of %s\r\n", device);
		return;
	}
	for(int i=0; i<6; i++){
		printf("VGA] BAR%d %08x[%08x] %08x\r\n", i, regions.base[i], regions.length[i], regions.flags[i]);
	}

	// Map VGA memory
	uint8_t * vga_mem = squire_memory_mmap_phys(0, regions.base[0], regions.length[0], MMAP_READ|MMAP_WRITE);

	// Create device structure for later use
	vga_device_t * d = (vga_device_t*)malloc(sizeof(vga_device_t));
	if(vga_devices){
		vga_device_t * l = vga_devices;
		while(l->next) l=l->next;
		l->next = d;
	}else{
		vga_devices = d;
	}
	d->next = 0;
	strcpy(d->device, device);
	d->vga_memory = vga_mem;
	d->vga_memory_size = regions.length[0];

	// Set vga mode and clear screen
	// ASSUME VGA IS IN 640x480x32 mode and memory mapped area is big enough for full framebuffer
	for(int x=0; x<640*480; x++){
									//  AARRGGBB
			((uint32_t*)vga_mem)[x] = 0x00ffffff;
	}
}

squire_ddm_driver_t driver_info = {
	"generic_vga", 1, 0,
	"PCI", 1, 0,		// Need a PCI bus driver of at least version 1.0
	0, 1, 2,
	NULL,
	NULL,
	init,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	{
		{"PCI:03-00-00"}
	}
};
SQUIRE_DDM_DRIVER(driver_info);