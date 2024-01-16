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
#ifndef STM32F4_SYSCFG_H
#define STM32F4_SYSCFG_H

#include "io.h"

// registers
#define SYSCFG_BASE		0x40013800
#define SYSCFG_REG(o)	_IOREG(SYSCFG_BASE, o)
#define SYSCFG_MEMRMP	SYSCFG_REG(0x00)
#define SYSCFG_PMC		SYSCFG_REG(0x04)
#define SYSCFG_EXTICR1	SYSCFG_REG(0x08)
#define SYSCFG_EXTICR2	SYSCFG_REG(0x0C)
#define SYSCFG_EXTICR3	SYSCFG_REG(0x10)
#define SYSCFG_EXTICR4	SYSCFG_REG(0x14)
#define SYSCFG_CMPCR	SYSCFG_REG(0x20)

#endif	// STM32F4_SYSCFG_H
