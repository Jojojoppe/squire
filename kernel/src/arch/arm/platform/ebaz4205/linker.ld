OUTPUT_FORMAT ("elf32-littlearm")

MEMORY {
  SRAM_0 : ORIGIN = 0x00000000, LENGTH = 0x00030000  /* 192k */
  SRAM_1 : ORIGIN = 0xFFFF0000, LENGTH = 0x00010000  /*  64k */
  DDR_PS : ORIGIN = 0x00100000, LENGTH = 0x0FF00000  /* 256M */
}

EXTERN(_start);
ENTRY(_start);

SECTIONS {

	. = 0x00100000 + 0xc0000000;
	__phys_start = . - 0xc0000000;

	.text : AT(ADDR(.text)-0xc0000000){
		__stext = .;
		*(.vtab)          		 /* Vector table */
		*(.text .text.*)         /* Standard Code */
		*(.rodata .rodata.*)     /* Constants, strings, ... */
		*(.glue_7)               /* Glue ARM to thumb code */
		*(.glue_7t)              /* Glue thumb to ARM code */
		__etext = .;
	}

	.data : AT(ADDR(.data)-0xc0000000){
		. = ALIGN(0x1000);
		__sdata = .;
		*(.data .data.*)
		__edata = .;
	}

	.bss : AT(ADDR(.data)-0xc0000000){
		. = ALIGN(0x4000);		  /* Page directory MUST be 0x4000 bytes aligned */
		__bss_start__ = .;
		*(.bss.bootpd)
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
