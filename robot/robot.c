
#include "pid.h"
#include "qtr8rc.h"
#include "motor_driver.h"
#include "utils.h"


#define GREEN_LED 12
#define BUTTON 0

// TIMER POUR SYNC
#define N 0.1
#define PSC 128 // ->1s  // 8->0.01s
#define PERIOD (N*APB1_CLK)/PSC

// UTILS 
void _robot_delay(int cycles) {
    for(int i = 0; i < cycles; i++) NOP;
}

void robot_wait_seconds(float seconds) {
    int cycles = seconds*APB1_CLK;
    _robot_delay(cycles);
}

void init_timer_sync(void) {
    TIM2_CR1 = 0;
	TIM2_PSC = PSC-1;
	TIM2_ARR = PERIOD;
	TIM2_EGR = TIM_UG;
	TIM2_CR1 |= TIM_CEN | TIM_ARPE;
	TIM2_SR = 0;
}

void init_gpio_led(void) {
	GPIOD_MODER = REP_BITS(GPIOD_MODER, GREEN_LED*2, 2, GPIO_MODER_OUT);
	GPIOD_OTYPER &= ~(1<<GREEN_LED);
}

void init_gpio_button(void) {
    GPIOA_MODER = REP_BITS(GPIOA_MODER, BUTTON*2, 2, GPIO_MODER_IN);
	GPIOA_PUPDR = REP_BITS(GPIOA_PUPDR, BUTTON*2, 2, GPIO_PUPDR_PD);
}

void init(void) {
    init_timer_sync();
    init_gpio_led();
    init_gpio_button();
    qtr8rc_init();
    motor_init();
}


void calibrate(void) {
    printf("Calibrating...\n");

    set_speed_left(-18);
    set_speed_right(18);

    for (size_t i = 0; i < 50; i++) {
        qtr8rc_calibrate();
        robot_wait_seconds(0.2);
    }

    display_calMinValues();
    display_calMaxValues();

    printf("Ready !\n");

    set_speed_left(0);
    set_speed_right(0);

    robot_wait_seconds(1);
}

int main(void) {
    printf("\n\n\nInitialization...\n");

    RCC_AHB1ENR |= RCC_GPIOAEN;
	RCC_AHB1ENR |= RCC_GPIODEN;
	RCC_AHB1ENR |= RCC_GPIOCEN;

    RCC_APB1ENR |= RCC_TIM4EN;
	RCC_APB1ENR |= RCC_TIM3EN;
	RCC_APB1ENR |= RCC_TIM2EN;

    RCC_APB2ENR |= RCC_ADC1EN;

    init();
    
    int position = 0;
    int junctions[2] = {0};
    int motorLeftSpeed = 25;
    int motorRightSpeed = 25;

    int stop = 0;
    
    turn_on(GREEN_LED);
    wait_start();
    turn_off(GREEN_LED);

    calibrate();

    turn_on(GREEN_LED);
    wait_start();
    turn_off(GREEN_LED);
    printf("Start !\n");

    while(1){
        qtr8rc_read_calibrated(&position, junctions);
        if (junctions[0] | junctions[1]) {
            set_speed_left(0);
            set_speed_right(0);
            stop = 1;
        } else if (!stop) {
            compute_motor_speed(&motorLeftSpeed, &motorRightSpeed, position);
            set_speed_left(motorLeftSpeed);
            set_speed_right(motorRightSpeed);
        }
        if ((GPIOA_IDR & (1 << SW_USER)) != 0) {
            stop = 0;
        }
    }
}
