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
#ifndef STM32F4_IO_H
#define STM32F4_IO_H

#include <stdint.h>

#define _IOREG(b, o)		(*(volatile uint32_t *)(b + o))
#define _SIZE(u, l)			((u) - (l) + 1)
#define _MASK(u, l)			(((1 << _SIZE(u, l)) - 1) << (l))
#define _SET(x, u, l, v)	{ uint32_t xx = (x); xx &= ~_MASK(u, l); xx |= (v) << l; (x) = xx; }
#define _GET(x, u, l)		(((x) & _MASK(u, l)) >> (l))

#define MHZ			* 1000000
#define SYSCLK		(168 MHZ)
#define AHB_CLK		(168 MHZ)
#define APB1_CLK	(42 MHZ)
#define APB2_CLK	(84 MHZ)

// AHB1
#define ID_GPIOA	0
#define ID_GPIOB	1
#define ID_GPIOC	2
#define ID_GPIOD	3
#define ID_GPIOE	4
#define ID_GPIOF	5
#define ID_GPIOG	6
#define ID_GPIOH	7
#define ID_GPIOI	8

// APB1
#define ID_TIM2		0
#define ID_TIM3		1
#define ID_TIM4		2
#define ID_TIM5		3
#define ID_TIM6		4
#define ID_TIM7		5
#define ID_TIM12	6
#define ID_TIM13	7
#define ID_TIM14	8

// APB2
#define ID_TIM1		0
#define ID_TIM8		1
#define ID_ADC1		8
#define ID_ADC2		9
#define ID_ADC3		10

// LEDS and buttons
#define LED_GREEN	0x3c
#define LED_ORANGE	0x3d
#define LED_RED		0x3e
#define LED_BLUE	0x3f
#define SW_USER		0x00

// bit range macros
#define MASK(l)	((1 << (l))-1)
#define GET_BITS(x, i, l)		(((x)>>(i))&MASK(l))
#define REP_BITS(x, i, l, y)	(((x)&~(MASK(l)<<i))|((y)<<(i)))

// interrupt 
#define ENABLE_IRQS		__asm("CPSIE I")
#define DISABLE_IRQS	__asm("CPSID I")
#define NOP				__asm__("nop")

#endif	// STM32F4_IO_H
