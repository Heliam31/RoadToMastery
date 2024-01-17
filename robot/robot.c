#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/nvic.h>
#include <stm32f4/exti.h>
#include <stm32f4/syscfg.h>
#include <stm32f4/tim.h>
#include <stm32f4/adc.h>

#include "qtr8rc.h"


#define GREEN_LED 12
#define OUT 4

// TIMER POUR SYNC
#define PSC 1024 // ->1s  // 8->0.01s
#define PERIOD 1*(APB1_CLK)/PSC

void init_timer_sync(void) {
    TIM3_CR1 = 0;
	TIM3_PSC = PSC-1;
	TIM3_ARR = PERIOD;
	TIM3_EGR = TIM_UG;
	TIM3_CR1 |= TIM_CEN | TIM_ARPE;
	TIM3_SR = 0;
}

void init_gpio_led(void) {
	GPIOD_MODER = REP_BITS(GPIOD_MODER, GREEN_LED*2, 2, GPIO_MODER_OUT);
	GPIOD_OTYPER &= ~(1<<GREEN_LED);
}

void init(void) {
    init_timer_sync();
    init_gpio_led();
    init_qtr8rc();
}

void sync(void) {
    while(((TIM3_SR & TIM_UIF) == 0));
	TIM3_SR &= ~TIM_UIF;
    return;
}

void allumer_led(void) {
    GPIOD_BSRR = 1 << GREEN_LED;
}

void eteindre_led(void) {
    GPIOD_BSRR = 1 << (GREEN_LED + 16);
}

int main(void) {

	RCC_AHB1ENR |= RCC_GPIODEN;
	RCC_APB1ENR |= RCC_TIM4EN;

    init();

    int position = 0;

    while(1){
        qtr8rc_read(&position);
        printf("pos:%d\n", position);
        sync();
        allumer_led();
        sync();
        eteindre_led();
    }
}
