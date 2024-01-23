/**
 * Torpy - grid-follower wheeling and box delivering bot.
 *	Copyright (C) 2021  Université de Toulouse <casse@irit.fr>
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 */
#ifndef STM32F4_DBG_H
#define STM32F4_DBG_H

#include "io.h"

// any documentation any where?
#define DBG_DEMCR        		*((volatile uint32_t*) 0xE000EDFCU )
#define DBG_DEMCR_TRCENA		( 1 << 24)

#define ITM_STIMULUS_PORT0   	*((volatile uint32_t*) 0xE0000000 )
#define ITM_TRACE_EN          	*((volatile uint32_t*) 0xE0000E00 )
#define ITM_TRACE_EN_PORT0		(1 << 0)

void ITM_putc(uint8_t ch);
void ITM_puts(const char *s);

#endif	// STM32F4_IO_H
