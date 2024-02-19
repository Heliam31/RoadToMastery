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
#include "utils.h"
#include "i2c.h"

// DEBUG (GPIO)
#define GREEN_LED 12
#define BUT 0

// TIMER POUR SYNC
#define N 0.1
#define PSC 128 // ->1s  // 8->0.01s
#define PERIOD (N*APB1_CLK)/PSC

// UTILS 
void _robot_delay(int cycles) {
    for(int i = 0; i < cycles; i++) NOP;
}

void robot_wait_seconds(float seconds) {
    int cycles = seconds*APB1_CLK;
    _robot_delay(cycles);
}

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

void init_gpio_button(void) {
    GPIOA_MODER = REP_BITS(GPIOA_MODER, BUT*2, 2, GPIO_MODER_IN);
	GPIOA_PUPDR = REP_BITS(GPIOA_PUPDR, BUT*2, 2, GPIO_PUPDR_PD);
}

void init(void) {
    init_timer_sync();
    init_gpio_led();
    init_gpio_button();

    qtr8rc_init();
    motor_init();
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

    int start = 0;
    while(!start) {
        if ((GPIOA_IDR & (1 << BUT)) != 0)  {
            start = 1;
        }
    }

    printf("Button pushed 1\n");

    qtr8rc_calibrate();

    start = 0;
    while(!start) {
        if ((GPIOA_IDR & (1 << BUT)) != 0)  {
            start = 1;
        }
    }

    printf("Button pushed 2\n");
    printf("Start\n");

    while(1){
        qtr8rc_read_calibrated(&position);
        set_speed_left(motorLeftSpeed);
        set_speed_right(motorRightSpeed);
    }
}
