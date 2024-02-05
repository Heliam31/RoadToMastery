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
#include "utils.h"


// broches utilisées du GPIOC 
#define PC0 0
#define PC1 1
#define PC2 2
#define PC3 3
#define PC6 6
#define PC7 7

typedef enum motor_e {LEFT, RIGHT} Motor;

typedef struct motor_s {
	int en;
	int in1;
	int in2;
	Motor motor;
} motor_t;

//Motor Left on GPIOC
motor_t motorLeft = {PC7, PC0, PC1, LEFT};

//Motor Right on GPIOC
motor_t motorRight = {PC6, PC2, PC3, RIGHT};

//ARR
#define T3_PSC 14
#define P3_20MS ((APB1_CLK) / (T3_PSC)) / 50
#define PMOTOR P3_20MS

void init_gpio_motor(void) {
	// Left motor
	GPIOC_MODER = REP_BITS(GPIOC_MODER, motorLeft.en*2, 2, GPIO_MODER_ALT);
	GPIOC_AFRL = REP_BITS(GPIOC_AFRL, motorLeft.en*4, 4, 2);

	GPIOC_MODER = REP_BITS(GPIOC_MODER, motorLeft.in1*2, 2, GPIO_MODER_OUT);
	GPIOC_OTYPER &= ~(1 << motorLeft.in1);
    GPIOC_MODER = REP_BITS(GPIOC_MODER, motorLeft.in2*2, 2, GPIO_MODER_OUT);
	GPIOC_OTYPER &= ~(1 << motorLeft.in2);

	// Right motor
	GPIOC_MODER = REP_BITS(GPIOC_MODER, motorRight.en*2, 2, GPIO_MODER_ALT);
	GPIOC_AFRL = REP_BITS(GPIOC_AFRL, motorRight.en*4, 4, 2);

	GPIOC_MODER = REP_BITS(GPIOC_MODER, motorRight.in1*2, 2, GPIO_MODER_OUT);
	GPIOC_OTYPER &= ~(1 << motorRight.in1);
    GPIOC_MODER = REP_BITS(GPIOC_MODER, motorRight.in2*2, 2, GPIO_MODER_OUT);
	GPIOC_OTYPER &= ~(1 << motorRight.in2);
}

void init_timer_motor(void){
	// Init TIM3
	TIM3_CR1 = 0;
	TIM3_PSC = T3_PSC - 1;
	TIM3_ARR = PMOTOR;

	// Right motor
	TIM3_CCMR1 |= TIM_OC1M_PWM1;
	TIM3_CCMR1 |= TIM_CCS1S_OUT;
	TIM3_CCMR1 |= TIM_OC1PE;
	TIM3_CCER |= TIM_CC1E;
	TIM3_CCER &= ~(TIM_CC1P);

	TIM3_CCR1 = 0;
	
	// Left motor
	TIM3_CCMR1 |= TIM_OC2M_PWM1;
	TIM3_CCMR1 |= TIM_CCS2S_OUT;
	TIM3_CCMR1 |= TIM_OC2PE;
	TIM3_CCER |= TIM_CC2E;
	TIM3_CCER &= ~(TIM_CC2P);
	TIM3_CCR2 = 0;


	TIM3_EGR = TIM_UG;
	TIM3_SR = 0; 
	TIM3_CR1 = TIM_CEN;
}	

void set_forward(Motor motor) { // Forward mode
	if (motor == LEFT) {
		GPIOC_BSRR = 1 << motorLeft.in1;
		GPIOC_BSRR = 1 << (motorLeft.in2+16);
	} else {
		GPIOC_BSRR = 1 << motorRight.in1;
		GPIOC_BSRR = 1 << (motorRight.in2+16);		
	}
}

void set_backward(Motor motor) { // Backward mode
	if (motor == LEFT) {
		GPIOC_BSRR = 1 << (motorLeft.in1+16);
		GPIOC_BSRR = 1 << motorLeft.in2;
	} else {
		GPIOC_BSRR = 1 << (motorRight.in1+16);
		GPIOC_BSRR = 1 << motorRight.in2;	
	}
}

void motor_disable(Motor motor) {
	if (motor == LEFT) {
		GPIOC_BSRR = 1 << (motorLeft.in1+16);
		GPIOC_BSRR = 1 << (motorLeft.in2+16);
	} else {
		GPIOC_BSRR = 1 << (motorRight.in1+16);
		GPIOC_BSRR = 1 << (motorRight.in2+16);	
	}
}

void motor_init(void){
    init_gpio_motor();
	init_timer_motor();
}

void set_speed_right(int speed) {
	if (speed < -100 || speed > 100) {
		printf("ERROR speed not in [-100,100], you may break... something\n");
		motor_disable(RIGHT);
		return;
	}

	if (speed < 0) // backward
		set_backward(RIGHT);
	else if (speed > 0) // forward
		set_forward(RIGHT);
	else if (speed == 0)
		motor_disable(RIGHT);

	TIM3_CCR1 = (abs(speed) * PMOTOR) / 100;
}

void set_speed_left(int speed) {
	if (speed < -100 || speed > 100) {
		printf("ERROR speed not in [-100,100], you may break... something\n");
		motor_disable(LEFT);
		return;
	}

	if (speed < 0) // backward
		set_backward(LEFT);
	else if (speed > 0) // forward
		set_forward(LEFT);
	else if (speed == 0)
		motor_disable(LEFT);

	TIM3_CCR2 = (abs(speed) * PMOTOR) / 100;
}