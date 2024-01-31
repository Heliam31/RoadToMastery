
#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/nvic.h>
#include <stm32f4/exti.h>
#include <stm32f4/syscfg.h>
#include <stm32f4/tim.h>
#include <stm32f4/adc.h>

#include "qtr8rc.h"
#include "utils.h"
#include "motor_driver.h"


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

// GLOBAL
int maxValue = 4000;
int maximumValues[NB_QTR_SENSORS];
int minimumValues[NB_QTR_SENSORS];
int meanValues[NB_QTR_SENSORS];

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

void compute_time(int *sensorValues, const uint32_t elapsedTime) {
    for (unsigned int i = 0; i < NB_QTR_SENSORS; i++) {
        if (((GPIOD_IDR & (1 << IR_LEDS[i])) == 0) && (elapsedTime < sensorValues[i])) {
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
    qtr8rc_reset_calibrate();
}

void qtr8rc_read(int *sensorValues) {
    TIM4_CNT = 0;
    TIM4_SR = 0;
    TIM4_CR1 = TIM_CEN;

    init_gpiod_out();
    gpiod_drive_high();
    wait_seconds(0.0001);
    init_gpiod_in();
    
    uint32_t startTime = TIM4_CNT;
    uint32_t elapsedTime = 0;
    while (elapsedTime < maxValue) {
        // Calcule le temps écoulé à chaque itération
        elapsedTime = TIM4_CNT - startTime;
        compute_time(sensorValues, elapsedTime);
    }
    TIM4_CR1 &= ~TIM_CEN;  // Disable the timer
}

void qtr8rc_reset_calibrate(void) {
    for (int i = 0; i < NB_QTR_SENSORS; i++) {
        maximumValues[i] = maxValue; 
        minimumValues[i] = 0;
    }
}

void qtr8rc_calibrate(void) {
    // qtr8rc_reset_calibrate();

    // on black line
    // qtr8rc_read(maximumValues);

    // move on white line 
    // set_speed_left(10);
    // set_speed_right(10);

    // on white line
    // qtr8rc_read(minimumValues);

    // wait_seconds(0.01);

    int sensorValues[NB_QTR_SENSORS];

    for (int k = 0; k < 13; k++) {
        set_speed_left(-25);
        set_speed_right(25);
        //wait_seconds(1);

        qtr8rc_read(sensorValues);

        for (int i = 0; i < NB_QTR_SENSORS; i++) {
            if ((minimumValues[i] < sensorValues[i]) && (sensorValues[i] < maximumValues[i])) {
                minimumValues[i] = sensorValues[i];
            } 
            if ((maximumValues[i] > sensorValues[i]) && (sensorValues[i] > minimumValues[i])) {
                maximumValues[i] = sensorValues[i];
            }
        }
    }

    set_speed_left(0);
    set_speed_right(0);

    // for (int i = 0; i < NB_QTR_SENSORS; i++) {
    //     meanValues[i] = (maximumValues[i] + minimumValues[i]) / 2;
    // }
}

void qtr8rc_read_calibrated(int *position) {
    // printf("[%s] - entry\n", __func__);

    int sensorValues[NB_QTR_SENSORS];
    for (int i = 0; i < NB_QTR_SENSORS; i++) {
        sensorValues[i] = maximumValues[i];
    }
    
    qtr8rc_read(sensorValues);

    *position = compute_position(sensorValues);

    // for (int i = 0; i < NB_QTR_SENSORS; i++) {
    //     printf("[%d]: %d\n", i, sensorValues[i]);
    // }
    // printf("%d\n", *position);
}
