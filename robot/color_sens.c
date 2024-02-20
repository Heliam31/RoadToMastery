#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/nvic.h>
#include <stm32f4/exti.h>
#include <stm32f4/syscfg.h>
#include <stm32f4/tim.h>


// Pin definitions
#define S0_PIN    11  // PB14
#define S1_PIN    12  // PB14
#define S2_PIN    13  // PB14
#define S3_PIN    14  // PB14
#define SENSOR_OUT_PIN  0   // PB0

// TIM5
#define COLOR_T 1
#define COLOR_WAIT_PSC 65
#define WAIT_COLOR_DELAY (COLOR_T*APB1_CLK)/COLOR_WAIT_PSC
const int colorPeriod = WAIT_COLOR_DELAY;


// COLOR VALUES
uint32_t RGB_RAW[3] = {0, 0, 0};
int raw_index = 0;

#define CLEAR_FILTER  0
#define RED_FILTER    1
#define GREEN_FILTER  2
#define BLUE_FILTER   3

#define REDMAX   225
#define REDMIN   181 

#define BLUEMAX   202 
#define BLUEMIN   181 

#define GREENMAX   1 
#define GREENMIN   150 

// Init
void init_gpio_color(void) {

    GPIOB_MODER = REP_BITS(GPIOB_MODER, S0_PIN * 2, 2, GPIO_MODER_OUT);
    GPIOB_OTYPER &= ~(1 << S0_PIN);

    GPIOB_MODER = REP_BITS(GPIOB_MODER, S1_PIN * 2, 2, GPIO_MODER_OUT);
    GPIOB_OTYPER &= ~(1 << S1_PIN);

    GPIOB_MODER = REP_BITS(GPIOB_MODER, S2_PIN * 2, 2, GPIO_MODER_OUT);
    GPIOB_OTYPER &= ~(1 << S2_PIN);

    GPIOB_MODER = REP_BITS(GPIOB_MODER, S3_PIN * 2, 2, GPIO_MODER_OUT);
    GPIOB_OTYPER &= ~(1 << S3_PIN);

    GPIOB_MODER = REP_BITS(GPIOB_MODER, SENSOR_OUT_PIN * 2, 2, GPIO_MODER_IN);
    GPIOB_PUPDR = REP_BITS(GPIOB_PUPDR, SENSOR_OUT_PIN * 2, 2, GPIO_PUPDR_PU);

}
void init_tim5_color(void) {
    TIM5_CR1 = 0;
    TIM5_PSC = COLOR_WAIT_PSC-1;
    TIM5_ARR = colorPeriod;
    TIM5_EGR = TIM_UG;
    TIM5_SR = 0;
    TIM5_CR1 = TIM_CEN;
}

void color_init(void) {
    init_gpio_color();
    init_tim5_color();
}

// =============== UTILS ===============
void TIM5_Delay(float delay){
    TIM5_ARR = (int)(delay*APB1_CLK/COLOR_WAIT_PSC); // Total period of the timer
    TIM5_CNT = 0;
    TIM5_SR &= ~TIM_UIF; //Reset the update interrupt flag
    TIM5_CR1 = TIM_CEN; //Start the Timer

    while(!(TIM5_SR & TIM_UIF)){} //Polling the update interrupt flag
	TIM5_ARR = colorPeriod;
    TIM5_CR1 &= ~TIM_CEN;
}


int set_filter(char color, int scaling) {
    GPIOB_BSRR = (1 << (S0_PIN +16)) | (1 << (S1_PIN +16))  | (1 << (S2_PIN +16))  | (1 << (S3_PIN +16)) ; // Reset all filter pins

    switch (scaling)
    {
    case 0:
        GPIOB_BSRR = (1 << (S0_PIN+16));
        GPIOB_BSRR = (1 << (S1_PIN+16));
        break;
    case 2:
        GPIOB_BSRR = (1 << (S0_PIN+16));
        GPIOB_BSRR = (1 << (S1_PIN));
        break;
    case 20:
        GPIOB_BSRR = (1 << (S0_PIN));
        GPIOB_BSRR = (1 << (S1_PIN+16));
        break;
    case 100:
        GPIOB_BSRR = (1 << (S0_PIN));
        GPIOB_BSRR = (1 << (S1_PIN));
        break;
    
    default:
        GPIOB_BSRR = (1 << (S0_PIN));
        GPIOB_BSRR = (1 << (S1_PIN+16));
        break;
    }

    switch (color)
    {
    case 'r':
        GPIOB_BSRR = (1 << (S2_PIN+16));
        GPIOB_BSRR = (1 << (S3_PIN+16));
        break;
    case 'b':
        GPIOB_BSRR = (1 << (S2_PIN+16));
        GPIOB_BSRR = (1 << (S3_PIN));
        break;
    case 'c':
        GPIOB_BSRR = (1 << (S2_PIN));
        GPIOB_BSRR = (1 << (S3_PIN+16));
        break;
    case 'g':
        GPIOB_BSRR = (1 << (S2_PIN));
        GPIOB_BSRR = (1 << (S3_PIN));
        break;
    
    default:
        GPIOB_BSRR = (1 << (S2_PIN));
        GPIOB_BSRR = (1 << (S3_PIN+16));
        break;
    }


}

uint32_t _read_c() {
    uint32_t colorElapsedTime = 0;
    uint32_t frequency = 0;
    uint32_t edgeTime = 0;

    // Start the timer for the measure
    TIM5_CNT = 0;
    TIM5_CR1 = TIM_CEN;
    TIM5_SR = 0;

    uint32_t high_time, low_time;


    high_time = TIM5_CNT;
    while (!(GPIOB_IDR & (1 << SENSOR_OUT_PIN)));// Wait for rising edge
    high_time = TIM5_CNT - high_time;
    TIM5_CR1 &= ~TIM_CEN; 

    // Start the timer for the measure
    TIM5_CNT = 0;
    TIM5_CR1 = TIM_CEN;
    TIM5_SR = 0;

    low_time = TIM5_CNT;
    while ((GPIOB_IDR & (1 << SENSOR_OUT_PIN))); // Wait for falling edge
    low_time = TIM5_CNT - low_time;
    TIM5_CR1 &= ~TIM_CEN; 

    edgeTime = low_time + high_time;
    int timeInMicro = (edgeTime * 1E6 / colorPeriod);

    RGB_RAW[raw_index] = timeInMicro;
    raw_index = (raw_index + 1) % 3;

    if (edgeTime != 0){
        frequency = 1E6 / timeInMicro;
    }

    return frequency;
}

int is_not_color() { // returns 1 if detects black / white (other than blue red green)
    return (RGB_RAW[0] + RGB_RAW[1] + RGB_RAW[2]) < 1300;
}

void color_read(int *isG, int *isB, int *isR) {
    uint32_t r = 0, g = 0, b = 0;
    set_filter('g',2);
    g += _read_c();
    TIM5_Delay(0.03);

    set_filter('r',2);
    r += _read_c();
    TIM5_Delay(0.03);
    
    set_filter('b',2);
    b += _read_c();
    TIM5_Delay(0.03);

    if (is_not_color()) {
        *isR = 0;
        *isG = 0;
        *isB = 0;
    } else if (r >= g && r >= b) {
        *isR = 1;
        *isG = 0;
        *isB = 0;
    } else if (g >= r && g >= b) {
        *isR = 0;
        *isG = 1;
        *isB = 0;
    } else if (b >= r && b >= g) {
        *isR = 0;
        *isG = 0;
        *isB = 1;
    }


}