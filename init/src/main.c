#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <stdint.h>

#include <squire.h>

#define PCI_CONFIG_ADDRESS 0xcf8
#define PCI_CONFIG_DATA 0xcfc

uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
	uint32_t indat;
 
    /* create configuration address as per Figure 1 */
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
 
    squire_io_port_outd(PCI_CONFIG_ADDRESS, address);
	squire_io_port_ind(PCI_CONFIG_DATA, &indat);
    tmp = (uint16_t)((indat >> ((offset & 2) * 8)) & 0xffff);
    return (tmp);
}

void pci_check_bus(uint8_t bus){
	uint8_t device;
	for(device=0; device<32; device++){
		uint16_t vendor = pci_config_read_word(bus, device, 0, 0);
		if(vendor != 0xffff){
			uint16_t devid = pci_config_read_word(bus, device, 0, 2);
			uint16_t class = pci_config_read_word(bus, device, 0, 10);
			uint32_t BAR0 = pci_config_read_word(bus, device, 0, 0x10) | pci_config_read_word(bus, device, 0, 0x12)<<16;
			printf("PCI %d.%d vendor: %04x device: %04x class: %04x BAR0:%08x\r\n", bus, device, vendor, devid, class, BAR0&0xfffffff0);
		}
	}
}

void putpixel(unsigned char * vmem, int x, int y, int color){
	unsigned pos = y*640*4 + x*4;
	vmem[pos] = color&0xff;
	vmem[pos+1] = (color>>8)&0xff;
	vmem[pos+2] = (color>>16)&0xff;
}

int main(int argc, char ** argv){
	printf("Main thread of init.bin\r\n");

	// Register PCI IO ports
	squire_io_register_port(PCI_CONFIG_ADDRESS, 4, IO_PORT_READ | IO_PORT_WRITE);
	squire_io_register_port(PCI_CONFIG_DATA, 4, IO_PORT_READ | IO_PORT_WRITE);

	pci_check_bus(0);

	// Test for physical memory mapping
	unsigned char * vmem = squire_memory_mmap_phys(0, 0xa0000, 1228800, MMAP_WRITE);

	for(int x=0; x<10; x++){
		for(int y=0; y<10; y++){
			putpixel(vmem, x, y, 0xffffffff);
		}
	}

	for(;;);
	return 0;
} 
