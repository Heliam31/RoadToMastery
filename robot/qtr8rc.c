
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
const int IR_LEDS[8] = {IR1_LED, IR2_LED, IR3_LED, IR4_LED, IR5_LED, IR6_LED, IR7_LED, IR8_LED};

// TIM4
#define T 0.0025
#define WAIT_PSC 2
// #define WAIT_qtr8rc_delay ((APB1_CLK/WAIT_PSC)/400)
#define WAIT_qtr8rc_delay (T*APB1_CLK)/WAIT_PSC


// MISC
const int PERIOD = WAIT_qtr8rc_delay;
const int _maxValue = PERIOD;

int calMaxValues[NB_QTR_SENSORS] = {0};
int calMinValues[NB_QTR_SENSORS] = {0};
uint16_t _lastPosition = 0;

// UTILS
void _qtr8rc_delay(int cycles) {
    for(int i = 0; i < cycles; i++) NOP;
}

void qtr8rc_wait_seconds(float seconds) {
    int cycles = seconds*APB1_CLK;
    _qtr8rc_delay(cycles);
}
// DEBUG
void display_calMaxValues(){
    printf("\nMax - ");
    for (int i =0; i < NB_QTR_SENSORS; i++) {
        printf("[%d]:%d, ", i+1, calMaxValues[i]);
    }
    printf("\n");
}

void display_calMinValues(){
    printf("\nMin - ");
    for (int i =0; i < NB_QTR_SENSORS; i++) {
        printf("[%d]:%d, ", i+1, calMinValues[i]);
    }
    printf("\n");
}

void _display_sensorValues(int* sensorValues){
    printf("\nsensValues: \n");
    for (int i =0; i < NB_QTR_SENSORS; i++) {
        printf("[%d]:%d, ", i+1, sensorValues[i]);
    }
    printf("\n");
}

// INIT
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

void qtr8rc_init(void) {
    init_gpio_qtr8rc();
	init_tim4_qtr8rc();

    /*Init minSensorValues*/
    for (int i = 0; i < NB_QTR_SENSORS; i++) {
        calMinValues[i] = _maxValue;
        // printf("[%d], ",calMinValues[i]);
        qtr8rc_wait_seconds(0.01);
    }
}

// FUNCTIONS
int compute_position(int *sensorValues) {
    
    // _display_sensorValues(sensorValues);
    
    int onLine = 0;
    uint32_t avg = 0; // this is for the weighted total
    uint16_t sum = 0; // this is for the denominator, which is <= 64000

    for (uint8_t i = 0; i < NB_QTR_SENSORS; i++) {
        uint16_t value = sensorValues[i];

        // keep track of whether we see the line at all
        if (value > 200) {
            onLine = 1;
        }

        // only average in values that are above a noise threshold
        if (value > 50) {
            avg += (uint32_t)value * (i * 1000);
            sum += value;
        }
    }

    if (!onLine) {
        // If it last read to the left of center, return 0.
        if (_lastPosition < (NB_QTR_SENSORS - 1) * 1000 / 2) {
            return 0;
        }
        // If it last read to the right of center, return the max.
        else {
            return (NB_QTR_SENSORS - 1) * 1000;
        }
    }

    _lastPosition = avg / sum;
    return _lastPosition;
}

void _read(int* sensorValues) {
    GPIOD_BSRR = 1 << ON_LED;
    
    init_gpiod_out();
    gpiod_drive_high();

    qtr8rc_wait_seconds(0.00001);

    TIM4_CNT = 0;
    TIM4_CR1 = TIM_CEN;
    TIM4_SR = 0;

    uint32_t startTime = TIM4_CNT;
    uint32_t elapsedTime = 0;
    uint32_t now = 0;

    init_gpiod_in();

    while (((now=TIM4_CNT) + elapsedTime) < _maxValue) {
        // printf("elapsedT: %d | _max: %d\n",elapsedTime, _maxValue);
        // Calcule time at each iteration
        elapsedTime = now - startTime;

        // Compute each led's value
        for (unsigned int i = 0; i < NB_QTR_SENSORS; i++) {
            if (((GPIOD_IDR & (1 << IR_LEDS[i])) == 0) && (elapsedTime < sensorValues[i])){
                sensorValues[i] = elapsedTime;
            }
        }
    }
    TIM4_CR1 &= ~TIM_CEN;  // Disable the timer

    GPIOD_BSRR = 1 << (ON_LED + 16);
}

void qtr8rc_calibrate() {
    int sensorValues[NB_QTR_SENSORS] = {_maxValue,_maxValue,_maxValue,_maxValue,_maxValue,_maxValue,_maxValue,_maxValue};

    GPIOD_BSRR = 1 << ON_LED;

    // _read(sensorValues);
    init_gpiod_out();
    gpiod_drive_high();

    qtr8rc_wait_seconds(0.00001);

    TIM4_CNT = 0;
    TIM4_CR1 = TIM_CEN;
    TIM4_SR = 0;

    uint32_t startTime = TIM4_CNT;
    uint32_t elapsedTime = 0;
    uint32_t now = 0;
    
    init_gpiod_in();

    while (((now=TIM4_CNT) + elapsedTime) < _maxValue) {
        // Calcule time at each iteration
        elapsedTime = now - startTime;
        // Compute each led's value
        for (unsigned int i = 0; i < NB_QTR_SENSORS; i++) {
            if (((GPIOD_IDR & (1 << IR_LEDS[i])) == 0) && (elapsedTime < sensorValues[i])){
                if (elapsedTime < calMinValues[i])
                    calMinValues[i] = elapsedTime;
                if (elapsedTime > calMaxValues[i])
                    calMaxValues[i] = elapsedTime;
            }
        }
    }
    TIM4_CR1 &= ~TIM_CEN;  // Disable the timer

    GPIOD_BSRR = 1 << (ON_LED + 16);
}

void qtr8rc_read_calibrated(int* position) {
    int sensorValues[NB_QTR_SENSORS] = {_maxValue,_maxValue,_maxValue,_maxValue,_maxValue,_maxValue,_maxValue,_maxValue};

    _read(sensorValues);


    uint16_t calmin = 0, calmax = 0;
    for (unsigned int i = 0; i < NB_QTR_SENSORS; i++) {

        calmin = calMinValues[i];
        calmax = calMaxValues[i];

        // Appliquer la formule de calibration
        int denominator = calmax - calmin;
        int value = 0;

        if (denominator != 0) {
            value = ((sensorValues[i] - calmin) * 1000) / denominator;
        }

        if (value < 0) { value = 0; } 
        else if (value > 1000) { value = 1000; }

        sensorValues[i] = value;
    }

    _display_sensorValues(sensorValues);
    *position = compute_position(sensorValues);
}