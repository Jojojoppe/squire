OUTPUT_FORMAT ("elf32-littlearm")

MEMORY {
  SRAM_0 : ORIGIN = 0x00000000, LENGTH = 0x00030000  /* 192k */
  SRAM_1 : ORIGIN = 0xFFFF0000, LENGTH = 0x00010000  /*  64k */
  DDR_PS : ORIGIN = 0x00100000, LENGTH = 0x0FF00000  /* 256M */
}

EXTERN(_start);
ENTRY(_start);

SECTIONS {

	.text : {
		__stext = .;
		*(.vtab)          		 /* Vector table */
		*(.text .text.*)         /* Standard Code */
		*(.rodata .rodata.*)     /* Constants, strings, ... */
		*(.glue_7)               /* Glue ARM to thumb code */
		*(.glue_7t)              /* Glue thumb to ARM code */
		__etext = .;
	} >DDR_PS

	.data : {
		. = ALIGN(0x1000);
		__sdata = .;
		*(.data .data.*)
		__edata = .;
	} >DDR_PS

	.bss : {
		. = ALIGN(0x1000);
		__bss_start__ = .;              /* Used for zeroing bss on startup */
		*(.bss .bss.*)
		__bss_end__ = .;
	} >DDR_PS

	/DISCARD/ : {
		*(.comment)
		*(.eh_frame)
		*(.note.gnu.build-id)
	}

	. = ALIGN(0x1000);
	end = .;
 
}
