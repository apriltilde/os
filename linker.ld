OUTPUT_FORMAT(elf32-i386)
ENTRY(start)

SECTIONS
{
    . = 0x100000;

    .text : {
        *(.text)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
    }

    /* Place print function at known address */
    . = 0x1000000;
    .print_func :
    {
        KEEP(*(.print_func))
    }

	. = 0x2000000;
	.char_func :
	{
		KEEP(*(.char_func))
	}


}

