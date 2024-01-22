#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/nvic.h>
#include <stm32f4/exti.h>
#include <stm32f4/syscfg.h>
#include <stm32f4/tim.h>
#include <stm32f4/adc.h>

#include "qtr8rc.h"
#include "motor_driver.h"

// DEBUG LED (GPIOD)
#define GREEN_LED 12

// TIMER POUR SYNC
#define N 0.1
#define PSC 128 // ->1s  // 8->0.01s
#define PERIOD N*(APB1_CLK)/PSC


// PID
float Kp = 0.002; //set up the constants value
float Ki = 0.001;
float Kd = 15;
float Kr = 0;
int P, I, D, R;
int lastError = 0;
int errors[10] = {0,0,0,0,0,0,0,0,0,0};
int errorSum = 0;
int lastEnd = 0;	// 0 -> Left, 1 -> Right 
int lastIdle = 0;
const int maxSpeedRight = 10;
const int maxSpeedLeft = 10;
const int baseSpeedRight = 9;
const int baseSpeedLeft = 9;


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

    qtr8rc_init();
    motor_init();
}

void sync(void) {
    while(((TIM3_SR & TIM_UIF) == 0)) NOP;
	TIM3_SR &= ~TIM_UIF;
    return;
}

int errors_sum(int index, int abs) {
    int sum = 0;
    for (int i = 0; i < index; i++) {
    if ((abs == 1) & (errors[i] < 0)) 
        sum += -errors[i];
    else
        sum += errors[i];
    }
    return sum;
}

void compute_pid(int *position, int *motorLeftSpeed, int *motorRightSpeed) {
    // int error = 4500 - *position;

    // // save error
    // for (int i = 9; i > 0; i--) {
    //     int prev_value = error[i-1];
    //     errors[i] = prev_value;
    // }
    // errors[0] = error;

    // P = error;
    // I = errors_sum(5, 1);
    // D = error - lastError;
    // R = errors_sum(5, 1);
    // lastError = error;

    // int motorSpeed = P*Kp + I*Ki + D*Kd;

    // *motorLeftSpeed = baseSpeedLeft + motorSpeed - R*Kr;
    // *motorRightSpeed = baseSpeedRight - motorSpeed - R*Kr;

    // if (*motorLeftSpeed > maxSpeedLeft) {
    //     *motorLeftSpeed = maxSpeedLeft;
    // }
    // if (motorRightSpeed > maxSpeedRight) {
    //     *motorRightSpeed = maxSpeedRight;
    // }
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
    RCC_APB1ENR |= RCC_TIM4EN;
	RCC_APB1ENR |= RCC_TIM3EN;
	RCC_APB1ENR |= RCC_TIM2EN;
    RCC_APB2ENR |= RCC_ADC1EN;


    init();
    int position = 0;
    int motorLeftSpeed = 0;
    int motorRightSpeed = 0;

    while(1){
        qtr8rc_read(&position);
        printf("pos:%d\n", position);
        sync();
        allumer_led();
        sync();
        eteindre_led();
        sync();
        compute_pid(&position, &motorLeftSpeed, &motorRightSpeed);
        sync();
        motor_set_speeds(0, 0);
        sync();
    }
}
