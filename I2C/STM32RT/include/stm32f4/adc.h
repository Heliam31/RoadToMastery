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
#ifndef STM32F4_ADC_H
#define STM32F4_ADC_H

#include "io.h"

#define ADC1_BASE	0x40012000
#define ADC2_BASE	0x40012100
#define ADC3_BASE	0x40012200
#define ADCC_BASE	0x40012300
#define ADC1_REG(o)	_IOREG(ADC1_BASE, o)
#define ADC2_REG(o)	_IOREG(ADC2_BASE, o)
#define ADC3_REG(o)	_IOREG(ADC3_BASE, o)
#define ADCC_REG(o)	_IOREG(ADCC_BASE, o)

#define ADC1_SR		ADC1_REG(0x00)
#define ADC2_SR		ADC2_REG(0x00)
#define ADC3_SR		ADC3_REG(0x00)
#define ADC_JEOC	(1 << 2)
#define ADC_EOC		(1 << 1)

#define ADC1_CR1	ADC1_REG(0x04)
#define ADC2_CR1	ADC2_REG(0x04)
#define ADC3_CR1	ADC3_REG(0x04)
#define ADC_OVERIE	(1 << 26)
#define ADC_12b		0b00
#define ADC_10b		0b01
#define ADC_8b		0b10
#define ADC_6b		0b11
#define ADC_JDISCEN	(1 << 12)
#define ADC_DISCEN	(1 << 11)
#define ADC_SCAN	(1 << 8)
#define ADC_EOCIE	(1 << 5)

#define ADC1_CR2	ADC1_REG(0x08)
#define ADC2_CR2	ADC2_REG(0x08)
#define ADC3_CR2	ADC3_REG(0x08)
#define ADC_SWSTART	(1 << 30)
#define ADC_EXTEN_dis		(0b00 << 28)
#define ADC_EXTEN_rise		(0b01 << 28)
#define ADC_EXTEN_fall		(0b10 << 28)
#define ADC_EXTEN_both		(0b11 << 28)
#define ADC_EXTSEL_T1CC1	(0b0000 << 24)
#define ADC_EXTSEL_T1CC2	(0b0001 << 24)
#define ADC_EXTSEL_T1CC3	(0b0010 << 24)
#define ADC_EXTSEL_T2CC2	(0b0011 << 24)
#define ADC_EXTSEL_T2CC3	(0b0100 << 24)
#define ADC_EXTSEL_T2CC4	(0b0101 << 24)
#define ADC_EXTSEL_T2TRGO	(0b0110 << 24)
#define ADC_EXTSEL_T3CC1	(0b0111 << 24)
#define ADC_EXTSEL_T3TRGO	(0b1000 << 24)
#define ADC_EXTSEL_T4CC4	(0b1001 << 24)
#define ADC_EXTSEL_T5CC1	(0b1010 << 24)
#define ADC_EXTSEL_T5CC2	(0b1011 << 24)
#define ADC_EXTSEL_T5CC3	(0b1100 << 24)
#define ADC_EXTSEL_T8CC1	(0b1101 << 24)
#define ADC_EXTSEL_T8TRGO	(0b1110 << 24)
#define ADC_EXTSEL_EXTI11	(0b1111 << 24)
#define ADC_JWSTART (1 << 22)
#define ADC_EOCS	(1 << 10)
#define ADC_CONT	(1 << 1)
#define ADC_ADON	(1 << 0)

#define ADC1_SMPR1	ADC1_REG(0x0C)
#define ADC2_SMPR1	ADC2_REG(0x0C)
#define ADC3_SMPR1	ADC3_REG(0x0C)

#define ADC1_SMPR2	ADC1_REG(0x10)
#define ADC2_SMPR2	ADC2_REG(0x10)
#define ADC3_SMPR2	ADC3_REG(0x10)

#define ADC1_JOFR(n)	ADC1_REG(0x14 + 4*(n))
#define ADC2_JOFR(n)	ADC2_REG(0x14 + 4*(n))
#define ADC2_JOFR(n)	ADC2_REG(0x14 + 4*(n))

#define ADC1_HTR	ADC1_REG(0x24)
#define ADC2_HTR	ADC2_REG(0x24)
#define ADC3_HTR	ADC3_REG(0x24)

#define ADC1_LTR	ADC1_REG(0x28)
#define ADC2_LTR	ADC2_REG(0x28)
#define ADC3_LTR	ADC3_REG(0x28)

#define ADC1_SQR1	ADC1_REG(0x2C)
#define ADC2_SQR1	ADC2_REG(0x2C)
#define ADC3_SQR1	ADC3_REG(0x2C)

#define ADC1_SQR2	ADC1_REG(0x30)
#define ADC2_SQR2	ADC2_REG(0x30)
#define ADC3_SQR2	ADC3_REG(0x30)

#define ADC1_SQR3	ADC1_REG(0x34)
#define ADC2_SQR3	ADC2_REG(0x34)
#define ADC3_SQR3	ADC3_REG(0x34)

#define ADC1_JSQR	ADC1_REG(0x38)
#define ADC2_JSQR	ADC2_REG(0x38)
#define ADC3_JSQR	ADC3_REG(0x38)

#define ADC1_JDR(n)	ADC1_REG(0x3C + 4*(n))
#define ADC2_JDR(n)	ADC2_REG(0x3C + 4*(n))
#define ADC2_JDR(n)	ADC2_REG(0x3C + 4*(n))

#define ADC1_DR		ADC1_REG(0x4C)
#define ADC2_DR		ADC2_REG(0x4C)
#define ADC2_DR		ADC2_REG(0x4C)

#define ADC_CSR		ADCC_REG(0x00)
#define ADC_CCR		ADCC_REG(0x04)
#define	ADC_DIV2	(0b00 << 16)
#define ADC_DIV4	(0b01 << 16)
#define ADC_DIV6	(0b10 << 16)
#define ADC_DIV8	(0b11 << 16)
#define ADC_CDR		ADCC_REG(0x08)

#define ADC_IRQ		18

#endif	// STM32F4_GPIO_H
