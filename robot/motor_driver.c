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
#define enA 7

//Motor 2
#define motorPin3  2
#define motorPin4  3
#define enB 6

//ARR
#define T3_PSC 14
#define P3_20MS ((APB1_CLK) / (T3_PSC)) / 50
#define PSERVO P3_20MS

void init_gpio_motor(void) {
    // Moder ALT for enA enB
	GPIOC_MODER = REP_BITS(GPIOC_MODER, enA * 2, 2, GPIO_MODER_ALT);
	GPIOC_MODER = REP_BITS(GPIOC_MODER, enB * 2, 2, GPIO_MODER_ALT);
	
	GPIOC_AFRL = REP_BITS(GPIOC_AFRL, enA * 4, 4, 2);
	GPIOC_AFRL = REP_BITS(GPIOC_AFRL, enB * 4, 4, 2);
	
	// Output in1, in2, in3, in4
	GPIOA_MODER = REP_BITS(GPIOA_MODER, motorPin1 * 2, 2, GPIO_MODER_OUT);
	GPIOA_OTYPER &= ~(1 << motorPin1);
    GPIOA_MODER = REP_BITS(GPIOA_MODER, motorPin2 * 2, 2, GPIO_MODER_OUT);
	GPIOA_OTYPER &= ~(1 << motorPin2);
    GPIOA_MODER = REP_BITS(GPIOA_MODER, motorPin3 * 2, 2, GPIO_MODER_OUT);
	GPIOA_OTYPER &= ~(1 << motorPin3);
    GPIOA_MODER = REP_BITS(GPIOA_MODER, motorPin4 * 2, 2, GPIO_MODER_OUT);
	GPIOA_OTYPER &= ~(1 << motorPin4);
}

void init_timer_motor(void){
	// Init TIM3
	TIM3_CR1 = 0;
	TIM3_PSC = T3_PSC - 1;
	TIM3_ARR = PSERVO;

	TIM3_CCMR1 = TIM_CCS1S_OUT|TIM_OC1M_PWM1|TIM_OC1PE; // Right motor
	TIM3_CCMR2 = TIM_CCS2S_OUT|TIM_OC2M_PWM1|TIM_OC2PE; // Left motor
	TIM3_CCER = TIM_CC1E|TIM_CC2E;
	TIM3_CCER &= ~(TIM_CC1P);
	TIM3_CCER &= ~(TIM_CC2P);

	// Taux PWM
	TIM3_CCR1 = 0;
	TIM3_CCR2 = 0;

	// Période
	// Prescaler
	// Lancer Timer
	TIM3_EGR = TIM_UG;
	TIM3_SR = 0;
	TIM3_CR1 = TIM_CEN|TIM_ARPE;
}	

void set_forward(void) { // Forward mode
	// Motor 1
    GPIOA_BSRR |= 1 << (motorPin1);
    GPIOA_BSRR |= 1 << (motorPin2 + 16);

	// Motor 2
    GPIOA_BSRR |= 1 << (motorPin3);
    GPIOA_BSRR |= 1 << (motorPin4 + 16);
}

void set_backward(void) { // Backward mode
	// Motor 1
    GPIOA_BSRR |= 1 << (motorPin1 + 16);
    GPIOA_BSRR |= 1 << (motorPin2);

	// Motor 2
    GPIOA_BSRR |= 1 << (motorPin3 + 16);
    GPIOA_BSRR |= 1 << (motorPin4);
}

void motor_disable(void) {
	// Motor 1
    GPIOA_BSRR |= 1 << (motorPin1 + 16);
    GPIOA_BSRR |= 1 << (motorPin2 + 16);

	// Motor 2
    GPIOA_BSRR |= 1 << (motorPin3 + 16);
    GPIOA_BSRR |= 1 << (motorPin4 + 16);
}

void motor_init(void){
    init_gpio_motor();
	init_timer_motor();
	set_forward();
}

void motor_set_speeds(int speedMotorLeft, int speedMotorRight) {
	// Motor 1
	if (speedMotorLeft < -100 || speedMotorLeft > 100 || speedMotorRight < -100 || speedMotorRight > 100) {
		printf("ERROR speed not in [-100,100], you may break... something\n");
		return;
	}

	if (speedMotorLeft <= 0){ // backward
		TIM3_CCR1 = (abs(speedMotorLeft) * PSERVO)/100;
		set_backward();
	}
	else if (speedMotorLeft > 0){ // forward
		TIM3_CCR1 = (abs(speedMotorLeft) * PSERVO)/100;
		set_forward();
	}
	
	// Motor 2
	if (speedMotorRight <= 0){ // backward
		TIM3_CCR2 = (abs(speedMotorRight) * PSERVO)/100;
		set_backward();
	}
	else if (speedMotorRight > 0){ // forward
		TIM3_CCR2 = (abs(speedMotorRight) * PSERVO)/100;
		set_forward();
	}
}
