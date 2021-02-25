#include "pci.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <squire.h>
#include <squire_ddm.h>

#include "../include/pci.h"

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
    uint8_t class, subclass, prog;

	// Device management
	unsigned int driver_pid;
} pci_function_t;

unsigned int nr_pci_functions = 0;
pci_function_t * pci_functions;

pci_function_t * pci_get_function(char * device){
	char dname[64];
	pci_function_t * f = pci_functions;
	while(f){
		memset(dname, 0, 64);
		sprintf(dname, "PCI_ROOT/%02x_%02x_%02x", f->bus, f->slot, f->func);
		if(!strcmp(dname, device)){
			return f;
		}
		f = f->next;
	}
	return 0;
}

void pci_config_write_d(uint32_t bus, uint32_t slot, uint32_t func, uint32_t offset, uint32_t value){
	uint32_t address = (bus<<16) | (slot<<11) | (func<<8) | (offset&0xfc) | 0x80000000;
	squire_io_port_outd(PCI_CONFIG_ADDRESS, address);
	squire_io_port_outd(PCI_CONFIG_DATA, value);
}

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
	uint16_t subclass_class = pci_config_read_w(bus, slot, func, PCI_HEADER_SUBCLASS);
    uint16_t rev_prog = pci_config_read_w(bus, slot, func, PCI_HEADER_REVISION);
	// printf("PCI: %d.%d [%d] -> %04x:%04x\r\n", bus, slot, func, vendorid, deviceid);

	pci_function_t * f = (pci_function_t*)malloc(sizeof(pci_function_t));
	memset(f, 0, sizeof(pci_function_t));
	f->next = 0;
	f->vendor = vendorid;
	f->device = deviceid;
	f->bus = bus;
	f->slot = slot;
	f->func = func;
    f->class = subclass_class>>8;
    f->subclass = subclass_class&0xff;
    f->prog = rev_prog>>8;
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

void x86_generic_PCI_init(char * device){
    printf("%s] Initialize device\r\n", device);
    // Regiger IO ports
    int regstatus = 0;
    regstatus |= squire_io_register_port(PCI_CONFIG_ADDRESS, 4, IO_PORT_READ | IO_PORT_WRITE);
    if(regstatus) printf("Registering PCI_CONFIG_ADDRESS not succeeded: %d\r\n", regstatus);
    regstatus |= squire_io_register_port(PCI_CONFIG_DATA, 4, IO_PORT_READ | IO_PORT_WRITE);
    if(regstatus) printf("Registering PCI_CONFIG_DATA not succeeded: %d\r\n", regstatus);
}

void x86_generic_PCI_enum(char * device){
    printf("%s] Enumerate device\r\n", device);
    for(int b=0; b<256; b++){
        for(int d=0; d<32; d++){
            pci_check_device(b, d);
        }
    }
    // pci_functions contains all the devices

    // Register pci devices at device manager
    unsigned int nr_devices = nr_pci_functions;
    pci_function_t * func = pci_functions;
    for(int i=0; i<nr_devices; i++){
        char dname[64];
        char dtype[64];
        sprintf(dname, "%s/%02x_%02x_%02x", device, func->bus, func->slot, func->func);
        sprintf(dtype, "PCI:%02x-%02x-%02x|PCI:%04x-%04x", func->class, func->subclass, func->prog, func->vendor, func->device);
        squire_ddm_driver_register_device(dname, dtype, SQUIRE_DDM_DEVICE_TYPE_NONE, "PCI_ROOT");
        func = func->next;
    }
}

void x86_generic_PCI_idm(unsigned int function, void * data, size_t length, unsigned int from, unsigned int box){
	pci_function_header_t * fhdr = (pci_function_header_t*)data;
	switch(function){

		case PCI_FUNCTIONS_REGISTER_DRIVER:{
			pci_function_header_t rethdr;
			rethdr.status = 1;	// ERROR

			pci_function_t * f = pci_get_function(fhdr->name);
			if(f && f->driver_pid==0){
				rethdr.status = 0;	// NO ERROR
				f->driver_pid = from;
			}

			squire_ddm_driver_idmr("PCI", from, box, PCI_FUNCTIONS_REGISTER_DRIVER, &rethdr, sizeof(pci_function_header_t));
		} break;

		case PCI_FUNCTIONS_GET_CONFIG:{
			pci_function_header_t * rethdr = (pci_function_header_t*)malloc(sizeof(rethdr)+sizeof(pci_config_t));
			pci_config_t * config = (pci_config_t*)(rethdr+1);
			rethdr->status = 1;	// ERROR

			pci_function_t * f = pci_get_function(fhdr->name);
			if(f && f->driver_pid==from){
				rethdr->status = 0;	// NO ERROR

				// Read config space
				for(int i=0; i<=0x11; i++){
					config->d[i] = pci_config_read_d(f->bus, f->slot, f->func, i*4);
				}
			}
			squire_ddm_driver_idmr("PCI", from, box, PCI_FUNCTIONS_GET_CONFIG, rethdr, sizeof(pci_function_header_t)+sizeof(pci_config_t));
		} break;

		case PCI_FUNCTIONS_GET_REGIONS:{
			pci_function_header_t * rethdr = (pci_function_header_t*)malloc(sizeof(pci_function_header_t)+sizeof(pci_regions_t));
			pci_regions_t * regions = (pci_regions_t*)(rethdr+1);
			memset(regions, 0, sizeof(pci_regions_t));
			rethdr->status = 1;	// ERROR

			pci_function_t * f = pci_get_function(fhdr->name);
			if(f && f->driver_pid==from){
				rethdr->status = 0;	// NO ERROR
				for(int i=0; i<6; i++){
					regions->base[i] = pci_config_read_d(f->bus, f->slot, f->func, PCI_HEADER_00_BAR0+i*4);
					if((regions->base[i]&1)==0){
						// M BAR
						regions->flags[i] = regions->base[i]&0x0000000f;
						regions->base[i] &= 0xfffffff0;
						// Get length
						pci_config_write_d(f->bus, f->slot, f->func, PCI_HEADER_00_BAR0+i*4, 0xffffffff);
						regions->length[i] = pci_config_read_d(f->bus, f->slot, f->func, PCI_HEADER_00_BAR0+i*4)&0xfffffff0;
						pci_config_write_d(f->bus, f->slot, f->func, PCI_HEADER_00_BAR0+i*4, regions->base[i] | regions->flags[i]);
					}else{
						// IO BAR
						regions->flags[i] = regions->base[i]&0x00000003;
						regions->base[i] &= 0xfffffffc;
						// Get length
						pci_config_write_d(f->bus, f->slot, f->func, PCI_HEADER_00_BAR0+i*4, 0xffffffff);
						regions->length[i] = pci_config_read_d(f->bus, f->slot, f->func, PCI_HEADER_00_BAR0+i*4)&0xfffffffc;
						pci_config_write_d(f->bus, f->slot, f->func, PCI_HEADER_00_BAR0+i*4, regions->base[i] | regions->flags[i]);
					}
					regions->length[i] = (~regions->length[i])+1;
				}
			}
			squire_ddm_driver_idmr("PCI", from, box, PCI_FUNCTIONS_GET_REGIONS, rethdr, sizeof(pci_function_header_t)+sizeof(pci_regions_t));
		} break;

		default:{
			pci_function_header_t rethdr;
			rethdr.status = 1;
			squire_ddm_driver_idmr("PCI", from, box, function, &rethdr, sizeof(pci_function_header_t));
		} break;
	}
}