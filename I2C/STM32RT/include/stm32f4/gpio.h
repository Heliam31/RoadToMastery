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
#ifndef STM32F4_GPIO_H
#define STM32F4_GPIO_H

#include "io.h"

#define GPIO_INST(n)	((n) >> 4)		
#define GPIO_NUM(n)		((n) & 0b00001111)
#define GPIO_HNUM(n)	((n) & 0b00000111)
#define GPIO_HOFF(n)	((GPIO_NUM(n) >> 3) * 4)

#define GPIO_START		0x40020000
#define GPIO_SIZE		0x400
#define GPIO_BASE(n)	(GPIO_START + (n) * GPIO_SIZE)

#define GPIO_MODER_IN	0b00
#define GPIO_MODER_OUT	0b01
#define GPIO_MODER_ALT	0b10
#define GPIO_MODER_ANA	0b11

#define GPIO_OSPEEDR_LO	0b00
#define GPIO_OSPEEDR_ME	0b01
#define GPIO_OSPEEDR_HI	0b10
#define GPIO_OSPEEDR_VH	0b11

#define GPIO_PUPDR_NO	0b00
#define GPIO_PUPDR_PU	0b01
#define GPIO_PUPDR_PD	0b10

#define GPIO_REG(n, o)	_IOREG(GPIO_BASE(n), o)
#define GPIO_MODER(n)	_IOREG(GPIO_BASE(n), 0x00)
#define GPIO_OTYPER(n)	_IOREG(GPIO_BASE(n), 0x04)
#define GPIO_OSPEEDR(n)	_IOREG(GPIO_BASE(n), 0x08)
#define GPIO_OSPEEDR(n)	_IOREG(GPIO_BASE(n), 0x08)
#define GPIO_PUPDR(n)	_IOREG(GPIO_BASE(n), 0x0c)
#define GPIO_IDR(n)		_IOREG(GPIO_BASE(n), 0x10)
#define GPIO_ODR(n)		_IOREG(GPIO_BASE(n), 0x14)
#define GPIO_BSRR(n)	_IOREG(GPIO_BASE(n), 0x18)
#define GPIO_LCKR(n)	_IOREG(GPIO_BASE(n), 0x1c)
#define GPIO_AFRL(n)	_IOREG(GPIO_BASE(n), 0x20)
#define GPIO_AFRH(n)	_IOREG(GPIO_BASE(n), 0x24)

#define GPIOA_MODER		_IOREG(GPIO_BASE(0), 0x00)
#define GPIOA_OTYPER	_IOREG(GPIO_BASE(0), 0x04)
#define GPIOA_OSPEEDR	_IOREG(GPIO_BASE(0), 0x08)
#define GPIOA_OSPEEDR	_IOREG(GPIO_BASE(0), 0x08)
#define GPIOA_PUPDR		_IOREG(GPIO_BASE(0), 0x0c)
#define GPIOA_IDR		_IOREG(GPIO_BASE(0), 0x10)
#define GPIOA_ODR		_IOREG(GPIO_BASE(0), 0x14)
#define GPIOA_BSRR		_IOREG(GPIO_BASE(0), 0x18)
#define GPIOA_LCKR		_IOREG(GPIO_BASE(0), 0x1c)
#define GPIOA_AFRL		_IOREG(GPIO_BASE(0), 0x20)
#define GPIOA_AFRH		_IOREG(GPIO_BASE(0), 0x24)

#define GPIOB_MODER		_IOREG(GPIO_BASE(1), 0x00)
#define GPIOB_OTYPER	_IOREG(GPIO_BASE(1), 0x04)
#define GPIOB_OSPEEDR	_IOREG(GPIO_BASE(1), 0x08)
#define GPIOB_OSPEEDR	_IOREG(GPIO_BASE(1), 0x08)
#define GPIOB_PUPDR		_IOREG(GPIO_BASE(1), 0x0c)
#define GPIOB_IDR		_IOREG(GPIO_BASE(1), 0x10)
#define GPIOB_ODR		_IOREG(GPIO_BASE(1), 0x14)
#define GPIOB_BSRR		_IOREG(GPIO_BASE(1), 0x18)
#define GPIOB_LCKR		_IOREG(GPIO_BASE(1), 0x1c)
#define GPIOB_AFRL		_IOREG(GPIO_BASE(1), 0x20)
#define GPIOB_AFRH		_IOREG(GPIO_BASE(1), 0x24)

