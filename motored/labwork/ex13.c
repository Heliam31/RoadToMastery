/* Embedded Systems - Exercise 13 */

#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/nvic.h>
#include <stm32f4/exti.h>
#include <stm32f4/syscfg.h>
#include <stm32f4/tim.h>
#include <stm32f4/adc.h>


// GPIOD
#define GREEN_LED	12
#define ORANGE_LED	13
#define RED_LED		14
#define BLUE_LED	15

#define RES		 	3
//BOUTTON PD1
#define LED 		1  
// GPIODA
#define USER_BUT	0
volatile int TIM4_triggered = 0;
void handleADC(){
	TIM4_SR = ~TIM_UIF;
	TIM4_ARR = 50000;
	printf("handled");
	TIM4_triggered = 1;
}
void init(){
	GPIOA_MODER = REP_BITS(GPIOA_MODER, RES*2,2,GPIO_MODER_ANA);
	ADC1_SQR3=3;
	ADC1_CR1 = 0;
	ADC1_CR2 = ADC_ADON ;

	DISABLE_IRQS;

	//NVIC
	NVIC_ICER(TIM4_IRQ >> 5) = 1 << (TIM4_IRQ & 0X1f);
	NVIC_IRQ(TIM4_IRQ) = (uint32_t)handleADC;
	NVIC_IPR(TIM4_IRQ) = 0;

	//purge IRQ
	NVIC_ICPR(TIM4_IRQ >> 5) = 1<<(TIM4_IRQ & 0X1f);

	//configure TIM4
	TIM4_CR1 = 0;
	TIM4_PSC = 1000-1;
	TIM4_ARR =  10000;
	//reset Timer
	TIM4_EGR = TIM_UG;
	TIM4_SR = 0;

	//enable IRQ
	NVIC_ISER(TIM4_IRQ >> 5) = 1 <<(TIM4_IRQ & 0X1f);
	TIM4_DIER = TIM_UIE;

	//start
	ENABLE_IRQS;
	TIM4_CR1 = TIM_CEN;

	//Init led
	GPIOD_MODER = REP_BITS(GPIOD_MODER, LED*2, 2, GPIO_MODER_OUT);
	GPIOD_OTYPER = GPIOD_OTYPER && ~(1<<LED);
}

int main() {
	printf("\nStarting...\n");
	// RCC init
	RCC_AHB1ENR |= RCC_GPIOAEN;
	RCC_AHB1ENR |= RCC_GPIODEN;
	RCC_APB1ENR |= RCC_TIM4EN;
	RCC_APB2ENR |= RCC_ADC1EN;

	// initialization
	init();
	// main loop
	printf("Endless loop!\n");
	while(1) {
		if(TIM4_triggered){
			TIM4_triggered = 0;
			printf("triggered");
			ADC1_CR2|= ADC_SWSTART;
			while((ADC1_SR & ADC_EOC) == 0){
			}
			long mesure = ADC1_DR;
			printf("mesure %ld\n",mesure);
			if (mesure < 2000){
				GPIOD_BSRR = 1<<LED;
			}
			else{
				GPIOD_BSRR = 1 <<(16+LED);
			}
		}	
	}
}






