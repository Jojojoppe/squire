#include <x86_generic_RTC.h>

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <squire.h>

// RTC time data
uint8_t rtc_seconds, rtc_minutes, rtc_hours, rtc_weekday, rtc_dayofmonth;
uint8_t rtc_month, rtc_year, rtc_century;

uint8_t bcd_to_bin(uint8_t bcd){
	return ((bcd/16)*10)+(bcd&0xf);
}

uint8_t rtc_read(uint8_t reg){
	squire_io_port_outb(RTC_IO_PORT_INDEX, reg);
	uint8_t val;
	squire_io_port_inb(RTC_IO_PORT_DATA, &val);
	return val;
}

void rtc_read_datetime(){
	rtc_seconds = rtc_read(0x00);
	rtc_minutes = rtc_read(0x02);
	rtc_hours = rtc_read(0x04);
	rtc_weekday = rtc_read(0x06);
	rtc_dayofmonth = rtc_read(0x07);
	rtc_month = rtc_read(0x08);
	rtc_year = rtc_read(0x09);
	rtc_century = rtc_read(0x32);
}

void x86_generic_RTC_INTR(){
	// Must read index C to clear interrupts
	squire_io_port_outb(RTC_IO_PORT_INDEX, 0x0c);	// Index C
	uint8_t valC;
	squire_io_port_inb(RTC_IO_PORT_DATA, &valC);

	// Read date and time
	rtc_read_datetime();
	// printf("RTC] %d:%d:%d\r\n", bcd_to_bin(rtc_hours), bcd_to_bin(rtc_minutes), bcd_to_bin(rtc_seconds));

	// if((bcd_to_bin(rtc_seconds)&0xf)==0){
		// printf("EXIT!!!\r\n");
		// exit(1234);
	// }
}

void x86_generic_RTC_function_callback(unsigned int from, squire_driver_submessage_function_t * func){
	switch(func->function){
	
		case DRIVER_FUNCTIONS_INIT:{
			printf("x86_generic_RTC %08x] Initialize device\r\n", func->instance);
			
			// Register RTC interrupt
			squire_io_register_isr(RTC_INTR);
			// Register IO port usage
			squire_io_register_port(RTC_IO_PORT_INDEX, 2, IO_PORT_READ|IO_PORT_WRITE);

			// Enable RTC
			// Enable update ended interrupt (1Hz)
			squire_io_port_outb(RTC_IO_PORT_INDEX, 0x8b);	// Index B, disable NMI
			uint8_t valB;
			squire_io_port_inb(RTC_IO_PORT_DATA, &valB);
			squire_io_port_outb(RTC_IO_PORT_INDEX, 0x8b);	// Index B, disable NMI
			squire_io_port_outb(RTC_IO_PORT_DATA, valB | 0x10);
			// Read Index C to clear interrupts
			squire_io_port_outb(RTC_IO_PORT_INDEX, 0x0c);	// Index C
			uint8_t valC;
			squire_io_port_inb(RTC_IO_PORT_DATA, &valC);
		
			// Read date and time for the first time
			rtc_read_datetime();
			printf("RTC] %d:%d:%d\r\n", bcd_to_bin(rtc_hours), bcd_to_bin(rtc_minutes), bcd_to_bin(rtc_seconds));

		} break;

		default:
			printf("x86_generic_RTC %08x] Unknown function requested\r\n", func->instance);
	}
}

#include <squire.h>