#define GPIOC_MODER		_IOREG(GPIO_BASE(2), 0x00)
#define GPIOC_OTYPER	_IOREG(GPIO_BASE(2), 0x04)
#define GPIOC_OSPEEDR	_IOREG(GPIO_BASE(2), 0x08)
#define GPIOC_OSPEEDR	_IOREG(GPIO_BASE(2), 0x08)
#define GPIOC_PUPDR		_IOREG(GPIO_BASE(2), 0x0c)
#define GPIOC_IDR		_IOREG(GPIO_BASE(2), 0x10)
#define GPIOC_ODR		_IOREG(GPIO_BASE(2), 0x14)
#define GPIOC_BSRR		_IOREG(GPIO_BASE(2), 0x18)
#define GPIOC_LCKR		_IOREG(GPIO_BASE(2), 0x1c)
#define GPIOC_AFRL		_IOREG(GPIO_BASE(2), 0x20)
#define GPIOC_AFRH		_IOREG(GPIO_BASE(2), 0x24)

#define GPIOD_MODER		_IOREG(GPIO_BASE(3), 0x00)
#define GPIOD_OTYPER	_IOREG(GPIO_BASE(3), 0x04)
#define GPIOD_OSPEEDR	_IOREG(GPIO_BASE(3), 0x08)
#define GPIOD_OSPEEDR	_IOREG(GPIO_BASE(3), 0x08)
#define GPIOD_PUPDR		_IOREG(GPIO_BASE(3), 0x0c)
#define GPIOD_IDR		_IOREG(GPIO_BASE(3), 0x10)
#define GPIOD_ODR		_IOREG(GPIO_BASE(3), 0x14)
#define GPIOD_BSRR		_IOREG(GPIO_BASE(3), 0x18)
#define GPIOD_LCKR		_IOREG(GPIO_BASE(3), 0x1c)
#define GPIOD_AFRL		_IOREG(GPIO_BASE(3), 0x20)
#define GPIOD_AFRH		_IOREG(GPIO_BASE(3), 0x24)


/*
#define GPIO_MODER_SET(n, m)	_SET(GPIO_MODER(GPIO_INST(n)), GPIO_NUM(n)*2+1, GPIO_NUM(n)*2, m)
#define GPIO_OSPEEDR_SET(n, m)	_SET(GPIO_OSPEEDR(GPIO_INST(n)), GPIO_NUM(n)*2+1, GPIO_NUM(n)*2, m)
#define GPIO_PUPDR_SET(n, m)	_SET(GPIO_PUPDR(GPIO_INST(n)), GPIO_NUM(n)*2+1, GPIO_NUM(n)*2, m)
#define GPIO_BSRR_SET(n)		GPIO_BSRR(GPIO_INST(n)) = 1 << GPIO_NUM(n)
#define GPIO_BSRR_RESET(n)		GPIO_BSRR(GPIO_INST(n)) = 1 << (GPIO_NUM(n) + 16)
#define GPIO_AFR(n, a)			_SET(GPIO_REG(GPIO_INST(n), 0x20 + GPIO_HOFF(n)), GPIO_NUM(n)*4+3, GPIO_NUM(n)*4, a)

#define GPIOA(n)	(0x00|(n))
#define GPIOB(n)	(0x10|(n))
#define GPIOC(n)	(0x20|(n))
#define GPIOD(n)	(0x30|(n))
#define GPIOE(n)	(0x40|(n))
#define GPIOF(n)	(0x50|(n))
#define GPIOG(n)	(0x60|(n))
#define GPIOH(n)	(0x70|(n))
#define GPIOI(n)	(0x80|(n))
*/

#endif	// STM32F4_GPIO_H
