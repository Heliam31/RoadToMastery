#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/nvic.h>
#include <stm32f4/exti.h>
#include <stm32f4/syscfg.h>
#include <stm32f4/tim.h>
#include <stm32f4/adc.h>

#include "pid.h"
#include "qtr8rc.h"
#include "motor_driver.h"

// DEBUG LED (GPIOD)
#define GREEN_LED 12

// TIMER POUR SYNC
#define N 0.1
#define PSC 128 // ->1s  // 8->0.01s
#define PERIOD (N*APB1_CLK)/PSC

void init_timer_sync(void) {
    TIM2_CR1 = 0;
	TIM2_PSC = PSC-1;
	TIM2_ARR = PERIOD;
	TIM2_EGR = TIM_UG;
	TIM2_CR1 |= TIM_CEN | TIM_ARPE;
	TIM2_SR = 0;
}

void init_gpio_led(void) {
	GPIOD_MODER = REP_BITS(GPIOD_MODER, GREEN_LED*2, 2, GPIO_MODER_OUT);
	GPIOD_OTYPER &= ~(1<<GREEN_LED);
}

void init(void) {
    init_timer_sync();
    init_gpio_led();
    qtr8rc_init();
    // motor_init();
    l298nDCMotor_init();
}

void sync(void) {
    while(((TIM3_SR & TIM_UIF) == 0)) NOP;
	TIM2_SR &= ~TIM_UIF;
    return;
}

void allumer_led(void) {
    GPIOD_BSRR = 1 << GREEN_LED;
}

void eteindre_led(void) {
    GPIOD_BSRR = 1 << (GREEN_LED + 16);
}

int main(void) {

    RCC_AHB1ENR |= RCC_GPIOAEN;
	RCC_AHB1ENR |= RCC_GPIODEN;
	RCC_AHB1ENR |= RCC_GPIOCEN;

    RCC_APB1ENR |= RCC_TIM4EN;
	RCC_APB1ENR |= RCC_TIM3EN;
	RCC_APB1ENR |= RCC_TIM2EN;

    RCC_APB2ENR |= RCC_ADC1EN;

    init();
    
    int position = 0;
    int motorLeftSpeed = 25;
    int motorRightSpeed = 25;

    while(1){
        // qtr8rc_read(&position);
        // printf("pos:%d\n", position);

        // calculate_motor_speed(&motorLeftSpeed, &motorRightSpeed, position);
        // printf("motorLeft = %d\n", motorLeftSpeed);
        // printf("motorRight = %d\n", motorRightSpeed);

        // motor_set_speeds(100, 0);
        set_speed_left(motorLeftSpeed);
        set_speed_right(motorRightSpeed);
    }
}
