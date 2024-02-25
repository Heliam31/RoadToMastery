#ifndef __UTILS_H__
#define __UTILS_H__

#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/nvic.h>
#include <stm32f4/exti.h>
#include <stm32f4/syscfg.h>
#include <stm32f4/tim.h>
#include <stm32f4/adc.h>
#include <stm32f4/io.h>


// CONSTANTS & TYPES
// timer
#define PSC_42000 42000
#define PSC_42 42
#define DEFAULT_HYPERPERIOD 125  // Default Hyperperiod Sonar + Color + IR

// =============== led & button ===============
#define GREEN_LED	12
#define ORANGE_LED	13
#define RED_LED		14
#define BLUE_LED	15
#define BUTTON 0

// =============== states & directions ===============
typedef enum calibration_e {ON, OFF} Calibration;
typedef enum state_e {
    FOLLOW, STOP, TURN, // main states
    S_LEFT, S_RIGHT, S_BACK,  // sub states
    CHECK2, CHECK3, CHECK4, CHECK5, CHECK6, CHECK7, CHECK8_WHITE // subsub states
} State;
typedef enum direction_e {FRONT, BACK, LEFT, RIGHT} Direction;

// =============== i2c ===============
#define ESP32_ADDR 0x33

typedef struct msg_s {
    int addr;
    int data[8];
    int size;
} msg_t;


// FUNCTIONS
// =============== math ===============
int abs(int x);
int max(int a, int b);
int min(int a, int b
);
int clamp(int x, int a, int b);
int sign(int x);

// =============== comm. ===============

// =============== stm32 ===============
void init_tim6(void);
void delay_ms(int ms);
void delay_us(int us);

void init_tim7(void);
void start_sync(int hyperperiod);
void sync(void);

void led_init(int led);
void led_turn_on(int led);
void led_turn_off(int led);
void led_clear_all(void);

void button_init(int button);
void button_wait(int button);

void display_state(State state);
void display_direction(Direction direction);

// =============== misc ===============
void set_tab(int *tab, int size, int value);
int map(int x, int in_min, int in_max, int out_min, int out_max);


#endif