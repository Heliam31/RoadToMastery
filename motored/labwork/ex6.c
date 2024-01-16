/* Embedded Systems - Exercise 6 */


#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/tim.h>
#include <stm32f4/nvic.h>

// GPIOD
#define GREEN_LED	12
#define ORANGE_LED	13
#define RED_LED		14
#define BLUE_LED	15

// GPIODA
#define USER_BUT	0

volatile int TIM4_triggered = 0;

void handle_TIM4()  {   //handle l'interruption
	TIM4_SR = ~TIM_UIF;
	TIM4_ARR = 50000;
	printf("handled");
	TIM4_triggered = 1;
	
}


void init_TIM4(){
	DISABLE_IRQS;

	//NVIC
	NVIC_ICER(TIM4_IRQ >> 5) = 1 << (TIM4_IRQ & 0X1f);
	NVIC_IRQ(TIM4_IRQ) = (uint32_t)handle_TIM4;
	NVIC_IPR(TIM4_IRQ) = 0;

	//purge IRQ
	NVIC_ICPR(TIM4_IRQ >> 5) = 1<<(TIM4_IRQ & 0X1f);

	//configure TIM4
	TIM4_CR1 = 0;
	TIM4_PSC = 1000-1;
	TIM4_ARR =  50000;
	//reset Timer
	TIM4_EGR = TIM_UG;
	TIM4_SR = 0;

	//enable IRQ
	NVIC_ISER(TIM4_IRQ >> 5) = 1 <<(TIM4_IRQ & 0X1f);
	TIM4_DIER = TIM_UIE;

	//start
	ENABLE_IRQS;
	TIM4_CR1 = TIM_CEN;
}




int main() {
	printf("\nStarting...\n");

	// RCC init
	RCC_AHB1ENR |= RCC_GPIOAEN;
	RCC_AHB1ENR |= RCC_GPIODEN;
	RCC_APB1ENR |= RCC_TIM4EN;

	//GPIO INIT
	for(int i = 12; i<=15; i++){
		GPIOD_MODER = REP_BITS(GPIOD_MODER, i*2, 2, GPIO_MODER_OUT);
		GPIOD_OTYPER = GPIOD_OTYPER && ~(1<<i);
	}
	
	init_TIM4();

	// main loop
	printf("Endless loop!\n");
	while(1) {
		// printf("compteur: %d \n", TIM4_CNT);
		if(TIM4_triggered){
			printf("triggered");
			TIM4_triggered = 0;
			if((GPIOD_ODR & (1<<GREEN_LED)) == 0)
				GPIOD_BSRR = 1 << GREEN_LED;
			else
				GPIOD_BSRR = 1 << (GREEN_LED + 16);
		}
	}
}


