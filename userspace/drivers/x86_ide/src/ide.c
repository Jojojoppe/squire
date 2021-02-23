#include "ide.h"
#include <squire.h>
#include <squire_ddm.h>
#include <drivers/pci.h>

ide_device_t ide_devices[4];

unsigned char ide_buf[2048];

unsigned int ATA_PRIMARY_IO = 0x1f0;
unsigned int ATA_PRIMARY_DCR_AS = 0x3f6;
unsigned int ATA_SECONDARY_IO = 0x170;
unsigned int ATA_SECONDARY_DCR_AS = 0x376A;
unsigned int ATA_PRIMARY_INTR = 0x20+14;
unsigned int ATA_SECONDARY_INTR = 0x20+15;

unsigned char inb(unsigned short port){
    unsigned char tmp;
    squire_io_port_inb(port, &tmp);
    return tmp;
}
unsigned short inw(unsigned short port){
    unsigned short tmp;
    squire_io_port_inw(port, &tmp);
    return tmp;
}

void outb(unsigned short port, unsigned char value){
    squire_io_port_outb(port, value);
}

void ide_400ns(unsigned int base_ctrl){
    for(int i=0; i<4; i++)
        inb(base_ctrl);
}

void ide_detect(unsigned int base_io, unsigned int base_ctrl, unsigned int channel, unsigned int drive){
    ide_devices[drive+2*channel].available = 0;

    // Reset drive
    outb(base_ctrl, ATA_CMD_RESET);
    ide_400ns(base_ctrl);
    outb(base_ctrl, 0);
    ide_400ns(base_ctrl);

    // Select drive, send 0xa0 to master, 0xb0 to slave
    outb(base_io+ATA_REG_HDDEVSEL, (0xa+drive)<<4);

    // Set sector counts and lbas to 0
    outb(base_io+ATA_REG_SECCOUNT0, 0);
    outb(base_io+ATA_REG_LBA0, 0);
    outb(base_io+ATA_REG_LBA1, 0);
    outb(base_io+ATA_REG_LBA2, 0);
    ide_400ns(base_ctrl);

    // Send identify
    outb(base_io+ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
    ide_400ns(base_ctrl);
    unsigned char stat = inb(base_io+ATA_REG_STATUS);
    if(!stat){
        printf("ide_detect: not existing\r\n");
        return;
    }

    if((stat&0x01)==0){
        // Probably ATA device: Poll until BSY clears
        // TODO build in time-out
        // while((inb(base_io+ATA_REG_STATUS)&0x80)==0x80);
    }

    // Differentiate ATA, ATAPI, SATA and SATAPI
    unsigned char lba1 = inb(base_io+ATA_REG_LBA1);
    unsigned char lba2 = inb(base_io+ATA_REG_LBA2);
    int type = ATA_TYPE_UNKNOWN;
    if(lba1==0x14 && lba2==0xeb) type = ATA_TYPE_PATAPI;
    if(lba1==0x69 && lba2==0x96) type = ATA_TYPE_SATAPI;
    if(lba1==0x00 && lba2==0x00) type = ATA_TYPE_PATA;
    if(lba1==0x3c && lba2==0xc3) type = ATA_TYPE_SATA;
    if(type!=ATA_TYPE_PATA){
        printf("ide_detect: device not ATA but %d\r\n", type);
        return;
    }

    // Poll until DRQ sets or ERR sets
    // TODO build in time-out
    while(1){
        stat = inb(base_io+ATA_REG_STATUS);
        if((stat&0x08)==8){
            break;
        }else if((stat&0x01)==1){
            printf("ide_detect: error\r\n");
            return;
        }
        break;
    }

    // Read information
    ide_400ns(base_ctrl);
    for(int i=0; i<256; i++){
        ((uint16_t*)ide_buf)[i] = inw(base_io+ATA_REG_DATA);
    }

    printf("ide_detect: ATA device found\r\n");

    ide_devices[drive+2*channel].available = 1;
    ide_devices[drive+2*channel].type = type;
    ide_devices[drive+2*channel].channel = channel;
    ide_devices[drive+2*channel].drive = drive;
    ide_devices[drive+2*channel].signature = *((unsigned short*)(ide_buf+ATA_IDENT_DEVICETYPE));
    ide_devices[drive+2*channel].capabilities = *((unsigned short*)(ide_buf+ATA_IDENT_CAPABILITIES));
    ide_devices[drive+2*channel].commandsets = *((unsigned int*)(ide_buf+ATA_IDENT_COMMANDSETS));

    if(ide_devices[drive+2*channel].commandsets&(1<<26))
        // 48-bit addressing
        ide_devices[drive+2*channel].size = *((unsigned int*)(ide_buf+ATA_IDENT_MAX_LBA_EXT));
    else
        // CHS or 28-bit addressing
        ide_devices[drive+2*channel].size = *((unsigned int*)(ide_buf+ATA_IDENT_MAX_LBA));

    for(int i=0; i<40; i+=2){
        ide_devices[drive+2*channel].model[i] = ide_buf[ATA_IDENT_MODEL+i+1];
        ide_devices[drive+2*channel].model[i+1] = ide_buf[ATA_IDENT_MODEL+i];
    }
    ide_devices[drive+2*channel].model[40] = 0;

    memset(ide_buf, 0, 512);
}

void ide_initialize(unsigned int BAR0, unsigned int BAR1, unsigned int BAR2, unsigned int BAR3, unsigned int BAR4){
    // Disable IRQs:
    outb(ATA_PRIMARY_IO+ATA_REG_CONTROL, 2);
    outb(ATA_SECONDARY_IO+ATA_REG_CONTROL, 2);

    ide_detect(ATA_PRIMARY_IO, ATA_PRIMARY_DCR_AS, ATA_PRIMARY, ATA_MASTER);
    ide_detect(ATA_PRIMARY_IO, ATA_PRIMARY_DCR_AS, ATA_PRIMARY, ATA_SLAVE);
    ide_detect(ATA_SECONDARY_IO, ATA_SECONDARY_DCR_AS, ATA_SECONDARY, ATA_MASTER);
    ide_detect(ATA_SECONDARY_IO, ATA_SECONDARY_DCR_AS, ATA_SECONDARY, ATA_SLAVE);

    printf("ide_initialize: IDE drives:\r\n");
    for(int i=0; i<4; i++){
        if(ide_devices[i].available){
            printf("ide_initialize: Found drive '%s' - %dMB of type %d on %d-%d\r\n", ide_devices[i].model, ide_devices[i].size/1024/2, ide_devices[i].type, ide_devices[i].channel, ide_devices[i].drive);
        }
    }
}