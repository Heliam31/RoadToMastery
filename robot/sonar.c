#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/nvic.h>
#include <stm32f4/exti.h>
#include <stm32f4/syscfg.h>
#include <stm32f4/tim.h>

#define TRIG_PIN 14
#define ECHO_PIN 15

// TIM2
#define SONAR_T 0.001
#define SONAR_WAIT_PSC 2
#define WAIT_qtr8rc_delay (SONAR_T*APB1_CLK)/SONAR_WAIT_PSC

const int SONAR_PERIOD = WAIT_qtr8rc_delay;

unsigned int data = 0;
double time;

// =============== INIT ===============
void init_gpioc_sonar(void) {
    // Configure TRIG_PIN as output
    GPIOC_MODER = REP_BITS(GPIOC_MODER, TRIG_PIN * 2, 2, GPIO_MODER_OUT);
    GPIOC_OTYPER &= ~(1 << TRIG_PIN);

    GPIOC_MODER = REP_BITS(GPIOC_MODER, ECHO_PIN * 2, 2, GPIO_MODER_IN);
    GPIOC_PUPDR = REP_BITS(GPIOC_PUPDR, ECHO_PIN * 2, 2, GPIO_PUPDR_PU);

    GPIOC_BSRR = 0x00000000;// Setting trig pin to low to initialize the module
}

void init_tim2_sonar(void) {
    TIM2_CR1 = 0;
	TIM2_PSC = SONAR_WAIT_PSC-1;
	TIM2_ARR = SONAR_PERIOD;
	TIM2_EGR = TIM_UG;
	TIM2_SR = 0;
	TIM2_CR1 |= TIM_CEN | TIM_ARPE;
}

void sonar_init(void) {
    init_gpioc_sonar();
    init_tim2_sonar();
}

void TIM2_us_Delay(float delay){
    TIM2_ARR = (int)(delay*APB1_CLK/SONAR_WAIT_PSC); // Total period of the timer
    TIM2_CNT = 0;
    TIM2_CR1 = TIM_CEN; //Start the Timer
    TIM2_SR &= ~TIM_UIF; //Reset the update interrupt flag

    while(!(TIM2_SR & TIM_UIF)){} //Polling the update interrupt flag
	TIM2_ARR = SONAR_PERIOD;
}

void _read() {
    // Sending 10us pulse to
    GPIOC_BSRR = (1 << (TRIG_PIN + 16)); 
    TIM2_us_Delay(0.000002);
    GPIOC_BSRR = (1 << (TRIG_PIN));
    TIM2_us_Delay(0.00001);
    GPIOC_BSRR = (1 << (TRIG_PIN + 16));
    TIM2_us_Delay(0.00003);

    while (GPIOC_IDR & (1 << ECHO_PIN)){data += 1;}

    TIM2_CR1 &= ~TIM_CEN;
}

void sonar_read (unsigned int* distance) {
    _read();

    if (data > 0) {
        time = data * (0.0625 * 0.000001);
        *distance = ((time*340)/2)*100;
        printf("-->%d\n",*distance);
    }

    data = 0;
}