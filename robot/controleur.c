
#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/nvic.h>
#include <stm32f4/exti.h>
#include <stm32f4/syscfg.h>
#include <stm32f4/tim.h>
#include <stm32f4/adc.h>


#define PSC 1024 // ->1  // 8->0.01
#define PERIOD 1*(APB1_CLK)/PSC

#define PSC_DRIVER_MOTOR 8
#define PERIOD_DRIVER_MOTOR (0.01*APB1_CLK)/PSC_DRIVER_MOTOR

#define NB_OUT 4

void init_gpio(void) {
    for (int i = 0; i < NB_OUT; i++) {
        GPIOA_MODER = REP_BITS(GPIOA_MODER, i*2, 2, GPIO_MODER_ALT);
        GPIOA_AFRL = REP_BITS(GPIO_AFRL, i*4, 4, 2);
    }
}

void init_sync(void) {
    TIM3_CR1 = 0;
    TIM3_PSC = PSC-1;
	TIM3_ARR = PERIOD;
	TIM3_CR1 = TIM_CEN;
	TIM3_SR = 0;
}

void init_motor_driver(void) {
    init_gpio();    
    TIM4_CR1 = 0;
    TIM4_PSC = PSC_DRIVER_MOTOR-1;
	TIM4_ARR = PERIOD_DRIVER_MOTOR;
	TIM4_EGR = TIM_UG;
	TIM4_CCMR1 = TIM_OC1M_PWM1;
	TIM4_CCER = TIM_CC1E;
	TIM4_CR1 = TIM_ARPE | TIM_CEN;
	TIM4_SR = 0;
}

void sync(void) {
    while(((TIM4_SR & TIM_UIF) == 0));
	TIM4_SR &= ~TIM_UIF;
}

void set_speed(int speed) {
    TIM3_CCR1 = speed * PERIOD_DRIVER_MOTOR / 100
}

int main (void) {
    
    RCC_AHB1ENR |= RCC_GPIOAEN;
    RCC_AHB1ENR |= RCC_GPIODEN;
    RCC_APB1ENR |= RCC_TIM4EN;

    init_sync();
    init_controleur_motor();

    while(1) {
        set_speed(10);
        sync();
        set_speed(20);
        sync();
        set_speed(10);
        sync();
    }

}
