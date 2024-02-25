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
#define SONAR_TIME 20000
#define SONAR_WAIT_PSC PSC_42


// SONAR VALUES
const int _sonarTimeout = SONAR_TIME;
uint32_t sonarValue = _sonarTimeout;
uint32_t sonarMicros = 0;

// =============== INIT ===============
void init_gpioc_sonar(void) {
    // Configure TRIG_PIN as output
    GPIOC_MODER = REP_BITS(GPIOC_MODER, TRIG_PIN * 2, 2, GPIO_MODER_OUT);
    GPIOC_OTYPER &= ~(1 << TRIG_PIN);

    GPIOC_MODER = REP_BITS(GPIOC_MODER, ECHO_PIN * 2, 2, GPIO_MODER_IN);
    // GPIOC_PUPDR = REP_BITS(GPIOC_PUPDR, ECHO_PIN * 2, 2, GPIO_PUPDR_PU);
    GPIOC_OTYPER &= ~(1 << ECHO_PIN);

}

void init_tim2_sonar(void) {
    TIM2_CR1 = 0;
	TIM2_PSC = SONAR_WAIT_PSC-1;
	TIM2_ARR = SONAR_TIME;
	TIM2_EGR = TIM_UG;
	TIM2_SR = 0;
	TIM2_CR1 = TIM_CEN;
}

void sonar_init(void) {
    init_gpioc_sonar();
    init_tim2_sonar();
}

void _read() {
    // Sending 10us pulse to
    GPIOC_BSRR = (1 << (TRIG_PIN + 16)); 
    delay_us(2);

    GPIOC_BSRR = (1 << (TRIG_PIN));
    delay_us(10);

    GPIOC_BSRR = (1 << (TRIG_PIN + 16));
    delay_us(30);

    // Start the timer for the measure
    TIM2_CNT = 0;
    TIM2_CR1 = TIM_CEN;
    TIM2_SR = 0;

    sonarMicros = TIM2_CNT;
    while(!((GPIOC_IDR & (1 << ECHO_PIN)) != 0) && (TIM2_CNT - sonarMicros) < _sonarTimeout);
    sonarMicros = TIM2_CNT;
    while(((GPIOC_IDR & (1 << ECHO_PIN)) != 0) && (TIM2_CNT - sonarMicros) < _sonarTimeout);
    
    if (TIM2_CNT < sonarMicros ) {
        sonarValue = _sonarTimeout;
    } else {
        sonarValue = TIM2_CNT - sonarMicros;
    }
    
    // sonarValue = (TIM2_CNT < sonarMicros) ? _sonarTimeout : (TIM2_CNT - sonarMicros);

    TIM2_CR1 &= ~TIM_CEN; 
}

void sonar_read(unsigned int* distance) {
    _read();
    *distance =  (sonarValue / 2) * 0.034;

}