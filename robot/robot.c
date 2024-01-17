#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/nvic.h>
#include <stm32f4/exti.h>
#include <stm32f4/syscfg.h>
#include <stm32f4/tim.h>
#include <stm32f4/adc.h>


#define GREEN_LED 12
#define OUT 4

// TIMER POUR SYNC
#define PSC 1024 // ->1  // 8->0.01
#define PERIOD 1*(APB1_CLK)/PSC

void init_timer_sync(void) {
    TIM4_CR1 = 0;
	TIM4_PSC = PSC-1;
	TIM4_ARR = PERIOD;
	TIM4_EGR = TIM_UG;
	TIM4_CR1 |= TIM_CEN | TIM_ARPE;
	TIM4_SR = 0;
}

void init_gpio_led(void) {
	GPIOD_MODER = REP_BITS(GPIOD_MODER, GREEN_LED*2, 2, GPIO_MODER_OUT);
	GPIOD_OTYPER &= ~(1<<GREEN_LED);
}

void init_gpio_ir(void) {
    // code mohammed
}

void init_ir(void) {
    init_gpio_ir();
}

void init_gpio_motor(void) {

}

void init_driver_motor(void) {
    init_gpio_motor();
    // init adc
}

void init(void) {
    init_timer_sync();
    init_gpio_led();
    init_ir();
    init_driver_motor();
    // ...
}

void calc_traj(void) {
    // code mohammed
    return;
}

void calc_speed(int traj) {
    // code mihai
    return;
}

void sync(void) {
    while(((TIM4_SR & TIM_UIF) == 0));
	TIM4_SR &= ~TIM_UIF;
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
        position = calc_traj();
        calc_speed(position);
        sync();
        allumer_led();
        sync();
        eteindre_led();
    }
}
