/* Embedded Systems - Exercise 14 */

#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/nvic.h>
#include <stm32f4/exti.h>
#include <stm32f4/syscfg.h>
#include <stm32f4/tim.h>
#include <stm32f4/adc.h>
#include <stm32f4/dac.h>

// GPIOD
#define GREEN_LED	12
#define ORANGE_LED	13
#define RED_LED		14
#define BLUE_LED	15

// GPIODA
#define USER_BUT	0

//GPIOA
#define POT			3

void init(){
	GPIOA_MODER = REP_BITS(GPIOA_MODER, POT*2,2,GPIO_MODER_ANA);
	ADC1_SQR3=3;
	ADC1_CR1 = 0;
	ADC1_CR2 = ADC_ADON ;

	//init LEDS
	for (int i = 11;i<=14;i++){
		GPIOB_MODER = REP_BITS(GPIOB_MODER, i*2, 2, GPIO_MODER_OUT);
		GPIOB_OTYPER = GPIOB_OTYPER && ~(1<<i);
	}
}
int main() {
	printf("\nStarting...\n");

	// RCC init
	RCC_AHB1ENR |= RCC_GPIOAEN;
	RCC_AHB1ENR |= RCC_GPIOBEN;
	RCC_APB1ENR |= RCC_TIM4EN;
	RCC_APB2ENR |= RCC_ADC1EN;
DAC_CR = DAC_EN1;
	// initialization
	init();
	// main loop
	printf("Endless loop!\n");
	while(1) {
		ADC1_CR2|= ADC_SWSTART;
		while((ADC1_SR & ADC_EOC) == 0){
		}
		long mesure = ADC1_DR;
		printf("%ld \n",mesure);
		if(mesure < 500){
			GPIOB_BSRR = 1<<(14+16);
			GPIOB_BSRR = 1<<(13+16);
			GPIOB_BSRR = 1<<(12+16);
			GPIOB_BSRR = 1<<(11+16);
		}
		else if (mesure <1000){
			GPIOB_BSRR = 1<<14;
			GPIOB_BSRR = 1<<(13+16);
			GPIOB_BSRR = 1<<(12+16);
			GPIOB_BSRR = 1<<(11+16);
		}
		else if(mesure < 2000){
			GPIOB_BSRR = 1<<14;
			GPIOB_BSRR = 1<<13;
			GPIOB_BSRR = 1<<(12+16);
			GPIOB_BSRR = 1<<(11+16);
		}
		else if(mesure < 3000){
			GPIOB_BSRR = 1<<14;
			GPIOB_BSRR = 1<<13;
			GPIOB_BSRR = 1<<12;
			GPIOB_BSRR = 1<<(11+16);
		}
		else {
			GPIOB_BSRR = 1<<14;
			GPIOB_BSRR = 1<<13;
			GPIOB_BSRR = 1<<12;
			GPIOB_BSRR = 1<<11;
		}
	}

}


