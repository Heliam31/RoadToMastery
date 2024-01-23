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

#include <tinyprintf.h>
#include <stm32f4/gpio.h>
#include <stm32f4/led.h>
#include <stm32f4/tim.h>
#include <stm32f4/rcc.h>

#define PSC	1000
#define ARR	(APB1_CLK / PSC)
#define LED_NUM(n)	((n) - LED_GREEN + 1)

/**
 * Initialize the LED subsystem.
 */
void led_init() {

	// enable timer and GPIO
	RCC_AHB1ENR |= RCC_GPIODEN;
	RCC_APB1ENR |= RCC_TIM4EN;

	// switch off all 
	led_off(LED_BLUE);
	led_off(LED_GREEN);
	led_off(LED_ORANGE);
	led_off(LED_RED);

	// configure timer
	// prescaler !
	TIM_ARR(4) = ARR;
	TIM_PSC(4) = PSC;
	TIM_CCR1(4) = 0;
	TIM_CCR2(4) = 0;
	TIM_CCR3(4) = 0;
	TIM_CCR4(4) = 0;
	TIM_CCMR1(4) =
		TIM_OC2M_PWM1 | TIM_OC2PE |
		TIM_OC1M_PWM1 | TIM_OC1PE;
	TIM_CCMR2(4) =
		TIM_OC4M_PWM1 | TIM_OC4PE |
		TIM_OC3M_PWM1 | TIM_OC3PE;
	TIM_EGR(4) = TIM_UG;
	TIM_CR1(4) = TIM_ARPE | TIM_CEN;
}

/**
 * Switch on a LED.
 * @param led	LED number.
 */
void led_on(int led) {
	TIM_CCER(4) &= ~TIM_CCxE(LED_NUM(led));
	GPIO_MODER_SET(led, GPIO_MODER_OUT);	
	GPIO_BSRR_SET(led);
}

/**
 * Switch off a LED.
 * @param led	LED number.
 */
void led_off(int led) {
	TIM_CCER(4) &= ~TIM_CCxE(LED_NUM(led));
	GPIO_MODER_SET(led, GPIO_MODER_OUT);	
	GPIO_BSRR_RESET(led);
}


/**
 * Make a LED to blink.
 * @param led	LED number.
 */
void led_blink(int led) {

	// configure timer
	TIM_CCRx(4, LED_NUM(led)) = ARR / 2;
	TIM_CCER(4) |= TIM_CCxE(LED_NUM(led));

	// configure ALT function
	GPIO_MODER_SET(led, GPIO_MODER_ALT);
	GPIO_AFR(led, 2);
}
