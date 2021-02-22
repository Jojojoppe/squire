#include "ide.h"
#include <squire.h>
#include <squire_ddm.h>
#include <drivers/pci.h>

void hexDump (const char * desc, const void * addr, const int len) {
    int i;
    unsigned char buff[17];
    const unsigned char * pc = (const unsigned char *)addr;

    // Output description if given.

    if (desc != NULL)
        printf ("%s:\n", desc);

    // Length checks.

    if (len == 0) {
        printf("  ZERO LENGTH\n");
        return;
    }
    else if (len < 0) {
        printf("  NEGATIVE LENGTH: %d\n", len);
        return;
    }

    // Process every byte in the data.

    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Don't print ASCII buffer for the "zeroth" line.

            if (i != 0)
                printf ("  %s\n", buff);

            // Output the offset.

            printf ("  %04x ", i);
        }

        // Now the hex code for the specific character.
        printf (" %02x", pc[i]);

        // And buffer a printable ASCII character for later.

        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) // isprint() may be better.
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.

    while ((i % 16) != 0) {
        printf ("   ");
        i++;
    }

    // And print the final ASCII buffer.

    printf ("  %s\n", buff);
}

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
        inb(base_ctrl+ATA_REG_ALTSTATUS);
}

void ide_detect(unsigned int base_io, unsigned int base_ctrl, unsigned int slave){
    // Reset drive
    outb(base_ctrl, ATA_CMD_RESET);
    ide_400ns(base_ctrl);
    outb(base_ctrl, 0);
    ide_400ns(base_ctrl);

    // Select drive, send 0xa0 to master, 0xb0 to slave
    outb(base_io+ATA_REG_HDDEVSEL, (0xa+slave)<<4);
    // Set sector counts and lbas to 0
    outb(base_io+ATA_REG_SECCOUNT0, 0);
    outb(base_io+ATA_REG_SECCOUNT1, 0);
    outb(base_io+ATA_REG_LBA0, 0);
    outb(base_io+ATA_REG_LBA1, 0);
    outb(base_io+ATA_REG_LBA2, 0);
    outb(base_io+ATA_REG_LBA3, 0);
    outb(base_io+ATA_REG_LBA4, 0);
    outb(base_io+ATA_REG_LBA5, 0);

    // Send identify
    outb(base_io+ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
    ide_400ns(base_ctrl);
    if(!inb(base_io+ATA_REG_STATUS)){
        printf("ide_detect: not existing\r\n");
        return;
    }

    unsigned char lba1 = inb(base_io+ATA_REG_LBA1);
    unsigned char lba2 = inb(base_io+ATA_REG_LBA2);
    if(lba1!=0 || lba2 !=0){
        printf("ide_detect: device not ATA\r\n");
        return;
    }
    printf("ide_detect: ATA device found\r\n");
}

void ide_initialize(unsigned int BAR0, unsigned int BAR1, unsigned int BAR2, unsigned int BAR3, unsigned int BAR4){
    // Disable IRQs:
    outb(ATA_PRIMARY_IO+ATA_REG_CONTROL, 2);
    outb(ATA_SECONDARY_IO+ATA_REG_CONTROL, 2);

    ide_detect(ATA_PRIMARY_IO, ATA_PRIMARY_DCR_AS, ATA_MASTER);
    ide_detect(ATA_PRIMARY_IO, ATA_PRIMARY_DCR_AS, ATA_SLAVE);
    ide_detect(ATA_SECONDARY_IO, ATA_SECONDARY_DCR_AS, ATA_MASTER);
    ide_detect(ATA_SECONDARY_IO, ATA_SECONDARY_DCR_AS, ATA_SLAVE);
}