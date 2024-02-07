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
#include <stm32f4/led.h>
#include <stm32f4/rcc.h>
#include <stm32f4/tim.h>
#include <stm32f4/gpio.h>

volatile int n = 10;

#define PERIOD	30000000
#define WAIT(n) \
	for (int i = 0; i < (n); i++) \
		__asm__("nop");


// GPIOD
#define GREEN_LED	12
#define ORANGE_LED	13
#define RED_LED		14
#define BLUE_LED	15

// GPIODA
#define USER_BUT	0

int main() {
	printf("Starting...\n");

	// RCC init
	RCC_AHB1ENR |= RCC_GPIODEN;

	// GPIO init
	GPIOD_MODER = SET_BITS(GPIOD_MODER, 2*GREEN_LED, 2, GPIO_MODER_OUT);
	GPIOD_BSRR = 1 << (GREEN_LED + 16);
	GPIOD_MODER = SET_BITS(GPIOD_MODER, 2*ORANGE_LED, 2, GPIO_MODER_OUT);
	GPIOD_BSRR = 1 << (ORANGE_LED + 16);
	GPIOD_MODER = SET_BITS(GPIOD_MODER, 2*RED_LED, 2, GPIO_MODER_OUT);
	GPIOD_BSRR = 1 << (RED_LED + 16);
	GPIOD_MODER = SET_BITS(GPIOD_MODER, 2*BLUE_LED, 2, GPIO_MODER_OUT);
	GPIOD_BSRR = 1 << (BLUE_LED + 16);

	printf("LED switched on!\n");

	while(1) {
		printf("Next!\n");

		/* green */
		GPIOD_BSRR = 1 << GREEN_LED;
		WAIT(PERIOD);
		GPIOD_BSRR = 1 << (GREEN_LED + 16);

		/* orange */
		GPIOD_BSRR = 1 << ORANGE_LED;
		WAIT(PERIOD);
		GPIOD_BSRR = 1 << (ORANGE_LED + 16);

		/* red */
		/*GPIOD_BSRR = 1 << RED_LED;
		WAIT(PERIOD);
		GPIOD_BSRR = 1 << (RED_LED + 16);*/

		/* blue */
		GPIOD_BSRR = 1 << BLUE_LED;
		WAIT(PERIOD);
		GPIOD_BSRR = 1 << (BLUE_LED + 16);

	}

	//led_init();


/*
	// endless loop
#if 0
	int cnt = 0, i;
	while(1) {

		led_on(LED_ORANGE);
		led_off(LED_RED);
		led_on(LED_BLUE);
		led_off(LED_GREEN);
		printf("ping %d\n", cnt);

		for (i = 0; i < 6000000; i++)
			__asm__("nop");

		led_off(LED_ORANGE);
		led_on(LED_RED);
		led_off(LED_BLUE);
		led_on(LED_GREEN);
		printf("pong %d\n", cnt++);

		for (i = 0; i < 6000000; i++)
			__asm__("nop");
	}
#endif

#if 0
	int i;
	while(1) {
		_SET(GPIO_ODR(3), 15, 12, 0b1010);
		for (i = 0; i < 6000000; i++)
			__asm__("nop");
		_SET(GPIO_ODR(3), 15, 12, 0b0101);
		for (i = 0; i < 6000000; i++)
			__asm__("nop");
	}
#endif

#if 0
	led_on(LED_BLUE);
	led_on(LED_ORANGE);
	led_off(LED_GREEN);
	led_blink(LED_RED);
	while(1);
#endif*/
}
