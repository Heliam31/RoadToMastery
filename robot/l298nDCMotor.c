
#include <stm32f4/gpio.h>
#include <stm32f4/io.h>
#include <stm32f4/tim.h>
#include <tinyprintf.h>

#include "l298nDCMotor.h"
#include "utils.h"


// GPIOA pins 0,1,2,3,4,5 are connected to L298N module
// Motor A
#define enA 0
#define in1 1
#define in2 2
// Motor B
#define enB 3
#define in3 6
#define in4 7

// PWM frequency is 1kHz
#define N 1
#define PSC 6000
#define PERIOD (N*AHB_CLK/PSC)

void init_gpioa(void) {
    // Set GPIOA pins 0,1,2,3,6,7 to output
    GPIOA_MODER = REP_BITS(GPIOA_MODER, enA*2,2,GPIO_MODER_ALT);
    GPIOA_MODER = REP_BITS(GPIOA_MODER, in1*2,2,GPIO_MODER_OUT);
    GPIOA_MODER = REP_BITS(GPIOA_MODER, in2*2,2,GPIO_MODER_OUT);
    
    GPIOA_MODER = REP_BITS(GPIOA_MODER, enB*2,2,GPIO_MODER_ALT);
    GPIOA_MODER = REP_BITS(GPIOA_MODER, in3*2,2,GPIO_MODER_OUT);
    GPIOA_MODER = REP_BITS(GPIOA_MODER, in4*2,2,GPIO_MODER_OUT);

    GPIOA_AFRL = REP_BITS(GPIOA_AFRL, enA*4, 4, 1);
    GPIOA_OTYPER &= ~(1<<in1); 
    GPIOA_OTYPER &= ~(1<<in2); 
    GPIOA_AFRL = REP_BITS(GPIOA_AFRL, enB*4, 4, 1);
    GPIOA_OTYPER &= ~(1<<in3); 
    GPIOA_OTYPER &= ~(1<<in4); 
}

void init_timer2(void) {
    // Initialize TIM2 clock
    TIM2_CR1 = 0;
    TIM2_PSC = PSC - 1;
    TIM2_ARR = PERIOD;

    // Set TIM2 capture/compare
    TIM2_CCR1 = 0;
    TIM2_CCR2 = 0;
    TIM2_CCMR1 = TIM_OC2M_PWM1;
    TIM2_CCMR2 = TIM_OC2M_PWM1;

    // Set TIM2 capture/compare enable register
    TIM2_CCER = TIM_CC1E | TIM_CC2E;

    // Set TIM2 event generation register
	TIM3_EGR = TIM_UG;

    // Set TIM2 counter enable
    TIM2_CR1 = TIM_CEN | TIM_ARPE;
    TIM2_SR = 0;
}

void l298nDCMotor_init(void) {
    init_gpioa();
    init_timer2();
}

void l298nDCMoto_set_speed(const int speedA, const int speedB) {
    if (speedA > 0) { // Forward
        GPIOA_BSRR = (1 << in1);
        GPIOA_BSRR = (1 << (in2 +16));
    } else if (speedA < 0) { // Backward
        GPIOA_BSRR = (1 << (in1 + 16));
        GPIOA_BSRR = (1 << in2);
    } else { // Motor OFF
        GPIOA_BSRR = (1 << (in1 + 16));
        GPIOA_BSRR = (1 << (in2 + 16));
    }
    if (speedB > 0) {
        GPIOA_BSRR = (1 << in3);
        GPIOA_BSRR = (1 << (in4 + 16));
    } else if (speedB < 0) {
        GPIOA_BSRR = (1 << (in3 + 16));
        GPIOA_BSRR = (1 << in4);
    } else {
        GPIOA_BSRR = (1 << (in3 + 16));
        GPIOA_BSRR = (1 << (in4 + 16));
    }

    TIM2_CCR1 = abs(speedA)*PERIOD/1000;
    TIM2_CCR2 = abs(speedB)*PERIOD/1000;
}
