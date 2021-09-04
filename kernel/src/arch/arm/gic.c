#include "gic.h"
#include "cpu.h"
#include "../../common/arch/vas.h"

#define POINTER_TO_REGISTER(REG)	( *((volatile unsigned int*)(REG)))
#define POINTER_TO_REG_ARRAY(REG)	((volatile unsigned int*)(REG))

#define PERIPH_BASE					cpu_get_periphbase()
#define PERIPH_VIRT_OFFSET			0
#define PERIPH_VIRT_BASE			(PERIPH_BASE-PERIPH_VIRT_OFFSET)

// Interrupt interface to CPU
#define ICCICR						POINTER_TO_REGISTER(PERIPH_VIRT_BASE+0x0100)
#define ICCPMR						POINTER_TO_REGISTER(PERIPH_VIRT_BASE+0x0104)
#define ICCBPR						POINTER_TO_REGISTER(PERIPH_VIRT_BASE+0x0108)
#define ICCIAR						POINTER_TO_REGISTER(PERIPH_VIRT_BASE+0x010c)
#define ICCEOIR						POINTER_TO_REGISTER(PERIPH_VIRT_BASE+0x0110)
#define ICCRPR						POINTER_TO_REGISTER(PERIPH_VIRT_BASE+0x0114)
#define ICCHPIR						POINTER_TO_REGISTER(PERIPH_VIRT_BASE+0x0118)
#define ICCABPR						POINTER_TO_REGISTER(PERIPH_VIRT_BASE+0x011c)
#define ICCIDR						POINTER_TO_REGISTER(PERIPH_VIRT_BASE+0x01fc)

// Interrupt disruptor
#define ICDDCR						POINTER_TO_REGISTER(PERIPH_VIRT_BASE+0x1000)
#define ICDICTR						POINTER_TO_REGISTER(PERIPH_VIRT_BASE+0x1004)
#define ICDIIDR						POINTER_TO_REGISTER(PERIPH_VIRT_BASE+0x1008)
#define ICDISRn						POINTER_TO_REG_ARRAY(PERIPH_VIRT_BASE+0x1080)
#define ICDISERn					POINTER_TO_REG_ARRAY(PERIPH_VIRT_BASE+0x1100)
#define ICDICERn					POINTER_TO_REG_ARRAY(PERIPH_VIRT_BASE+0x1180)
#define ICDISPRn					POINTER_TO_REG_ARRAY(PERIPH_VIRT_BASE+0x1200)
#define ICDICPRn					POINTER_TO_REG_ARRAY(PERIPH_VIRT_BASE+0x1280)
#define ICDABRn						POINTER_TO_REG_ARRAY(PERIPH_VIRT_BASE+0x1300)
#define ICDIPRn						POINTER_TO_REG_ARRAY(PERIPH_VIRT_BASE+0x1400)
#define ICDIPTRn					POINTER_TO_REG_ARRAY(PERIPH_VIRT_BASE+0x1800)
#define ICDICFRn					POINTER_TO_REG_ARRAY(PERIPH_VIRT_BASE+0x1c00)
#define ppi_status					POINTER_TO_REGISTER(PERIPH_VIRT_BASE+0x1d00)
#define spi_status0					POINTER_TO_REGISTER(PERIPH_VIRT_BASE+0x1d04)
#define spi_status1					POINTER_TO_REGISTER(PERIPH_VIRT_BASE+0x1d08)
#define ICDSGIR						POINTER_TO_REGISTER(PERIPH_VIRT_BASE+0x1f00)

void gic_init(){
	// Map peripherals to vspace
	arch_vas_map(PERIPH_BASE, PERIPH_VIRT_BASE, VAS_FLAGS_KREAD|VAS_FLAGS_KWRITE);
	arch_vas_map(PERIPH_BASE+0x1000, PERIPH_VIRT_BASE+0x1000, VAS_FLAGS_KREAD|VAS_FLAGS_KWRITE);

	// Disable interrupt disruptor
	ICDDCR = 0;
	// Enable interrupt CPU interfaces
	ICCICR = 0x3;		// non-secure and secure enabled, using IRQ
	// Set priority mask to lowest
	ICCPMR = 0xffffffff;
	// Enable disruptor
	ICDDCR = 0x3;		// non-secure and secure mode enabled
}

void gic_enable_interrupt(unsigned char number){
	unsigned char reg = number/32;
	unsigned char bit = number%32;
	ICDISRn[reg] |= (1<<bit);
}

void gic_disable_interrupt(unsigned char number){
	unsigned char reg = number/32;
	unsigned char bit = number%32;
	ICDICERn[reg] &= ~(1<<bit);
}

void gic_set_priority(unsigned char number, unsigned char priority){
	unsigned char reg = number/4;
	unsigned char bit = (number%4)*8;
	ICDIPRn[reg] &= ~(0xff<<bit);
	ICDIPRn[reg] |= priority<<bit;
}

unsigned int gic_ack_interrupt(){
	return ICCIAR;
}

void gic_end_interrupt(unsigned char number){
	ICCEOIR = number;
}

void gic_sgi(unsigned char target, unsigned char cpu, unsigned char irq){
	ICDSGIR = (target&0x3)<<24 | (cpu&0xf)<<16 | 1<<15 | (irq&0xf);
}
