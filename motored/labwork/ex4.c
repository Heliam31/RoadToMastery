/* Embedded Systems - Exercise 1 */

#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/tim.h>

// GPIOD
#define GREEN_LED	12
#define ORANGE_LED	13
#define RED_LED		14
#define BLUE_LED	15

// GPIODA
#define USER_BUT	0

void init_tim4(){
	TIM4_CR1 = 0;
	TIM4_PSC = 1000-1;
	TIM4_ARR =  1000000;
	TIM4_EGR = TIM_UG;
	TIM4_SR = 0;
}

void init(){
	GPIOA_MODER = REP_BITS(GPIOA_MODER, USER_BUT*2 , 2, GPIO_MODER_IN);
	GPIOA_PUPDR = REP_BITS(GPIOA_PUPDR, USER_BUT*2 , 2, GPIO_PUPDR_PD);
	
	for(int i = 12; i<=15; i++){
	GPIOD_MODER = REP_BITS(GPIOD_MODER, i*2, 2, GPIO_MODER_OUT);
	GPIOD_OTYPER = GPIOD_OTYPER && ~(1<<i);
	}
}

int main() {
	printf("Starting...\n");
	
	// RCC init
	RCC_AHB1ENR |= RCC_GPIOAEN;
	RCC_AHB1ENR |= RCC_GPIODEN;
	RCC_APB1ENR |= RCC_TIM4EN;

	// GPIO init
	init();
	int HALF_PERIOD = 1000000;
	printf("Endless loop!\n");
	init_tim4();
	TIM4_CR1 = TIM_CEN;
	int b_state = 0;
	while(1) {
		while((TIM4_SR & TIM_UIF)==0){
			if((GPIOA_IDR & (1<<USER_BUT)) != 0)
				b_state = 1;
			else if(b_state){
				b_state = 0;
				if (HALF_PERIOD == 250000)
					HALF_PERIOD = 1000000;
				else
					HALF_PERIOD /= 2;
			}
		}

		if ((GPIOD_ODR & (1<<GREEN_LED)) == 0)
			GPIOD_BSRR = (1 << GREEN_LED);
		else
			GPIOD_BSRR = (1 << (GREEN_LED + 16));
		TIM4_ARR = HALF_PERIOD;
		TIM4_SR = 0;
	}

}
