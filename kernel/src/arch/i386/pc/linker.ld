EXTERN(_start);
ENTRY(_start);

SECTIONS {

	. = 0x100000 + 0xc0000000;
	__phys_start = . - 0xc0000000;

	.text : AT(ADDR(.text)-0xc0000000){
		__stext = .;
		*(.mboot)
		*(.text .text.*)
		*(.rodata .rodata.*)
		__etext = .;
	}

	.data : AT(ADDR(.data)-0xc0000000){
		. = ALIGN(0x1000);
		__sdata = .;
		*(.data .data.*)
		__edata = .;
	}

	.bss : AT(ADDR(.bss)-0xc0000000){
		. = ALIGN(0x1000);
		__bss_start__ = .;
		*(.bss .bss.*)
		__bss_end__ = .;
	}

	/DISCARD/ : {
		*(.comment)
		*(.eh_frame)
		*(.note.gnu.build-id)
	}

	. = ALIGN(0x1000);
	__end = .;
	__phys_end = . - 0xc0000000;
 
}
