
#include "utils.h"


// ==================== MATH ====================
/*
** @brief: Return the absolute value of x
** @param[in]: x: the number
** @retval: the absolute value of x
*/
int abs(int x) {
    if (x < 0) {
        return -x;
    }
    return x;
}

/*
** @brief: Return the minimum between x and y
** @param[in]: x: the first number
** @param[in]: y: the second number
** @retval: the minimum between x and y
*/
int min(int x, int y) {
    if (x < y) {
        return x;
    }
    return y;
}

/*
** @brief: Return the maximum between x and y
** @param[in]: x: the first number
** @param[in]: y: the second number
** @retval: the maximum between x and y
*/
int max(int x, int y) {
    if (x > y) {
        return x;
    }
    return y;
}

/*
** @biref: Return x clamped between min and max
** @param[in]: x: the number
** @param[in]: min: the minimum value
** @param[in]: max: the maximum value
** @retval: x clamped between min and max
*/
int clamp(int x, int min, int max) {
    if (x < min) {
        return min;
    }
    if (x > max) {
        return max;
    }
    return x;
}

/*
** @biref: Return x clamped between 0 and 255
** @param[in]: x: the number
** @retval: x clamped between 0 and 255
*/
int sign(int x) {
    if (x < 0) {
        return -1;
    }
    if (x > 0) {
        return 1;
    }
    return 0;
}

// ==================== COMM. ====================

// ==================== STM32 ====================
// --------------- sync ---------------
void init_timer_sync(void) {
    TIM2_CR1 = 0;
	TIM2_PSC = SYNC_PSC-1;
	TIM2_ARR = SYNC_PERIOD;
	TIM2_EGR = TIM_UG;
	TIM2_CR1 |= TIM_CEN | TIM_ARPE;
	TIM2_SR = 0;
}

void sync(void) {
    while(((TIM2_SR & TIM_UIF) == 0)) NOP;
	TIM2_SR &= ~TIM_UIF;
    return;
}

// --------------- led ---------------
void init_gpio_led(void) {
	GPIOD_MODER = REP_BITS(GPIOD_MODER, GREEN_LED*2, 2, GPIO_MODER_OUT);
	GPIOD_OTYPER &= ~(1<<GREEN_LED);
}

void turn_on(int led) {
    GPIOD_BSRR = 1 << led;
}

void turn_off(int led) {
    GPIOD_BSRR = 1 << (led + 16);
}

// --------------- button ---------------
void init_gpio_button(void) {
    GPIOA_MODER = REP_BITS(GPIOA_MODER, BUTTON*2, 2, GPIO_MODER_IN);
	GPIOA_PUPDR = REP_BITS(GPIOA_PUPDR, BUTTON*2, 2, GPIO_PUPDR_PD);
}

/*
** @brief: Wait the pushed button 
*/
void wait_button(void){
    int start = 0;
    while(!start) {
        if ((GPIOA_IDR & (1 << SW_USER)) != 0)  {
            start = 1;
        }
    }
}

// --------------- direction ---------------
void display_direction(Direction direction){
    switch (direction) {
    case BACK:
        printf("Direction:BACK\n");
        break;
    case FRONT:
        printf("Direction:FRONT\n");
        break;            
    case LEFT:
        printf("Direction:LEFT\n");
        break;
    case RIGHT:
        printf("Direction:RIGHT\n");
        break;
    default:
        break;
    }
}
