#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/nvic.h>
#include <stm32f4/exti.h>
#include <stm32f4/syscfg.h>
#include <stm32f4/tim.h>
#include "utils.h"

#define TRIG_PIN 14
#define ECHO_PIN 15

// TIM2
#define SONAR_T 0.02
#define SONAR_WAIT_PSC 65
#define WAIT_qtr8rc_delay (SONAR_T*APB1_CLK)/SONAR_WAIT_PSC
const int sonarPeriod = WAIT_qtr8rc_delay;


// SONAR VALUES
const int _sonarTimeout = sonarPeriod;
uint32_t sonarValue = _sonarTimeout;
unsigned int data = 0;
double time;

// =============== INIT ===============
void init_gpioc_sonar(void) {
    // Configure TRIG_PIN as output
    GPIOC_MODER = REP_BITS(GPIOC_MODER, TRIG_PIN * 2, 2, GPIO_MODER_OUT);
    GPIOC_OTYPER &= ~(1 << TRIG_PIN);

    GPIOC_MODER = REP_BITS(GPIOC_MODER, ECHO_PIN * 2, 2, GPIO_MODER_IN);
    GPIOC_OTYPER &= ~(1 << ECHO_PIN);
    // GPIOC_PUPDR = REP_BITS(GPIOC_PUPDR, ECHO_PIN * 2, 2, GPIO_PUPDR_PU);

    // GPIOC_BSRR = 0x00000000;// Setting trig pin to low to initialize the module
}

void init_tim2_sonar(void) {
    TIM2_CR1 = 0;
	TIM2_PSC = SONAR_WAIT_PSC-1;
	TIM2_ARR = sonarPeriod;
	TIM2_EGR = TIM_UG;
	TIM2_SR = 0;
	TIM2_CR1 = TIM_CEN;
}

void sonar_init(void) {
    init_gpioc_sonar();
    init_tim2_sonar();
}

// =============== UTILS ===============
void TIM2_Delay(float delay){
    TIM2_ARR = (int)(delay*APB1_CLK/SONAR_WAIT_PSC); // Total period of the timer
    TIM2_CNT = 0;
    TIM2_SR &= ~TIM_UIF; //Reset the update interrupt flag
    TIM2_CR1 = TIM_CEN; //Start the Timer

    while(!(TIM2_SR & TIM_UIF)){} //Polling the update interrupt flag
	TIM2_ARR = sonarPeriod;
    TIM2_CR1 &= ~TIM_CEN;
}
void sonar_delay(int cycles) {
    for(int i = 0; i < cycles; i++) NOP;
}

// void sonar_wait_seconds(float seconds) {
//     int cycles = seconds*APB1_CLK;
//     sonar_delay(cycles);
// }


void _read() {
    uint32_t sonarElapsedTime = 0;

    // Sending 10us pulse to
    GPIOC_BSRR = (1 << (TRIG_PIN + 16)); 
    TIM2_Delay(0.000002);

    GPIOC_BSRR = (1 << (TRIG_PIN));
    TIM2_Delay(0.00001);

    GPIOC_BSRR = (1 << (TRIG_PIN + 16));
    TIM2_Delay(0.00004);

    // Start the timer for the measure
    TIM2_CNT = 0;
    TIM2_CR1 = TIM_CEN;
    TIM2_SR = 0;

    uint32_t sonarNow = 0;
    uint32_t sonarStartTime = TIM2_CNT;
    while (((sonarNow=TIM2_CNT) + sonarElapsedTime) <= _sonarTimeout) {

        // Compute time at each iteration
        sonarElapsedTime = sonarNow - sonarStartTime;
        if (((GPIOC_IDR & (1 << ECHO_PIN)) != 0) && (sonarElapsedTime < _sonarTimeout)){
            sonarValue = sonarElapsedTime;
        }
    }

    TIM2_CR1 &= ~TIM_CEN; 
    // printf("\n-> %d\n",sonarValue);
}

void sonar_read(unsigned int* distance) {
    _read();
    
    double timeInSeconds = sonarValue / sonarPeriod;
    *distance = ((((sonarValue*100000) / sonarPeriod) * 340)/2)/100000; 

}