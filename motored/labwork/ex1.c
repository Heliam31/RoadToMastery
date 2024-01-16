/* Embedded Systems - Exercise 1 */

#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <unistd.h>


// GPIOD
#define GREEN_LED	12
#define ORANGE_LED	13
#define RED_LED		14
#define BLUE_LED	15

// GPIOA
#define USER_BUT	0


int main() {
	printf("Starting...\n");

	// RCC init
	RCC_AHB1ENR |= RCC_GPIODEN;

	// GPIO init
	for(int i = 12; i<=15; i++){
	GPIOD_MODER = REP_BITS(GPIOD_MODER, i*2, 2, GPIO_MODER_OUT);
	GPIOD_OTYPER = GPIOD_OTYPER && ~(1<<i);
	}

	printf("Endless loop!\n");

	while(1) {
		
		GPIOD_BSRR = (1 << GREEN_LED);

		for (int i = 0; i< 30000000; i++){
			NOP;
		}

		GPIOD_BSRR = (1<< (GREEN_LED+16))|(1 << ORANGE_LED);

		for (int i = 0; i< 30000000; i++){
			NOP;
		}

		GPIOD_BSRR = (1<< (ORANGE_LED+16))|(1 << RED_LED);

		for (int i = 0; i< 30000000; i++){
			NOP;
		}

		GPIOD_BSRR = (1<< (RED_LED+16))|(1 << BLUE_LED);

		for (int i = 0; i< 30000000; i++){
			NOP;
		}

		GPIOD_BSRR = (1<< (BLUE_LED+16));
		
		// int x = 0;
		// for (int i = 0; i< 30000000; i++){
		// 	NOP;
		// }
		// if (x != 0) GPIOD_BSRR = (1<< ((11+x)+16))|(1 << (12+x));
		// else GPIOD_BSRR = (1 << (12+x));
		// printf("allumed\n");
		// if (x == 3) {
		// 	x=0;
		// 	printf("Retour12\n");
		// }
		// else {
		// 	x++;
		// 	printf("incremented\n");
		// }
		
	}

}
