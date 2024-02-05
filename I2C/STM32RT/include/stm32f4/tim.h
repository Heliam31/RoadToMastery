/**
 *	Torpy - grid-follower wheeling and box delivering bot.
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
#ifndef STM32F4_TIM_H
#define STM32F4_TIM_H

#include "io.h"

// bases
#define TIM2_BASE	0x40000000
#define TIM3_BASE	0x40000400
#define TIM4_BASE	0x40000800
#define TIM5_BASE	0x40000C00
#define TIM6_BASE	0x40001000
#define TIM7_BASE	0x40001400
#define TIM12_BASE	0x40001800
#define TIM13_BASE	0x40001C00
#define TIM14_BASE	0x40002000

#define TIM1_BASE	0x40010000
#define TIM8_BASE	0x40010400
#define TIM9_BASE	0x40014000
#define TIM10_BASE	0x40014400
#define TIM11_BASE	0x40014800

// registers
#define TIM_IOREG(n, o)	_IOREG(TIM##n##_BASE, o)
#define TIM_CR1(n)		TIM_IOREG(n, 0x00)
#define TIM_CR2(n)		TIM_IOREG(n, 0x04)
#define TIM_SMCR(n)		TIM_IOREG(n, 0x08)
#define TIM_DIER(n)		TIM_IOREG(n, 0x0C)
#define TIM_SR(n)		TIM_IOREG(n, 0x10)
#define TIM_EGR(n)		TIM_IOREG(n, 0x14)
#define TIM_CCMR1(n)	TIM_IOREG(n, 0x18)
#define TIM_CCMR2(n)	TIM_IOREG(n, 0x1C)
#define TIM_CCER(n)		TIM_IOREG(n, 0x20)
#define TIM_CNT(n)		TIM_IOREG(n, 0x24)
#define TIM_PSC(n)		TIM_IOREG(n, 0x28)
#define TIM_ARR(n)		TIM_IOREG(n, 0x2C)
#define TIM_CCR1(n)		TIM_IOREG(n, 0x34)
#define TIM_CCR2(n)		TIM_IOREG(n, 0x38)
#define TIM_CCR3(n)		TIM_IOREG(n, 0x3C)
#define TIM_CCR4(n)		TIM_IOREG(n, 0x40)
#define TIM_DCR(n)		TIM_IOREG(n, 0x48)
#define TIM_DMAR(n)		TIM_IOREG(n, 0x4C)
#define TIM_OR(n)		TIM_IOREG(n, 0x50)

// TIM_CR1
#define TIM_CKD_CKINT	(0b00 << 8)
#define TIM_CKD_CKINT2	(0b01 << 8)
#define TIM_CKD_CKINT4	(0b10 << 8)
#define TIM_ARPE		(1 << 7)
#define TIM_CMS_MODE0	(0b00 << 5)
#define TIM_CMS_MODE1	(0b01 << 5)
#define TIM_CMS_MODE2	(0b10 << 5)
#define TIM_CMS_MODE3	(0b11 << 5)
#define TIM_DIR_DOWN	(1 << 4)
#define TIM_OPM			(1 << 3)
#define TIM_URS			(1 << 2)
#define TIM_UDIS		(1 << 1)
#define TIM_CEN			(1 << 0)

// TIM_CR2
#define TIM_TI1S				(1 << 7)
#define TIM_MMS_RESET			(0b000 << 4)
#define TIM_MMS_ENABLE			(0b001 << 4)
#define TIM_MMS_UPDATE			(0b010 << 4)
#define TIM_MMS_COMPARE_PULSE	(0b011 << 4)
#define TIM_MMS_COMPARE1		(0b100 << 4)
#define TIM_MMS_COMPARE2		(0b101 << 4)
#define TIM_MMS_COMPARE3		(0b110 << 4)
#define TIM_MMS_COMPARE4		(0b111 << 4)
#define TIM_CCDSS				(1 << 3)

// TIM_SMCR
#define TIM_ETP			(1 << 15)
#define TIM_ECE			(1 << 14)
#define TIM_ETPS_OFF	(0b00 << 12)
#define TIM_ETPS_ON2	(0b01 << 12)
#define TIM_ETPS_ON4	(0b10 << 12)
#define TIM_ETPS_ON8	(0b11 << 12)
#define TIM_ETF_CKINT1	(0b0000 << 8)
#define TIM_ETF_CKINT2	(0b0001 << 8)
#define TIM_ETF_CKINT4	(0b0010 << 8)
#define TIM_ETF_CKINT8	(0b0011 << 8)
#define TIM_ETF_DTS2_6	(0b0100 << 8)
#define TIM_ETF_DTS2_8	(0b0101 << 8)
#define TIM_ETF_DTS4_6	(0b0110 << 8)
#define TIM_ETF_DTS4_8	(0b0111 << 8)
#define TIM_ETF_DTS8_6	(0b1000 << 8)
#define TIM_ETF_DTS8_8	(0b1001 << 8)
#define TIM_ETF_DTS16_5	(0b1010 << 8)
#define TIM_ETF_DTS16_6	(0b1011 << 8)
#define TIM_ETF_DTS16_8	(0b1100 << 8)
#define TIM_ETF_DTS32_5	(0b1101 << 8)
#define TIM_ETF_DTS32_6	(0b1110 << 8)
#define TIM_ETF_DTS32_8	(0b1111 << 8)
#define TIM_MSM			(1 << 7)
#define TIM_TS_ITR0		(0b000 << 4)
#define TIM_TS_ITR1		(0b001 << 4)
#define TIM_TS_ITR2		(0b010 << 4)
#define TIM_TS_ITR3		(0b011 << 4)
#define TIM_TS_TI1F_ED	(0b100 << 4)
#define TIM_TS_TI1FP1	(0b101 << 4)
#define TIM_TS_TI2FP2	(0b110 << 4)
#define TIM_TS_ETRF		(0b111 << 4)
#define TIM_SMS_OFF		(0b000 << 0)
#define TIM_SMS_ENC1	(0b001 << 0)
#define TIM_SMS_ENC2	(0b010 << 0)
#define TIM_SMS_ENC3	(0b011 << 0)
#define TIM_SMS_RESET	(0b100 << 0)
#define TIM_SMS_GATED	(0b101 << 0)
#define TIM_SMS_TRIGGER	(0b110 << 0)
#define TIM_SMS_EXT		(0b111 << 0)

// TIM_DIER
#define TIM_TDE		(1 << 14)
#define TIM_CC4DE	(1 << 13)
#define TIM_CC3DE	(1 << 12)
#define TIM_CC2DE	(1 << 11)
#define TIM_CC1DE	(1 << 10)
#define TIM_UDE		(1 << 9)
#define TIM_TIE		(1 << 6)
#define TIM_CC4IE	(1 << 4)
#define TIM_CC3IE	(1 << 3)
#define TIM_CC2IE	(1 << 2)
#define TIM_CC1IE	(1 << 1)
#define TIM_UIE		(1 << 0)

// TIM_SR
#define TIM_CC4OF	(1 << 12)
#define TIM_CC3OF	(1 << 11)
#define TIM_CC2OF	(1 << 10)
#define TIM_CC1OF	(1 << 9)
#define TIM_TIF		(1 << 6)
#define TIM_CC4IF	(1 << 4)
#define TIM_CC3IF	(1 << 3)
#define TIM_CC2IF	(1 << 2)
#define TIM_CC1IF	(1 << 1)
#define TIM_UIF		(1 << 0)

// TIM_EGR
#define TIM_CC4G	(1 << 4)
#define TIM_CC3G	(1 << 3)
#define TIM_CC2G	(1 << 2)
#define TIM_CC1G	(1 << 1)
#define TIM_UG		(1 << 0)

// CCMR1
#define TIM_OC2CE		(1 << 15)
#define TIM_OC2M_FROZEN	(0b000 << 12)
#define TIM_OC2M_SET	(0b001 << 12)
#define TIM_OC2M_CLR	(0b010 << 12)
#define TIM_OC2M_TOGGLE	(0b011 << 12)
#define TIM_OC2M_PWM1	(0b110 << 12)
#define TIM_OC2M_PWM2	(0b111 << 12)
#define TIM_OC2PE		(1 << 11)
#define TIM_OC2FE		(1 << 10)
#define TIM_CCS2S_OUT	(0b00 << 8)
#define TIM_CCS2S_IN2	(0b01 << 8)
#define TIM_CCS2S_IN1	(0b10 << 8)
#define TIM_CCS2S_TRC	(0b11 << 8)
#define TIM_OC1CE		(1 << 7)
#define TIM_OC1M_FROZEN	(0b000 << 4)
#define TIM_OC1M_SET	(0b001 << 4)
#define TIM_OC1M_CLR	(0b010 << 4)
#define TIM_OC1M_TOGGLE	(0b011 << 4)
#define TIM_OC1M_PWM1	(0b110 << 4)
#define TIM_OC1M_PWM2	(0b111 << 4)
#define TIM_OC1PE		(1 << 3)
#define TIM_OC1FE		(1 << 2)
#define TIM_CCS1S_OUT	(0b00 << 0)
#define TIM_CCS1S_IN1	(0b01 << 8)
#define TIM_CCS1S_IN2	(0b10 << 8)
#define TIM_CCS1S_TRC	(0b11 << 8)

// CCMR2
#define TIM_OC4CE		(1 << 15)
#define TIM_OC4M_FROZEN	(0b000 << 12)
#define TIM_OC4M_SET	(0b001 << 12)
#define TIM_OC4M_CLR	(0b010 << 12)
#define TIM_OC4M_TOGGLE	(0b011 << 12)
#define TIM_OC4M_PWM1	(0b110 << 12)
#define TIM_OC4M_PWM2	(0b111 << 12)
#define TIM_OC4PE		(1 << 11)
#define TIM_OC4FE		(1 << 10)
#define TIM_CCS4S_OUT	(0b00 << 8)
#define TIM_CCS4S_IN3	(0b01 << 8)
#define TIM_CCS4S_IN4	(0b10 << 8)
#define TIM_CCS4S_TRC	(0b11 << 8)
#define TIM_OC3CE		(1 << 7)
#define TIM_OC3M_FROZEN	(0b000 << 4)
#define TIM_OC3M_SET	(0b001 << 4)
#define TIM_OC3M_CLR	(0b010 << 4)
#define TIM_OC3M_TOGGLE	(0b011 << 4)
#define TIM_OC3M_PWM1	(0b110 << 4)
#define TIM_OC3M_PWM2	(0b111 << 4)
#define TIM_OC3PE		(1 << 3)
#define TIM_OC3RE		(1 << 2)
#define TIM_CCS3S_OUT	(0b00 << 0)
#define TIM_CCS3S_IN3	(0b01 << 8)
#define TIM_CCS3S_IN4	(0b10 << 8)
#define TIM_CCS3S_TRC	(0b11 << 8)

// CCER
#define TIM_CC4NP		(1 << 15)
#define TIM_CC4P		(1 << 13)
#define TIM_CC4E		(1 << 12)
#define TIM_CC3NP		(1 << 11)
#define TIM_CC3P		(1 << 9)
#define TIM_CC3E		(1 << 8)
#define TIM_CC2NP		(1 << 7)
#define TIM_CC2P		(1 << 5)
#define TIM_CC2E		(1 << 4)
#define TIM_CC1NP		(1 << 3)
#define TIM_CC1P		(1 << 1)
#define TIM_CC1E		(1 << 0)


// TIM2
#define TIM2_CR1		TIM_IOREG(2, 0x00)
#define TIM2_CR2		TIM_IOREG(2, 0x04)
#define TIM2_SMCR		TIM_IOREG(2, 0x08)
#define TIM2_DIER		TIM_IOREG(2, 0x0C)
#define TIM2_SR			TIM_IOREG(2, 0x10)
#define TIM2_EGR		TIM_IOREG(2, 0x14)
#define TIM2_CCMR1		TIM_IOREG(2, 0x18)
#define TIM2_CCMR2		TIM_IOREG(2, 0x1C)
#define TIM2_CCER		TIM_IOREG(2, 0x20)
#define TIM2_CNT		TIM_IOREG(2, 0x24)
#define TIM2_PSC		TIM_IOREG(2, 0x28)
#define TIM2_ARR		TIM_IOREG(2, 0x2C)
#define TIM2_CCR1		TIM_IOREG(2, 0x34)
#define TIM2_CCR2		TIM_IOREG(2, 0x38)
#define TIM2_CCR3		TIM_IOREG(2, 0x3C)
#define TIM2_CCR4		TIM_IOREG(2, 0x40)
#define TIM2_DCR		TIM_IOREG(2, 0x48)
#define TIM2_DMAR		TIM_IOREG(2, 0x4C)
#define TIM2_OR			TIM_IOREG(2, 0x50)

// TIM3
#define TIM3_CR1		TIM_IOREG(3, 0x00)
#define TIM3_CR2		TIM_IOREG(3, 0x04)
#define TIM3_SMCR		TIM_IOREG(3, 0x08)
#define TIM3_DIER		TIM_IOREG(3, 0x0C)
#define TIM3_SR			TIM_IOREG(3, 0x10)
#define TIM3_EGR		TIM_IOREG(3, 0x14)
#define TIM3_CCMR1		TIM_IOREG(3, 0x18)
#define TIM3_CCMR2		TIM_IOREG(3, 0x1C)
#define TIM3_CCER		TIM_IOREG(3, 0x20)
#define TIM3_CNT		TIM_IOREG(3, 0x24)
#define TIM3_PSC		TIM_IOREG(3, 0x28)
#define TIM3_ARR		TIM_IOREG(3, 0x2C)
#define TIM3_CCR1		TIM_IOREG(3, 0x34)
#define TIM3_CCR2		TIM_IOREG(3, 0x38)
#define TIM3_CCR3		TIM_IOREG(3, 0x3C)
#define TIM3_CCR4		TIM_IOREG(3, 0x40)
#define TIM3_DCR		TIM_IOREG(3, 0x48)
#define TIM3_DMAR		TIM_IOREG(3, 0x4C)
#define TIM3_OR			TIM_IOREG(3, 0x50)

// TIM4
#define TIM4_CR1		TIM_IOREG(4, 0x00)
#define TIM4_CR2		TIM_IOREG(4, 0x04)
#define TIM4_SMCR		TIM_IOREG(4, 0x08)
#define TIM4_DIER		TIM_IOREG(4, 0x0C)
#define TIM4_SR			TIM_IOREG(4, 0x10)
#define TIM4_EGR		TIM_IOREG(4, 0x14)
#define TIM4_CCMR1		TIM_IOREG(4, 0x18)
#define TIM4_CCMR2		TIM_IOREG(4, 0x1C)
#define TIM4_CCER		TIM_IOREG(4, 0x20)
#define TIM4_CNT		TIM_IOREG(4, 0x24)
#define TIM4_PSC		TIM_IOREG(4, 0x28)
#define TIM4_ARR		TIM_IOREG(4, 0x2C)
#define TIM4_CCR1		TIM_IOREG(4, 0x34)
#define TIM4_CCR2		TIM_IOREG(4, 0x38)
#define TIM4_CCR3		TIM_IOREG(4, 0x3C)
#define TIM4_CCR4		TIM_IOREG(4, 0x40)
#define TIM4_DCR		TIM_IOREG(4, 0x48)
#define TIM4_DMAR		TIM_IOREG(4, 0x4C)
#define TIM4_OR			TIM_IOREG(4, 0x50)

// TIM5
#define TIM5_CR1		TIM_IOREG(5, 0x00)
#define TIM5_CR2		TIM_IOREG(5, 0x04)
#define TIM5_SMCR		TIM_IOREG(5, 0x08)
#define TIM5_DIER		TIM_IOREG(5, 0x0C)
#define TIM5_SR			TIM_IOREG(5, 0x10)
#define TIM5_EGR		TIM_IOREG(5, 0x14)
#define TIM5_CCMR1		TIM_IOREG(5, 0x18)
#define TIM5_CCMR2		TIM_IOREG(5, 0x1C)
#define TIM5_CCER		TIM_IOREG(5, 0x20)
#define TIM5_CNT		TIM_IOREG(5, 0x24)
#define TIM5_PSC		TIM_IOREG(5, 0x28)
#define TIM5_ARR		TIM_IOREG(5, 0x2C)
#define TIM5_CCR1		TIM_IOREG(5, 0x34)
#define TIM5_CCR2		TIM_IOREG(5, 0x38)
#define TIM5_CCR3		TIM_IOREG(5, 0x3C)
#define TIM5_CCR4		TIM_IOREG(5, 0x40)
#define TIM5_DCR		TIM_IOREG(5, 0x48)
#define TIM5_DMAR		TIM_IOREG(5, 0x4C)
#define TIM5_OR			TIM_IOREG(5, 0x50)


// interrupts
#define TIM1_BRK_IRQ	24
#define TIM1_UP_IRQ		25
#define TIM1_TRG_COM_IRQ	26
#define TIM1_CC_IRQ		27
#define TIM2_IRQ		28
#define TIM3_IRQ		29
#define TIM4_IRQ		30
#define TIM5_IRQ		50
#define TIM7_IRQ		55
#define TIM8_BRK_IRQ	43
#define TIM8_UO_IRQ		44
#define TIM8_TRG_COM_IRQ	45
#define TIM8_CC_IRQ		46
#define TIM9_IRQ		24
#define TIM10_IRQ		25
#define TIM11_IRQ		26
#define TIM12_IRQ		43
#define TIM13_IRQ		44
#define TIM14_IRQ		45

#endif	// STM32F4_TIM_H
