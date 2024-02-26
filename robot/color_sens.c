#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/nvic.h>
#include <stm32f4/exti.h>
#include <stm32f4/syscfg.h>
#include <stm32f4/tim.h>

#include "utils.h"

// Pin definitions
#define S0_PIN    11  // PB14
#define S1_PIN    12  // PB14
#define S2_PIN    13  // PB14
#define S3_PIN    14  // PB14
#define SENSOR_OUT_PIN  0   // PB0

// COLOR VALUES
#define CLEAR_FILTER  0
#define RED_FILTER    1
#define GREEN_FILTER  2
#define BLUE_FILTER   3

// Calibration
int redMin = 17500; //20487;
int redMax = 84000; //105000;
int greenMin = 13333; //17500;
int greenMax = 76363; //93333;
int blueMin = 20487; //20487;
int blueMax = 31111; //105000;

// RGB values
int redColor = 0;
int greenColor = 0;
int blueColor = 0;

int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;

void set_scaling(int scaling);

// Init
void init_gpio_color(void) {

    GPIOB_MODER = REP_BITS(GPIOB_MODER, S0_PIN * 2, 2, GPIO_MODER_OUT);
    GPIOB_OTYPER &= ~(1 << S0_PIN);;

    GPIOB_MODER = REP_BITS(GPIOB_MODER, S1_PIN * 2, 2, GPIO_MODER_OUT);
    GPIOB_OTYPER &= ~(1 << S1_PIN);

    GPIOB_MODER = REP_BITS(GPIOB_MODER, S2_PIN * 2, 2, GPIO_MODER_OUT);
    GPIOB_OTYPER &= ~(1 << S2_PIN);

    GPIOB_MODER = REP_BITS(GPIOB_MODER, S3_PIN * 2, 2, GPIO_MODER_OUT);
    GPIOB_OTYPER &= ~(1 << S3_PIN);

    GPIOB_MODER = REP_BITS(GPIOB_MODER, SENSOR_OUT_PIN * 2, 2, GPIO_MODER_IN);
    GPIOB_PUPDR = REP_BITS(GPIOB_PUPDR, SENSOR_OUT_PIN * 2, 2, GPIO_PUPDR_PU);
    GPIOB_OTYPER &= ~(1 << SENSOR_OUT_PIN);

}

void color_init(void) {
    init_gpio_color();
}

void set_filter(uint8_t filter) {
    GPIOB_BSRR = (1 << (S0_PIN +16)) | (1 << (S1_PIN +16))  | (1 << (S2_PIN +16))  | (1 << (S3_PIN +16)) ; // Reset all filter pins
    GPIOB_BSRR = (1 << (S0_PIN+16));
    GPIOB_BSRR = (1 << (S1_PIN));

    switch (filter) {
        case CLEAR_FILTER: // Clear
            GPIOB_BSRR = (1 << (S2_PIN)) | (1 << (S3_PIN + 16)); // Set S2 and S3
            break;
        case RED_FILTER: // Red
            GPIOB_BSRR = (1 << (S2_PIN +16 )) | (1 << (S3_PIN + 16)); // Set S2 and S3
            break;
        case GREEN_FILTER: // Green
            GPIOB_BSRR = (1 << (S2_PIN)) | (1 << (S3_PIN)); // Set S2 and S3
            break;
        case BLUE_FILTER: // Blue
            GPIOB_BSRR = (1 << (S2_PIN + 16)) | (1 << (S3_PIN)); // Set S2 and S3
            break;
    }
}


int get_color(uint8_t color) {
    int Output_Color = 0;

    uint32_t high_time =0; 
    uint32_t low_time = 0;
    uint32_t period = 0;
    uint32_t frequency = 0;


    RCC_APB1ENR |= RCC_TIM5EN;

    // Configure TIM5
    TIM5_PSC = 81; // Prescaler
    TIM5_ARR = 20000; // Auto-reload
    TIM5_CR1 |= TIM_CEN; // Enable Timer 3
    TIM5_CNT = 0; // Reset timer counter
    TIM5_SR = 0; // Reset timer counter

    set_filter(color);
    high_time = TIM5_CNT;
    while (!(GPIOB_IDR & (1 << SENSOR_OUT_PIN)));// Wait for rising edge
        high_time = TIM5_CNT - high_time;

    while ((GPIOB_IDR & (1 << SENSOR_OUT_PIN))); // Wait for falling edge
        low_time = TIM5_CNT;

    if (low_time > high_time)
        period =  low_time - high_time;
    else
        period = TIM5_ARR - high_time + low_time;
    
    uint32_t time_in_micro = (period * 1000000) / (TIM5_ARR); // Convert to microseconds

    frequency = (APB1_CLK / (time_in_micro)); // Assuming TIM5 is running at SystemCoreClock


    TIM5_CR1 &= ~TIM_CEN; // Disable Timer 5

    set_filter(CLEAR_FILTER);

    return frequency; 
}


void set_color(char color) {
    GPIOB_BSRR = (1 << (S2_PIN +16))  | (1 << (S3_PIN +16)); // Réinitialiser tous les pins de filtre

    switch (color) {
        case 'r':
            GPIOB_BSRR |= (1 << (S2_PIN+16));
            GPIOB_BSRR |= (1 << (S3_PIN+16));
            break;
        case 'b':
            GPIOB_BSRR |= (1 << (S2_PIN+16));
            GPIOB_BSRR |= (1 << (S3_PIN));
            break;
        case 'c':
            GPIOB_BSRR |= (1 << (S2_PIN));
            GPIOB_BSRR |= (1 << (S3_PIN+16));
            break;
        case 'g':
            GPIOB_BSRR |= (1 << (S2_PIN));
            GPIOB_BSRR |= (1 << (S3_PIN));
            break;
        default:
            GPIOB_BSRR |= (1 << (S2_PIN));
            GPIOB_BSRR |= (1 << (S3_PIN+16));
            break;
    }
}

int get_rgb_value(char color){
    int rgb_value = 0;
    switch (color)
    {
    case 'r':
        rgb_value = map(redFrequency, redMax, redMin, 255,0);
        break;
    case 'g':
        rgb_value = map(greenFrequency, greenMax, greenMin, 255,0);
        break;
    case 'b':
        rgb_value = map(blueFrequency, blueMax, blueMin, 255,0);
        break;
    }
    //Constrain Value to MaxRange
    if (rgb_value > 255) rgb_value = 255;
    if (rgb_value < 0) rgb_value = 0;

    set_filter(CLEAR_FILTER);

    return rgb_value;
}

void color_read(int *isR, int *isG) {
    redFrequency = get_color(RED_FILTER);
    greenFrequency = get_color(GREEN_FILTER);

    redColor = get_rgb_value('r');
    greenColor = get_rgb_value('g');
    
    if (redColor > 250 && greenColor > 250) {
        *isR = 0;
        *isG = 0;
    } 
    else if (abs(redColor - greenColor) <= 20) {
        *isR = 0;
        *isG = 0;
    }
    else if (redColor > greenColor) {
        *isR = 1;
        *isG = 0;
    }
    else if (greenColor > redColor) {
        *isR = 0;
        *isG = 1;
    }
}