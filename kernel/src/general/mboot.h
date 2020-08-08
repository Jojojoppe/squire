#ifndef __MBOOT_H
#define __MBOOT_H 1
/**
 * @file
 * @section DESCRIPTION
 * Multiboot structures
 */

#define MBOOT_MAGIC 0x1badboo2
#define MBOOT_FLAGS 0x07
#define MBOOT_CHECKSUM (-(MBOOT_MAGIC+MBOOT_FLAGS))

typedef struct{
	unsigned int		flags;
	unsigned int	 	memLower;
	unsigned int 		memUpper;
	unsigned int 		bootDevice;
	unsigned int 		commandLine;
	unsigned int 		moduleCount;
	unsigned int		moduleAddress;
	unsigned int 		syms[4];
	unsigned int 		memMapLength;
	unsigned int		memMapAddress;
	unsigned int 		drivesLength;
	unsigned int		drivesAddress;	
	unsigned int 		configTable;
	unsigned int 		apmTable;
	unsigned int 		vbeControlInfo;	
	unsigned int 		vbeModeInfo;
	unsigned int 		vbeMode;
	unsigned short 		vbeInterfaceSeg;
	unsigned short 		vbeInterfaceOff;
	unsigned short 		vbeInterfaceLength;
    unsigned long       framebufferAddress;
    unsigned int        framebufferPitch;
    unsigned int        framebufferWidth;
    unsigned int        framebufferHeight;
    unsigned char       framebufferBpp;
    unsigned char       framebufferType;
    unsigned char       colorInfo[6;]
} __attribute__((packed)) mboot_info_t;

#endif