/**
 *	Embedded System Course - University of Toulouse 3
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
#ifndef STM32F4_NVIC_H
#define STM32F4_NVIC_H

#include "io.h"

#define NVIC_BASE		0xE000E000
#define NVIC_REG(o)		_IOREG(NVIC_BASE, o)
#define NVIC_ICTR		NVIC_REG(0x004)
#define NVIC_ISER(n)	NVIC_REG(0x100 + (n)*4)
#define NVIC_ICER(n)	NVIC_REG(0x180 + (n)*4)
#define NVIC_ISPR(n)	NVIC_REG(0x200 + (n)*4)
#define NVIC_ICPR(n)	NVIC_REG(0x280 + (n)*4)
#define NVIC_IABR(n)	NVIC_REG(0x300 + (n)*4)
#define NVIC_IPR(n)		NVIC_REG(0x400 + (n))

#define NVIC_IRQ(n)		*(uint32_t *)(0x40 + (n)*4)

#endif	// STM32F4_RCC_H
