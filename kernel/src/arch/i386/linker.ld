EXTERN(_start);
ENTRY(_start);

SECTIONS {

	. = 0x100000;

	.text : {
		__stext = .;
		*(.mboot)
		*(.text .text.*)
		*(.rodata .rodata.*)
		__etext = .;
	}

	.data : {
		. = ALIGN(0x1000);
		__sdata = .;
		*(.data .data.*)
		__edata = .;
	}

	.bss : {
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
	end = .;
 
}
