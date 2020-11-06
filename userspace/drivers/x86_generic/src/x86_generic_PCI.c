#include <x86_generic_PCI.h>

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <squire.h>

#define PCI_CONFIG_ADDRESS 0xcf8
#define PCI_CONFIG_DATA 0xcfc

#define PCI_HEADER_VENDOR 0
#define PCI_HEADER_DEVICE 2
#define PCI_HEADER_COMMAND 4
#define PCI_HEADER_STATUS 6
#define PCI_HEADER_REVISION 8
#define PCI_HEADER_PROG 9
#define PCI_HEADER_SUBCLASS 10
#define PCI_HEADER_CLASS 11
#define PCI_HEADER_CLSIZE 12
#define PCI_HEADER_LATENCY 13
#define PCI_HEADER_HEADER 14
#define PCI_HEADER_BIST 15

#define PCI_HEADER_00_BAR0 16
#define PCI_HEADER_00_BAR1 20
#define PCI_HEADER_00_BAR2 24
#define PCI_HEADER_00_BAR3 28
#define PCI_HEADER_00_BAR4 32
#define PCI_HEADER_00_BAR5 36
#define PCI_HEADER_00_CBCISP 40
#define PCI_HEADER_00_SUBVENDOR 44
#define PCI_HEADER_00_SUBSYSTEM 46
#define PCI_HEADER_00_ROM 48
#define PCI_HEADER_00_CAPAB 52
#define PCI_HEADER_00_INTRLINE 60
#define PCI_HEADER_00_INTRPIN 61
#define PCI_HEADER_00_MINGRANT 62
#define PCI_HEADER_00_MAXLATENCY 62


typedef struct pci_function_s{
	struct pci_function_s * next;
	uint16_t vendor, device;
	uint8_t bus, slot, func;
} pci_function_t;

unsigned int nr_pci_functions = 0;
pci_function_t * pci_functions;


uint32_t pci_config_read_d(uint32_t bus, uint32_t slot, uint32_t func, uint32_t offset){
	uint32_t address = (bus<<16) | (slot<<11) | (func<<8) | (offset&0xfc) | 0x80000000;
	squire_io_port_outd(PCI_CONFIG_ADDRESS, address);
	uint32_t tmp;
	squire_io_port_ind(PCI_CONFIG_DATA, &tmp);
	return tmp;
}
uint16_t pci_config_read_w(uint32_t bus, uint32_t slot, uint32_t func, uint32_t offset){
	uint32_t address = (bus<<16) | (slot<<11) | (func<<8) | (offset&0xfc) | 0x80000000;
	squire_io_port_outd(PCI_CONFIG_ADDRESS, address);
	uint32_t tmp;
	squire_io_port_ind(PCI_CONFIG_DATA, &tmp);
	return (tmp>>((offset&2)*8))&0xffff;
}

void pci_check_function(uint32_t bus, uint32_t slot, uint32_t func){
	uint16_t vendorid = pci_config_read_w(bus, slot, func, PCI_HEADER_VENDOR);
	uint16_t deviceid = pci_config_read_w(bus, slot, func, PCI_HEADER_DEVICE);
	uint16_t class = pci_config_read_w(bus, slot, func, PCI_HEADER_SUBCLASS);
//	printf("PCI: %d.%d [%d] -> %04x:%04x [%04x]\r\n", bus, slot, func, vendorid, deviceid, class);

	pci_function_t * f = (pci_function_t*)malloc(sizeof(pci_function_t));
	f->next = 0;
	f->vendor = vendorid;
	f->device = deviceid;
	f->bus = bus;
	f->slot = slot;
	f->func = func;
	if(pci_functions){
		pci_function_t * t = pci_functions;
		while(t->next) t = t->next;
		t->next = f;
	}else{
		pci_functions = f;
	}
	nr_pci_functions++;

}

void pci_check_device(uint32_t bus, uint32_t device){
	uint16_t vendor = pci_config_read_w(bus, device, 0, PCI_HEADER_VENDOR);
	if(vendor==0xffff) return;
	pci_check_function(bus, device, 0);
	uint8_t headertype = (uint8_t)pci_config_read_w(bus, device, 0, PCI_HEADER_HEADER);
	if((headertype&0x80)!=0){
		// Multifunction device
		for(int i=1; i<8; i++){
			uint16_t func_vendor = pci_config_read_w(bus, device, i, PCI_HEADER_VENDOR);
			if(func_vendor!=0xffff){
				pci_check_function(bus, device, i);
			}
		}
	}
}

void x86_generic_PCI_function_callback(unsigned int from, squire_driver_submessage_function_t * func){
	switch(func->function){
	
		case DRIVER_FUNCTIONS_INIT:{
			printf("x86_generic_PCI %08x] Initialize device\r\n", func->instance);
			// Regiger IO ports
			int regstatus = 0;
			regstatus |= squire_io_register_port(PCI_CONFIG_ADDRESS, 4, IO_PORT_READ | IO_PORT_WRITE);
			if(regstatus) printf("Registering PCI_CONFIG_ADDRESS not succeeded: %d\r\n", regstatus);
			regstatus |= squire_io_register_port(PCI_CONFIG_DATA, 4, IO_PORT_READ | IO_PORT_WRITE);
			if(regstatus) printf("Registering PCI_CONFIG_DATA not succeeded: %d\r\n", regstatus);
		} break;

		case DRIVER_FUNCTIONS_ENUM:{
			printf("x86_generic_PCI %08x] Enumerate device\r\n", func->instance);
			for(int b=0; b<256; b++){
				for(int d=0; d<32; d++){
					pci_check_device(b, d);
				}
			}
			// pci_functions contains all the devices

			// Register pci devices at device manager
			unsigned int nr_devices = nr_pci_functions;
			// Message size
			size_t msg_size = sizeof(squire_driver_message_t) + nr_devices*(sizeof(squire_driver_submessage_t)+sizeof(squire_driver_submessage_device_t));
			squire_driver_message_t * msg = (squire_driver_message_t*)alloca(msg_size);
			msg->amount_messages = nr_devices;
			squire_driver_submessage_t * submsg = msg->messages;
			pci_function_t * func = pci_functions;
			for(int i=0; i<nr_devices; i++){
				strcpy(submsg->name, "");
				submsg->type = SUBMESSAGE_TYPE_O_REGDEVICE;
				submsg->size = sizeof(squire_driver_submessage_device_t);
				squire_driver_submessage_device_t * dev = (squire_driver_submessage_device_t*)submsg->content;
				strcpy(dev->parent, "x86_generic_PCI");
				sprintf(dev->id, "PCI_%04x:%04x", func->vendor, func->device);
				dev->instance = 0;
				dev->instance = ((func->bus<<16)&0xff0000) | ((func->slot<<8)&0xff00) | (func->func&0xff);
				submsg = (squire_driver_submessage_t*)((void*)&submsg->content + submsg->size);
				func = func->next;
			}
			squire_message_simple_box_send(msg, msg_size, device_manager_pid, device_manager_box);


		} break;

		default:
			printf("x86_generic_PCI %08x] Unknown function requested\r\n", func->instance);
			break;
	}
}
