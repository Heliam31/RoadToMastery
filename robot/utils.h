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
// =============== sync ===============
#define SYNC_N 0.1
#define SYNC_PSC 8 // ->1s  // 8->0.01s
#define SYNC_PERIOD (SYNC_N*APB1_CLK)/SYNC_PSC

// =============== led & button ===============
#define GREEN_LED 12
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
void init_timer_sync(void);
void sync(void);

void init_gpio_led(void);
void turn_on(int led);
void turn_off(int led);

void init_gpio_button(void);
void wait_button(void);

void display_direction(Direction direction);

#endif