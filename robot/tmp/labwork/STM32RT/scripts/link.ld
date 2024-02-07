MEMORY {
	RAM 	(xrw)	: ORIGIN = 0x20000000, LENGTH = 128K
	FLASH 	(rx)	: ORIGIN = 0x08000000, LENGTH = 1024K
}


ENTRY(handler_reset)

SECTIONS {

	.text :
	{
		_vectab_begin = .;
		KEEP(*(.vector_table))
		_vectab_end = .;
		*(.text)
		*(.text*)
		*(.rodata)
		*(.rodata*)
		. = ALIGN(4);
	} > FLASH

	_data_flash = .;
	.data : AT(_data_flash)
	{
		_vectab_in_ram = .;
		*(.vector_table);
		_data_begin = .;
		*(.init)
		*(.init*)
		*(.fini)
		*(.fini*)
		*(.data)
		*(.data*)
		. = ALIGN(4);
		_data_end = .;
	} > RAM

	.bss :
	{
		_bss_begin = .;
		*(.bss)
		*(.bss*)
		. = ALIGN(4);
		_bss_end = .;
	} > RAM

	_stack_size = 1024;
	_stack_end = ORIGIN(RAM) + LENGTH(RAM);
	_stack_begin = _stack_end - _stack_size;
	. = _stack_begin;
	.stack :
	{
		. = . + _stack_size;
	} > RAM
}

