
#include "utils.h"

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


// ==================== STM32 ====================
void sync(void) {
    while(((TIM3_SR & TIM_UIF) == 0)) NOP;
	TIM2_SR &= ~TIM_UIF;
    return;
}

void turn_on(int led) {
    GPIOD_BSRR = 1 << led;
}

void turn_off(int led) {
    GPIOD_BSRR = 1 << (led + 16);
}

/*
** @brief: Wait the pushed button 
*/
void wait_start(void){
    int start = 0;
    while(!start) {
        if ((GPIOA_IDR & (1 << SW_USER)) != 0)  {
            start = 1;
        }
    }
}
