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

int abs(int x);
int max(int a, int b);
int min(int a, int b);
int clamp(int x, int a, int b);
int sign(int x);

void sync(void);
void turn_on(int led);
void turn_off(int led);
void wait_start(void);

#endif