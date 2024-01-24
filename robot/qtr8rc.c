
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
    GPIOD_MODER = REP_BITS(GPIOD_MODER, IR1_LED * 2, 2, GPIO_MODER_OUT);
    GPIOD_OTYPER &= ~(1 << IR1_LED);
    GPIOD_MODER = REP_BITS(GPIOD_MODER, IR2_LED * 2, 2, GPIO_MODER_OUT);
    GPIOD_OTYPER &= ~(1 << IR2_LED);
    GPIOD_MODER = REP_BITS(GPIOD_MODER, IR3_LED * 2, 2, GPIO_MODER_OUT);
    GPIOD_OTYPER &= ~(1 << IR3_LED);
    GPIOD_MODER = REP_BITS(GPIOD_MODER, IR4_LED * 2, 2, GPIO_MODER_OUT);
    GPIOD_OTYPER &= ~(1 << IR4_LED);
    GPIOD_MODER = REP_BITS(GPIOD_MODER, IR5_LED * 2, 2, GPIO_MODER_OUT);
    GPIOD_OTYPER &= ~(1 << IR5_LED);
    GPIOD_MODER = REP_BITS(GPIOD_MODER, IR6_LED * 2, 2, GPIO_MODER_OUT);
    GPIOD_OTYPER &= ~(1 << IR6_LED);
    GPIOD_MODER = REP_BITS(GPIOD_MODER, IR7_LED * 2, 2, GPIO_MODER_OUT);
    GPIOD_OTYPER &= ~(1 << IR7_LED);
    GPIOD_MODER = REP_BITS(GPIOD_MODER, IR8_LED * 2, 2, GPIO_MODER_OUT);
    GPIOD_OTYPER &= ~(1 << IR8_LED);
}

void init_gpiod_in(void) {
    GPIOD_MODER = REP_BITS(GPIOD_MODER, IR1_LED * 2, 2, GPIO_MODER_IN);
    GPIOD_OTYPER &= ~(1 << IR1_LED);
    GPIOD_MODER = REP_BITS(GPIOD_MODER, IR2_LED * 2, 2, GPIO_MODER_IN);
    GPIOD_OTYPER &= ~(1 << IR2_LED);
    GPIOD_MODER = REP_BITS(GPIOD_MODER, IR3_LED * 2, 2, GPIO_MODER_IN);
    GPIOD_OTYPER &= ~(1 << IR3_LED);
    GPIOD_MODER = REP_BITS(GPIOD_MODER, IR4_LED * 2, 2, GPIO_MODER_IN);
    GPIOD_OTYPER &= ~(1 << IR4_LED);
    GPIOD_MODER = REP_BITS(GPIOD_MODER, IR5_LED * 2, 2, GPIO_MODER_IN);
    GPIOD_OTYPER &= ~(1 << IR5_LED);
    GPIOD_MODER = REP_BITS(GPIOD_MODER, IR6_LED * 2, 2, GPIO_MODER_IN);
    GPIOD_OTYPER &= ~(1 << IR6_LED);
    GPIOD_MODER = REP_BITS(GPIOD_MODER, IR7_LED * 2, 2, GPIO_MODER_IN);
    GPIOD_OTYPER &= ~(1 << IR7_LED);
    GPIOD_MODER = REP_BITS(GPIOD_MODER, IR8_LED * 2, 2, GPIO_MODER_IN);
    GPIOD_OTYPER &= ~(1 << IR8_LED);
}


void gpiod_drive_high(void) { // make sensor line an output and drive high
    GPIOD_ODR |= 1 << IR1_LED;
    GPIOD_ODR |= 1 << IR2_LED;
    GPIOD_ODR |= 1 << IR3_LED;
    GPIOD_ODR |= 1 << IR4_LED;
    GPIOD_ODR |= 1 << IR5_LED;
    GPIOD_ODR |= 1 << IR6_LED;
    GPIOD_ODR |= 1 << IR7_LED;
    GPIOD_ODR |= 1 << IR8_LED;
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

void compute_time(int *sensor_values, uint32_t elapsed_time) {
    for (unsigned int led = 1; led <= NB_QTR_SENSORS; led++) {
        int tmp;
        if (led == 1)
            tmp = IR1_LED;
        else if (led == 2)
            tmp = IR2_LED;
        else 
            tmp = led;
        
        if (((GPIOD_IDR & (1 << tmp)) == 0) && (elapsed_time < sensor_values[led-1])){
            sensor_values[led-1] = elapsed_time;
        }
    }
}

int compute_position(int *sensor_values) {
    int sum = 0;
    for (int i = 0; i < NB_QTR_SENSORS; i++) {
        sum += sensor_values[i];
    }
    int rsl = 0;
    int c = 0;
    for (int i = 0; i < NB_QTR_SENSORS; i++) {
        rsl += sensor_values[i] * c;
        c += 1000;
    }
    return rsl/sum;
}

void calibrate_time(int *sensor_values) {
    for (int i = 0; i < NB_QTR_SENSORS; i++) {
        if (sensor_values[i] <= 20)
            sensor_values[i] = 0;
        else
            sensor_values[i] = 1;
    }
}

void qtr8rc_init(void) {
    init_gpio_qtr8rc();
	init_tim4_qtr8rc();
}

void qtr8rc_read(int* position) {
    GPIOD_BSRR |= 1 << ON_LED;
    int sensor_values[NB_QTR_SENSORS] = {_maxValue,_maxValue,_maxValue,_maxValue,_maxValue,_maxValue,_maxValue,_maxValue};
    TIM4_CNT = 0;
    TIM4_SR = 0;
    TIM4_CR1 = TIM_CEN;

    uint32_t elapsed_time = 0;
    init_gpiod_out();
    gpiod_drive_high();

    wait_seconds(0.00001);

    init_gpiod_in();
    uint32_t start_time = TIM4_CNT;
    elapsed_time = TIM4_CNT - start_time;
    while (elapsed_time < _maxValue) { 
    // Calcule le temps écoulé à chaque itération
        elapsed_time = TIM4_CNT - start_time;
        compute_time(sensor_values, elapsed_time);
        // printf("-> %d\n", elapsed_time);
    }
    TIM4_CR1 &= ~TIM_CEN;  // Disable the timer

    //calibrate_time(sensor_values);
    GPIOD_BSRR |= 1 << (ON_LED + 16);
    *position = compute_position(sensor_values);
}
