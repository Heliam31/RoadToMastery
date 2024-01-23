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
#ifndef STM32F4_DAC_H
#define STM32F4_DAC_H

#include "io.h"

#define DAC_BASE	0x40007400
#define DAC_REG(o)	_IOREG(DAC_BASE, o)

#define DAC_CR			DAC_REG(0x00)
#define DAC_NOWAVE		0b00
#define DAC_NOISE		0b01
#define DAC_TRIANGLE	0b10
#define DAC_TSEL_TIM6	0b000
#define DAC_TSEL_TIM8	0b001
#define DAC_TSEL_TIM7	0b010
#define DAC_TSEL_TIM5	0b011
#define DAC_TTSEL_IM2	0b100
#define DAC_TSEL_TIM4	0b101
#define DAC_TSEL_EXTI9	0b110
#define DAC_TSEL_SWTRIG	0b111
#define DAC_TEN2		(1 << 18)
#define DAC_BOFF2		(1 << 17)
#define DAC_EN2			(1 << 16)
#define DAC_TEN1		(1 << 2)
#define DAC_BOFF1		(1 << 1)
#define DAC_EN1			(1 << 0)

#define DAC_SWTRIGR		DAC_REG(0x04)
#define DAC_SWTRIG2		(1 << 1)
#define DAC_SWTRIG1		(1 << 0)

#define DAC_DHR12R1		DAC_REG(0x08)
#define DAC_DHR12L1		DAC_REG(0x0c)
#define DAC_DHR8R1		DAC_REG(0x10)
#define DAC_DHR12R2		DAC_REG(0x14)
#define DAC_DHR12L2		DAC_REG(0x18)
#define DAC_DHR8R2		DAC_REG(0x1C)
#define DAC_DHR12RD		DAC_REG(0x20)
#define DAC_DHR12LD		DAC_REG(0x24)
#define DAC_DHR8RD		DAC_REG(0x28)
#define DAC_DOR1		DAC_REG(0x2C)
#define DAC_DOR2		DAC_REG(0x30)

#define DAC_SR			DAC_REG(0x34)
#define DAC_DMAUDR2		(1 << 29)

#endif	// STM32F4_GPIO_H
