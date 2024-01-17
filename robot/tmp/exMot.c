/* Embedded Systems - Exercise 12 */

#include <stdlib.h> 
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

//Moteurs Branchés sur pins PA0-PA3

//Motor 1
#define motorPin1  0
#define motorPin2  1
//Motor 2
#define motorPin3  2
#define motorPin4  3

//ARR
#define TIM2ARR   4199
	 
void init_Moteur(){
	//Moder ALT
	GPIOA_MODER = REP_BITS(GPIOA_MODER, motorPin1*2,2,GPIO_MODER_ALT);
	GPIOA_MODER = REP_BITS(GPIOA_MODER, motorPin2*2,2,GPIO_MODER_ALT);
	GPIOA_MODER = REP_BITS(GPIOA_MODER, motorPin3*2,2,GPIO_MODER_ALT);
	GPIOA_MODER = REP_BITS(GPIOA_MODER, motorPin4*2,2,GPIO_MODER_ALT);

	GPIOA_AFRL = REP_BITS(GPIOA_AFRL, motorPin1*4, 4, 1);
	GPIOA_AFRL = REP_BITS(GPIOA_AFRL, motorPin2*4, 4, 1);
	GPIOA_AFRL = REP_BITS(GPIOA_AFRL, motorPin3*4, 4, 1);
	GPIOA_AFRL = REP_BITS(GPIOA_AFRL, motorPin4*4, 4, 1);

	//Init TIM2
	TIM2_CR1 = 0;
	TIM2_CCMR1 = TIM_CCS1S_OUT|TIM_OC1M_PWM1|TIM_CCS2S_OUT|TIM_OC2M_PWM1;
	TIM2_CCMR2 = TIM_CCS3S_OUT|TIM_OC3M_PWM1|TIM_CCS4S_OUT|TIM_OC4M_PWM1;

	TIM2_CCER = TIM_CC1E|TIM_CC2E|TIM_CC3E|TIM_CC4E;

	//Taux PWM
	TIM2_CCR1 = 0;
	TIM2_CCR2 = 0;
	TIM2_CCR3 = 0;
	TIM2_CCR4 = 0;

	//Période
	TIM2_ARR = TIM2ARR;
	//Prescaler
	TIM2_PSC = 1000 - 1;
	//Lancer Timer
	TIM2_EGR = TIM_UG;
	TIM2_CR1 = TIM_CEN|TIM_ARPE;
}	

void init(){
	GPIOD_MODER = REP_BITS(GPIOD_MODER, GREEN_LED*2, 2, GPIO_MODER_OUT);
	GPIOD_OTYPER = GPIOD_OTYPER && ~(1<<GREEN_LED);

	init_Moteur();
}

// mot1 pourcentage positif ou negatif de vitesse moteur 1
// mot2 pourcentage positif ou negatif de vitesse moteur 2
void actionMoteur(int mot1, int mot2){
	//moteur 1
	if (mot1 <= 0){
		TIM2_CCR2 = (abs(mot1) * TIM2ARR)/100;
		TIM2_CCR1 = 0;
	}
	else if (mot1 > 0){
		TIM2_CCR1 = (abs(mot1) * TIM2ARR)/100;
		TIM2_CCR2 = 0;
	}
	
	//moteur 2
	if (mot2 <= 0){
		TIM2_CCR4 = (abs(mot2) * TIM2ARR)/100;
		TIM2_CCR3 = 0;
	}
	else if (mot2 > 0){
		TIM2_CCR3 = (abs(mot2) * TIM2ARR)/100;
		TIM2_CCR4 = 0;
	}
}
int main() {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
	printf("\nStarting...\n");
	
	// RCC init
	RCC_AHB1ENR |= RCC_GPIODEN;
	RCC_AHB1ENR |= RCC_GPIOAEN;
	RCC_APB1ENR  |= RCC_DACEN;
	RCC_APB1ENR |= RCC_TIM2EN;

	init();

	printf("Endless loop!\n");
	while(1) {
		GPIOD_BSRR = (1<<GREEN_LED);
		actionMoteur(100,100);

		for (int i = 0; i< 30000000; i++){
			NOP;
		}

		actionMoteur(0,0);
		GPIOD_BSRR = (1<<(GREEN_LED+16));
		for (int i = 0; i< 30000000; i++){
			NOP;
		}

		actionMoteur(-100,-100);
		GPIOD_BSRR = (1<<(GREEN_LED+16));
		for (int i = 0; i< 30000000; i++){
			NOP;
		}

		actionMoteur(0,0);
		GPIOD_BSRR = (1<<(GREEN_LED+16));
		for (int i = 0; i< 30000000; i++){
			NOP;
		}
	}
}
