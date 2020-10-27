ENTRY(g_start)

phys = 0x100000;
offset = 0xc0000000;
virt = offset + phys;

SECTIONS
{
	. = virt;
	ld_kernel_start = .;

	.mboot : AT(ADDR(.mboot)-offset){
		*(.mboot)
	}

	.text : AT(ADDR(.text)-offset){
		g_code = .;
		*(.text)
		*(.rodata)
		. = ALIGN(0x1000);
	}

	.data : AT(ADDR(.data)-offset){
		g_data = .;
		*(.data)
		. = ALIGN(0x1000);
	}

	.bss : AT(ADDR(.bss)-offset){
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

	. = ALIGN(0x1000);
	ld_kernel_end = .;
}
