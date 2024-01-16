/* Embedded Systems - Exercise 5 */

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

#define WAIT_PSC 1000
#define WAIT_DELAY (APB1_CLK / WAIT_PSC)
#define HALF_PERIOD (WAIT_DELAY/2)

void init(){
	// output 
	GPIOD_MODER = REP_BITS(GPIOD_MODER, GREEN_LED*2, 2, 0b01);
	GPIOD_OTYPER &= ~(1<<GREEN_LED);
	GPIOD_PUPDR = REP_BITS(GPIOD_PUPDR, GREEN_LED*2, 2, 0b00);
	GPIOD_MODER = REP_BITS(GPIOD_MODER, RED_LED*2, 2, 0b01);
	GPIOD_OTYPER &= ~(1<<RED_LED);
	GPIOD_PUPDR = REP_BITS(GPIOD_PUPDR, RED_LED*2, 2, 0b00);
	GPIOD_BSRR = 1 << (16 + RED_LED);
}

void /*__attribute__((interrupt("IRQ")))*/ handle_TIM4() {
	//TIM4_ARR = HALF_PERIOD;
	//TIM4_SR = 0;
	//printf("!");
	//if((TIM4_SR & TIM_UIF) != 0) {
		TIM4_SR = 0;

		if((GPIOD_ODR & (1<<GREEN_LED))==0){
			//printf("on allume\n");
			GPIOD_BSRR = 1 << GREEN_LED;
		}
		else{
			//printf("on éteint\n");
			GPIOD_BSRR = 1 << (16 + GREEN_LED);
		}		
		NVIC_ICPR(TIM4_IRQ >> 5) |= 1 << (TIM4_IRQ & 0X1f);
	//}
	//else
	//GPIOD_BSRR = 1 << RED_LED;
}

void init_TIM4(){
	DISABLE_IRQS;

	NVIC_ICER(TIM4_IRQ>>5) |= 1 << (TIM4_IRQ & 0X1f);
	NVIC_IRQ(TIM4_IRQ) = (uint32_t)handle_TIM4;
	NVIC_IPR(TIM4_IRQ) = 0;

	NVIC_ICPR(TIM4_IRQ >> 5) |= 1 << (TIM4_IRQ & 0X1f);
	NVIC_ISER(TIM4_IRQ >> 5) |= 1 << (TIM4_IRQ & 0X1f);

	//TIM4_CR1 &= ~TIM_CEN;
	TIM4_CR1 = 0;
	TIM4_PSC = WAIT_PSC;
	//TIM4_ARR = HALF_PERIOD;
	TIM4_ARR = WAIT_DELAY;
	TIM4_EGR = TIM_UG;
	TIM4_SR = 0;
	TIM4_CR1 = TIM_ARPE;
	TIM4_SR &= ~TIM_UIF;
	TIM4_DIER = TIM_UIE;

	//ENABLE_IRQS;
	//TIM4_CR1 |= TIM_CEN;
}

int main() {
	printf("\nStarting...\n");
	// RCC init
	RCC_AHB1ENR |= RCC_GPIOAEN;
	RCC_AHB1ENR |= RCC_GPIODEN;
	RCC_APB1ENR |= RCC_TIM4EN;

	init();
	init_TIM4();

	ENABLE_IRQS;
	TIM4_CR1 |= TIM_CEN;
	
	// main loop
	printf("Endless loop!\n");
	while(1) {
	}

}


