#ifndef __DRIVERS_PCI
#define __DRIVERS_PCI

#include <stdint.h>
#include <squire.h>
#include <squire_ddm.h>

enum PCI_FUNCTIONS{
    PCI_FUNCTIONS_GET_CONFIG
};

typedef union{
    uint32_t d[18];
    uint16_t w[18*2];
    uint8_t b[18*4];
} pci_config_t;

void pci_get_config(squire_ddm_driver_t * pci_driver, pci_config_t * pci_config);

#ifdef DRIVER_IMPLEMENTATION

    void pci_get_config(squire_ddm_driver_t * pci_driver, pci_config_t * pci_config){
        char buf[sizeof(squire_ddm_submessage_idm_t)+sizeof(pci_config_t)+sizeof(squire_ddm_submessage_header_t)+sizeof(squire_ddm_message_header_t)];
        squire_ddm_driver_idm("PCI", pci_driver->pid, pci_driver->child_box, PCI_FUNCTIONS_GET_CONFIG, NULL, 0, buf, sizeof(buf));
        memcpy(pci_config, buf+sizeof(squire_ddm_submessage_idm_t)+sizeof(squire_ddm_submessage_header_t)+sizeof(squire_ddm_message_header_t), sizeof(pci_config_t));
    }

#endif
#endif