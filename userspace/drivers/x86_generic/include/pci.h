#ifndef __DRIVERS_PCI
#define __DRIVERS_PCI

#include <stdint.h>
#include <string.h>
#include <squire.h>
#include <squire_ddm.h>

enum PCI_FUNCTIONS{
    PCI_FUNCTIONS_REGISTER_DRIVER,
    PCI_FUNCTIONS_GET_CONFIG
};

typedef struct{
    char name[64];
    int status;
} pci_function_header_t;

typedef union{
    uint32_t d[18];
    uint16_t w[18*2];
    uint8_t b[18*4];
} pci_config_t;

int pci_register_driver(char * device, squire_ddm_driver_t * pci_driver);
int pci_get_config(char * device, squire_ddm_driver_t * pci_driver, pci_config_t * pci_config);

#ifdef DRIVER_IMPLEMENTATION

    int pci_register_driver(char * device, squire_ddm_driver_t * pci_driver){
        pci_function_header_t fheader;
        strcpy(fheader.name, device);
        char buf[sizeof(squire_ddm_submessage_idm_t)+sizeof(pci_function_header_t)+sizeof(squire_ddm_submessage_header_t)+sizeof(squire_ddm_message_header_t)];
        squire_ddm_driver_idm("PCI", pci_driver->pid, pci_driver->child_box, PCI_FUNCTIONS_REGISTER_DRIVER, &fheader, sizeof(pci_function_header_t), buf, sizeof(buf));
        return ((pci_function_header_t*)(buf+sizeof(squire_ddm_message_header_t)+sizeof(squire_ddm_submessage_header_t)+sizeof(squire_ddm_submessage_idm_t)))->status;
    }

    int pci_get_config(char * device, squire_ddm_driver_t * pci_driver, pci_config_t * pci_config){
        pci_function_header_t fheader;
        strcpy(fheader.name, device);
        char buf[sizeof(squire_ddm_submessage_idm_t)+sizeof(pci_config_t)+sizeof(pci_function_header_t)+sizeof(squire_ddm_submessage_header_t)+sizeof(squire_ddm_message_header_t)];
        squire_ddm_driver_idm("PCI", pci_driver->pid, pci_driver->child_box, PCI_FUNCTIONS_GET_CONFIG, &fheader, sizeof(pci_function_header_t), buf, sizeof(buf));
        memcpy(pci_config, buf+sizeof(squire_ddm_submessage_idm_t)+sizeof(squire_ddm_submessage_header_t)+sizeof(squire_ddm_message_header_t)+sizeof(pci_function_header_t), sizeof(pci_config_t));
        return ((pci_function_header_t*)(buf+sizeof(squire_ddm_message_header_t)+sizeof(squire_ddm_submessage_header_t)+sizeof(squire_ddm_submessage_idm_t)))->status;
    }

#endif
#endif