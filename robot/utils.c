
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
/*
**  @brief: Configure TIM6 to generate a delay in microsecond
**  @param: None
**  @return: None
*/
void init_tim6(void) {
    TIM6_CR1 &= ~TIM_CEN; // Stop the timer
    TIM6_PSC = PSC_42000-1; // 1kHz -> 1000 ticks per second -> 1 tick = 1ms
    TIM6_SR &= ~TIM_UIF; // Clear the update event flag
}

/*
**  @brief: Delay the program for a given number of millisecond
**  @param: ms[in]: the number of millisecond to wait
**  @return: None
*/
void delay_ms(int ms) {
    TIM6_CR1 &= ~TIM_CEN; // Stop the timer
    TIM6_PSC = PSC_42000-1;
    TIM6_SR &= ~TIM_UIF; // Clear the update event flag
    TIM6_ARR = ms; // Set the auto-reload value
    // printf("ARR=%d\n", TIM6_ARR);
    TIM6_CR1 = TIM_CEN; // Start the timer
    while((TIM6_SR & TIM_UIF) == 0);// printf("%d\n", TIM6_CNT); // Wait for the update event
    // printf("END TIM6 MS\n");
    TIM6_CR1 &= ~TIM_CEN; // Stop the timer
    TIM6_SR &= ~TIM_UIF; // Clear the update event flag
}

/*
**  @brief: Delay the program for a given number of microsecond
**  @param: us[in]: the number of microsecond to wait
**  @return: None
*/
void delay_us(int us) { 
    TIM6_CR1 &= ~TIM_CEN; // Stop the timer
    TIM6_PSC = PSC_42-1;
    TIM6_SR &= ~TIM_UIF; // Clear the update event flag
    TIM6_ARR = us; // Set the auto-reload value
    // printf("ARR=%d\n", TIM6_ARR);
    TIM6_CR1 = TIM_CEN; // Start the timer
    while((TIM6_SR & TIM_UIF) == 0); // Wait for the update event
    // printf("END TIM6 US\n");
    TIM6_CR1 &= ~TIM_CEN; // Stop the timer
    TIM6_SR &= ~TIM_UIF; // Clear the update event flag
}

/*
*  @brief: Configure TIM7 to generate a delay in miliseconds
**  @param: None
**  @return: None
*/
void init_tim7(void) {
    TIM7_CR1 &= ~TIM_CEN; // Stop the timer
    TIM7_PSC = PSC_42000-1; // 1kHz -> 1000 ticks per second -> 1 tick = 1ms
    TIM7_ARR = 100; // 500 ms
    TIM7_EGR = TIM_UG; // RESET TIMER
    TIM7_SR &= ~TIM_UIF; // Clear the update event flag
}

void start_sync(int hyperperiod) {
    TIM7_ARR = hyperperiod;
    TIM7_CNT = 0; // Reset the counter
    TIM7_CR1 = TIM_CEN; // Start the timer
    TIM7_SR = 0; // Clear the update event flag
}

// Wait to sync
void sync(void) {
    while((TIM7_SR & TIM_UIF) == 0) NOP;
    TIM7_SR &= ~TIM_UIF;
}

// --------------- led ---------------
void led_init(int led) {
	GPIOD_MODER = REP_BITS(GPIOD_MODER, led*2, 2, GPIO_MODER_OUT);
	GPIOD_OTYPER &= ~(1<<led);
    delay_us(200);
}

void led_turn_on(int led) {
    GPIOD_BSRR = 1 << led;
    delay_us(200);
}

void led_turn_off(int led) {
    GPIOD_BSRR = 1 << (led + 16);
}

void led_clear_all(void) {
    led_turn_off(GREEN_LED);
    led_turn_off(ORANGE_LED);
    led_turn_off(RED_LED);
    led_turn_off(BLUE_LED);
}

// --------------- button ---------------
void button_init(int button) {
    GPIOA_MODER = REP_BITS(GPIOA_MODER, button*2, 2, GPIO_MODER_IN);
	GPIOA_PUPDR = REP_BITS(GPIOA_PUPDR, button*2, 2, GPIO_PUPDR_PD);
}

/*
** @brief: Wait the pushed button 
*/
void button_wait(int button){
    while((GPIOA_IDR & (1 << button)) == 0) NOP;
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

// --------------- state ---------------
void display_state(State state){
    switch (state) {
    case FOLLOW:
        printf("State:FOLLOW\n");
        break;
    case STOP:
        printf("State:STOP\n");
        break;
    case TURN:
        printf("State:TURN\n");
        break;
    case S_LEFT:
        printf("State:LEFT\n");
        break;
    case S_RIGHT:
        printf("State:RIGHT\n");
        break;
    case S_BACK:
        printf("State:BACK\n");
        break;
    case CHECK2:
        printf("State:CHECK2\n");
        break;
    case CHECK3:
        printf("State:CHECK3\n");
        break;
    case CHECK4:
        printf("State:CHECK4\n");
        break;
    case CHECK5:
        printf("State:CHECK5\n");
        break;
    case CHECK6:
        printf("State:CHECK6\n");
        break;
    case CHECK7:
        printf("State:CHECK7\n");
        break;
        break;
    case CHECK8_WHITE:
        printf("State:CHECK8_WHITE\n");
        break;
    default:
        break;
    }
}

// ==================== MISC ====================
void set_tab(int *tab, int size, int value) {
    for (int i = 0; i < size; i++) {
        tab[i] = value;
    }
}

int map(int x, int in_min, int in_max, int out_min, int out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}