
#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/nvic.h>
#include <stm32f4/exti.h>
#include <stm32f4/syscfg.h>
#include <stm32f4/tim.h>
#include <stm32f4/adc.h>

#include "qtr8rc.h"


#define NB_QTR_SENSORS 8

// GPIOD
#define IR1_LED    9
#define IR2_LED    10
#define IR3_LED    0
#define IR4_LED    1
#define IR5_LED    2
#define IR6_LED    3
#define IR7_LED    6
#define IR8_LED    7
const int IR_LEDS[8] = {IR1_LED, IR2_LED, IR3_LED, IR4_LED, IR5_LED, IR6_LED, IR7_LED, IR8_LED};
#define ON_LED    11

// TIM4
#define N 1
#define WAIT_PSC 1000
#define WAIT_DELAY (N*APB1_CLK)/WAIT_PSC
int PERIOD = WAIT_DELAY;
int _maxValue = 4000;

// GLOBAL
int actives = 0;
int position;

void _delay(int cycles) {
    for(int i = 0; i < cycles; i++) NOP;
}

void wait_seconds(float seconds) {
    int cycles = seconds*APB1_CLK;
    _delay(cycles);
}

void init_gpiod_out(void) {
    for (int i = 0; i < NB_QTR_SENSORS; i++) {
        GPIOD_MODER = REP_BITS(GPIOD_MODER, IR_LEDS[i] * 2, 2, GPIO_MODER_OUT);
        GPIOD_OTYPER &= ~(1 << IR_LEDS[i]);
    }
}

void init_gpiod_in(void) {
    for (int i = 0; i < NB_QTR_SENSORS; i++) {
        GPIOD_MODER = REP_BITS(GPIOD_MODER, IR_LEDS[i] * 2, 2, GPIO_MODER_IN);
        GPIOD_OTYPER &= ~(1 << IR_LEDS[i]);
    }
}


void gpiod_drive_high(void) { // make sensor line an output and drive high
    for (int i = 0; i < NB_QTR_SENSORS; i++) {
        GPIOD_ODR |= 1 << IR_LEDS[i];
    }
}

void init_gpio_qtr8rc() {
    // IR GPIO init
    init_gpiod_out();
    // LEDON GPIO init
    GPIOD_MODER = REP_BITS(GPIOD_MODER, ON_LED * 2, 2, GPIO_MODER_OUT);
    GPIOD_OTYPER &= ~(1 << ON_LED);
}

void init_tim4_qtr8rc(){
	TIM4_CR1 = 0;
	TIM4_PSC = WAIT_PSC-1;
	TIM4_ARR = PERIOD;
	TIM4_EGR = TIM_UG;
	TIM4_SR = 0;
	TIM4_CR1 = TIM_CEN;
}

void compute_time(int *sensorValues, uint32_t elapsedTime) {
    for (unsigned int i = 0; i < NB_QTR_SENSORS; i++) {
        if (((GPIOD_IDR & (1 << IR_LEDS[i])) == 0) && (elapsedTime < sensorValues[i])){
            sensorValues[i] = elapsedTime;
        }
    }
}

int compute_position(int *sensorValues) {
    int sum = 0;
    for (int i = 0; i < NB_QTR_SENSORS; i++) {
        sum += sensorValues[i];
    }
    int rsl = 0;
    int c = 0;
    for (int i = 0; i < NB_QTR_SENSORS; i++) {
        rsl += sensorValues[i] * c;
        c += 1000;
    }
    return rsl/sum;
}

void qtr8rc_init(void) {
    init_gpio_qtr8rc();
	init_tim4_qtr8rc();
}

void qtr8rc_read(int* position) {
    int sensorValues[NB_QTR_SENSORS] = {_maxValue,_maxValue,_maxValue,_maxValue,_maxValue,_maxValue,_maxValue,_maxValue};

    TIM4_CNT = 0;
    TIM4_SR = 0;
    TIM4_CR1 = TIM_CEN;

    init_gpiod_out();
    gpiod_drive_high();

    wait_seconds(0.00001);

    init_gpiod_in();
    
    uint32_t startTime = TIM4_CNT;
    uint32_t elapsedTime = 0;
    while (elapsedTime < _maxValue) { 
        // Calcule le temps écoulé à chaque itération
        elapsedTime = TIM4_CNT - startTime;
        compute_time(sensorValues, elapsedTime);
    }
    TIM4_CR1 &= ~TIM_CEN;  // Disable the timer
    
    // for (int i = 0; i < NB_QTR_SENSORS; i++) {
    //     printf("C%d:%d, ", i+1, sensorValues[i]);
    // }
    // printf("\n");

    //calibrate_time(sensorValues);
    *position = compute_position(sensorValues);
}
