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
#define TEMPS_SEC 0.01
#define TIMER_PSC 42
#define TIMER_PERIOD (TEMPS_SEC*APB1_CLK)/TIMER_PSC;

// =============== led & button ===============
#define GREEN_LED	12
#define ORANGE_LED	13
#define RED_LED		14
#define BLUE_LED	15
#define BUTTON 0

// =============== states & directions ===============
typedef enum state_e {FOLLOW, JONCTION, STOP} State;
typedef enum direction_e {FRONT, BACK, LEFT, RIGHT} Direction;

// =============== i2c ===============
#define ESP32_ADDR 0

typedef struct msg_s {
    int addr;
    int data[8];
    int size;
} msg_t;


// FUNCTIONS
// =============== math ===============
int abs(int x);
int max(int a, int b);
int min(int a, int b);
int clamp(int x, int a, int b);
int sign(int x);

// =============== comm. ===============

// =============== stm32 ===============
void timer_init(void);
void timer_reset(void);
void timer_enable(void);
void timer_sync(void);

void led_init(int led);
void led_turn_on(int led);
void led_turn_off(int led);
void led_clear_all(void);

void button_init(int button);
void button_wait(int button);

void display_direction(Direction direction);

#endif