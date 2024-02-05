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
#ifndef STM32F4_EXTI_H
#define STM32F4_EXTI_H

#include "io.h"

// registers
#define EXTI_BASE		0x40013C00
#define EXTI_REG(o)		_IOREG(EXTI_BASE, o)
#define EXTI_IMR		EXTI_REG(0x00)
#define EXTI_EMR		EXTI_REG(0x04)
#define EXTI_RTSR		EXTI_REG(0x08)
#define EXTI_FTSR		EXTI_REG(0x0C)
#define EXTI_SWIER		EXTI_REG(0x10)
#define EXTI_PR			EXTI_REG(0x14)

// interrupts
#define EXTI0_IRQ		6
#define EXTI1_IRQ		7
#define EXTI2_IRQ		8
#define EXTI3_IRQ		9
#define EXTI4_IRQ		10
#define EXTI9_5_IRQ		23
#define EXTI15_10_IRQ	40

#endif	// STM32F4_EXTI_H
