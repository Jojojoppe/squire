ENTRY(_start)

SECTIONS
{
	. = 0x00400000;

	.text : AT(ADDR(.text)){
		g_code = .;
		*(.text)
		*(.rodata)
		. = ALIGN(0x1000);
	}

	.data : AT(ADDR(.data)){
		g_data = .;
		*(.data)
		. = ALIGN(0x1000);
	}

	.bss : AT(ADDR(.bss)){
		g_bss = .;
		*(.bss)
		*(COMMON)
		. = ALIGN(0x1000);
	}

	g_end = .;

	/DISCARD/ :{
		*(.comment)
		*(.eh_frame)
		*(.note.gnu.build-id)
	}
}
