/* Embedded Systems - Exercise 15 */

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

// GPIODA
#define USER_BUT	0


void init_servo(){
	GPIOA_MODER = REP_BITS(GPIOA_MODER, 6*2,2,GPIO_MODER_ALT);
	GPIOA_AFRL = REP_BITS(GPIOA_AFRL, 6*4, 4, 2);

	TIM3_CR1 = 0;
	TIM3_CCMR1 = TIM_CCS1S_OUT|TIM_OC1M_PWM1;
	TIM3_CCER = TIM_CC1E;
	TIM3_CCR1 = 60000/20;
	TIM3_ARR = 60000;
	TIM3_PSC = 14 - 1;
	TIM3_EGR = TIM_UG;
	TIM3_CR1 = TIM_CEN|TIM_ARPE;
}

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

void set_servo(int n){
	TIM3_CCR1 = (60000/20) + (n*60000/20/180);
	}

int main() {
	printf("\nStarting...\n");

	// RCC init
	RCC_AHB1ENR |= RCC_GPIOAEN;
	RCC_AHB1ENR |= RCC_GPIODEN;
	RCC_APB1ENR |= RCC_TIM3EN;
	RCC_APB2ENR |= RCC_ADC1EN;

	// initialization
	init_servo();
	init_TIM4();
	int val = 0;
	int decrem = 0;
	// main loop
	printf("Endless loop!\n");
	while(1) {
		if(TIM4_triggered){
			printf("triggered");
			TIM4_triggered = 0;
			if(val == 180){
				decrem = 1;
				val = 170;
				set_servo(val);
				printf("%d!\n", val);
				}
			else if(val == 0){
				decrem = 0;
				val = 10;
				set_servo(val);
				printf("%d!\n", val);
			}
			else if(decrem == 0){
				val += 10;
				set_servo(val);
				printf("%d!\n", val);
			}
			else if(decrem == 1){
				val -= 10;
				set_servo(val);
				printf("%d!\n", val);
			}
		}

	}

}


