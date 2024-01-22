
#include <stdlib.h>
#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/nvic.h>
#include <stm32f4/exti.h>
#include <stm32f4/syscfg.h>
#include <stm32f4/tim.h>
#include <stm32f4/adc.h>

#include "motor_driver.h"


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

void init_gpio_motor(void) {
    //Moder ALT
	GPIOA_MODER = REP_BITS(GPIOA_MODER, motorPin1*2,2,GPIO_MODER_ALT);
	GPIOA_MODER = REP_BITS(GPIOA_MODER, motorPin2*2,2,GPIO_MODER_ALT);
	GPIOA_MODER = REP_BITS(GPIOA_MODER, motorPin3*2,2,GPIO_MODER_ALT);
	GPIOA_MODER = REP_BITS(GPIOA_MODER, motorPin4*2,2,GPIO_MODER_ALT);

	GPIOA_AFRL = REP_BITS(GPIOA_AFRL, motorPin1*4, 4, 1);
	GPIOA_AFRL = REP_BITS(GPIOA_AFRL, motorPin2*4, 4, 1);
	GPIOA_AFRL = REP_BITS(GPIOA_AFRL, motorPin3*4, 4, 1);
	GPIOA_AFRL = REP_BITS(GPIOA_AFRL, motorPin4*4, 4, 1);
}

void init_timer_motor(void){
	// Init TIM2
	TIM2_CR1 = 0;
	TIM2_CCMR1 = TIM_CCS1S_OUT|TIM_OC1M_PWM1|TIM_CCS2S_OUT|TIM_OC2M_PWM1;
	TIM2_CCMR2 = TIM_CCS3S_OUT|TIM_OC3M_PWM1|TIM_CCS4S_OUT|TIM_OC4M_PWM1;
	TIM2_CCER = TIM_CC1E|TIM_CC2E|TIM_CC3E|TIM_CC4E;

	// Taux PWM
	TIM2_CCR1 = 0;
	TIM2_CCR2 = 0;
	TIM2_CCR3 = 0;
	TIM2_CCR4 = 0;

	// Période
	TIM2_ARR = TIM2ARR;
	// Prescaler
	TIM2_PSC = 1000 - 1;
	// Lancer Timer
	TIM2_EGR = TIM_UG;
	TIM2_CR1 = TIM_CEN|TIM_ARPE;
}	

void motor_init(void){
    init_gpio_motor();
	init_timer_motor();
}

void motor_set_speeds(int speedMotorLeft, int speedMotorRight) {
	// moteur 1
	if (speedMotorLeft <= 0){ // marche arriere
		TIM2_CCR2 = (abs(speedMotorLeft) * TIM2ARR)/100;
		TIM2_CCR1 = 0;
	}
	else if (speedMotorLeft > 0){ // marche avant
		TIM2_CCR1 = (abs(speedMotorLeft) * TIM2ARR)/100;
		TIM2_CCR2 = 0;
	}
	
	// moteur 2
	if (speedMotorRight <= 0){ // marche arriere
		TIM2_CCR4 = (abs(speedMotorRight) * TIM2ARR)/100;
		TIM2_CCR3 = 0;
	}
	else if (speedMotorRight > 0){ // marche avant
		TIM2_CCR3 = (abs(speedMotorRight) * TIM2ARR)/100;
		TIM2_CCR4 = 0;
	}
}
