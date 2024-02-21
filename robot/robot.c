#include "pid.h"
#include "qtr8rc.h"
#include "motor_driver.h"
#include "color_sens.h"
#include "sonar.h"

#include "utils.h"


#define GREEN_LED 12
#define BUTTON 0

// UTILS 
void _robot_delay(int cycles) {
    for(int i = 0; i < cycles; i++) NOP;
}

void robot_wait_seconds(float seconds) {
    int cycles = seconds*APB1_CLK;
    _robot_delay(cycles);
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
    init_gpio_led();
    init_gpio_button();

    qtr8rc_init();
    motor_init();
    sonar_init();
    color_init();
}


void calibrate(void) {
    printf("Calibrating...\n");

    set_speed_left(-27);
    set_speed_right(27);

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
    printf("\n\n\nReady to calibrate !\n");

    RCC_AHB1ENR |= RCC_GPIOAEN;
    RCC_AHB1ENR |= RCC_GPIOBEN;
	RCC_AHB1ENR |= RCC_GPIOCEN;
	RCC_AHB1ENR |= RCC_GPIODEN;

	RCC_APB1ENR |= RCC_TIM2EN;
	RCC_APB1ENR |= RCC_TIM3EN;
    RCC_APB1ENR |= RCC_TIM4EN;
	RCC_APB1ENR |= RCC_TIM5EN;

    RCC_APB2ENR |= RCC_ADC1EN;

    init();
    
    turn_on(GREEN_LED);
    wait_start();
    turn_off(GREEN_LED);

    calibrate();

    turn_on(GREEN_LED);
    wait_start();
    turn_off(GREEN_LED);
    printf("Start !\n");

    int stop = 0;
    int stop_b = 0;
    int sonarUse_b = 0;
    int isR = 0, isG = 0, isB = 0;
    int position = 0;
    int junctions[2] = {0};
    int motorLeftSpeed = 0;
    int motorRightSpeed = 0;
    unsigned int* distance = 0;

    while(1){
        qtr8rc_read_calibrated(&position, junctions);
        color_read(&isG,&isB,&isR);
        sonar_read(&distance);
        
        if (sonarUse_b) {
            sonar_read(&distance);
            if (distance <= 8) {
                set_speed_left(0);
                set_speed_right(0);
                stop_b = 1;
            }
            // printf("->%d\n", distance);
        }
        if (junctions[0] | junctions[1]) {
            set_speed_left(0);
            set_speed_right(0);
            stop_b = 1;
        } 
        
        if (!stop_b) {
            compute_motor_speed(&motorLeftSpeed, &motorRightSpeed, position);
            set_speed_left(motorLeftSpeed);
            set_speed_right(motorRightSpeed);
        }

        if ((GPIOA_IDR & (1 << SW_USER)) != 0) {
            stop_b = 0;
        }
        // printf("%d | %d | %d\n",isR, isG, isB);
        sonarUse_b = !sonarUse_b;
    }
}
