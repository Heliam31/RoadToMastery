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

// TIM5
#define COLOR_TIME 20000 // 50ms
#define COLOR_WAIT_PSC PSC_42

// COLOR VALUES
#define CLEAR_FILTER  0
#define RED_FILTER    1
#define GREEN_FILTER  2
#define BLUE_FILTER   3

// MISC
#define LOW 0
#define HIGH 1

const int COLOR_TIMEOUT = COLOR_TIME;

// Calibration
int redMin = COLOR_TIMEOUT; //465;
int redMax = 0; //3058;
int greenMin = COLOR_TIMEOUT; //436;
int greenMax = 0; //2808;
int blueMin = COLOR_TIMEOUT; //529;
int blueMax = 0; //3300;

// RGB values
int redColor = 0;
int greenColor = 0;
int blueColor = 0;

int redFrequency = 0;
int redEdgeTime = 0;
int greenFrequency = 0;
int greenEdgeTime = 0;
int blueFrequency = 0;
int blueEdgeTime = 0;

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

    set_scaling(2);

}
void init_tim5_color(void) {
    TIM5_CR1 = 0;
    TIM5_PSC = COLOR_WAIT_PSC-1;
    TIM5_ARR = COLOR_TIME;
    TIM5_EGR = TIM_UG;
    TIM5_SR = 0;
    // TIM5_CR1 = TIM_CEN;
}

void color_init(void) {
    init_gpio_color();
    init_tim5_color();
}

void set_scaling(int scaling) {
    GPIOB_BSRR = (1 << (S0_PIN +16)) | (1 << (S1_PIN +16)); // Reset
    
    switch (scaling) {
        case 0:
            GPIOB_BSRR |= (1 << (S0_PIN+16));
            GPIOB_BSRR |= (1 << (S1_PIN+16));
            break;
        case 2:
            GPIOB_BSRR |= (1 << (S0_PIN+16));
            GPIOB_BSRR |= (1 << (S1_PIN));
            break;
        case 20:
            GPIOB_BSRR |= (1 << (S0_PIN));
            GPIOB_BSRR |= (1 << (S1_PIN+16));
            break;
        case 100:
            GPIOB_BSRR |= (1 << (S0_PIN));
            GPIOB_BSRR |= (1 << (S1_PIN));
            break;
        default:
            GPIOB_BSRR |= (1 << (S0_PIN+16));
            GPIOB_BSRR |= (1 << (S1_PIN));
            break;
    }
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

/*
**  @brief: Start the timer 5
**  @param: None
**  @return: None
*/
void start_tim5(void){
    TIM5_CNT = 0; // Reset the counter
    TIM5_CR1 = TIM_CEN; // Start the timer
    TIM5_SR = 0; // Clear the update event flag
}

/*
**  @brief: Stop the timer 5
**  @param: None
**  @return: None
*/
void stop_tim5(void){
    TIM5_CR1 &= ~TIM_CEN; // Stop the timer
}

/*
**  @brief: Measures GPIOB IN pulse time in microseconds (returns timeout if timeout) 
**  @param: pin : pin number
**  @param: value : LOW or HIGH
**  @param: timeout : timeout value
**  @return: pulse width in microseconds
*/

uint32_t pulseIn(int pin, int value, int timeout) {
    uint32_t startTime = 0;
    uint32_t pulseTime = 0;
    uint32_t elapsedTime = 0;
    
    start_tim5();
    startTime = TIM5_CNT;
    while (((GPIOB_IDR & (1 << pin)) == value) && (elapsedTime < timeout)) {
        elapsedTime = (TIM5_CNT - startTime);
    }
    stop_tim5();

    pulseTime = ((elapsedTime + startTime) >= timeout) ? timeout : (elapsedTime);
    return pulseTime;
}

int get_rgb_value(char color){
    int rgb_value = 0;
    switch (color)
    {
    case 'r':
        // printf("rf: %d\t\t",colorFrequency);
        rgb_value = map(redFrequency, redMax, redMin, 255,0);
        break;
    case 'g':
        // printf("gf: %d\t\t",colorFrequency);
        rgb_value = map(greenFrequency, greenMax, greenMin, 255,0);
        break;
    case 'b':
        // printf("bf: %d\t\t",colorFrequency);
        rgb_value = map(blueFrequency, blueMax, blueMin, 255,0);
        break;
    }
    //Constrain Value to MaxRange
    if (rgb_value > 255) rgb_value = 255;
    if (rgb_value < 0) rgb_value = 0;

    return rgb_value;
}

void color_read(int *isR, int *isG) {
    set_color('r');
    // delay_ms(300);
    redEdgeTime = pulseIn(SENSOR_OUT_PIN, HIGH, COLOR_TIMEOUT) + pulseIn(SENSOR_OUT_PIN,LOW, COLOR_TIMEOUT);
    if (redEdgeTime < COLOR_TIMEOUT) redFrequency = (1/(redEdgeTime*0.000001));
    redColor = get_rgb_value('r');
    // printf("*%d\t",redEdgeTime);
    // printf("%d\t",redColor);
    // delay_ms(300);
    
    set_color('g');
    // delay_ms(300);
    greenEdgeTime = pulseIn(SENSOR_OUT_PIN, HIGH, COLOR_TIMEOUT) + pulseIn(SENSOR_OUT_PIN,LOW, COLOR_TIMEOUT);
    if (greenEdgeTime < COLOR_TIMEOUT) greenFrequency = (1/(greenEdgeTime*0.000001));
    greenColor = get_rgb_value('g');
    // printf("*%d\t",greenEdgeTime);
    // printf("%d\t",greenColor);
    // delay_ms(300);
    
    set_color('b');
    // delay_ms(300);
    blueEdgeTime = pulseIn(SENSOR_OUT_PIN, HIGH, COLOR_TIMEOUT) + pulseIn(SENSOR_OUT_PIN,LOW, COLOR_TIMEOUT);
    if (blueEdgeTime < COLOR_TIMEOUT) blueFrequency = (1/(blueEdgeTime*0.000001));
    blueColor = get_rgb_value('b');
    // printf("*%d\n",blueEdgeTime);
    // printf("%d\n",blueColor);
    // delay_ms(300);

}

// void display_colorCalibration(void) {
//     printf("Color calibration values :\n");
//     printf(" >redMin    : %d\n",redMin);
//     printf(" >redMax    : %d\n",redMax);
//     printf(" >greenMin  : %d\n",greenMin);
//     printf(" >greenMax  : %d\n",greenMax);
//     printf(" >blueMin   : %d\n",blueMin);
//     printf(" >blueMax   : %d\n\n",blueMax);
// }

// void tcs3200_calibrate(char color) {
//     int redFreq = 0, greenFreq = 0, blueFreq = 0;
//     switch (color)
//     {
//     case 'w':
//         set_color('r');
//         delay_ms(3);
//         redFreq = _read_color_frequency();
//         if (redFreq > redMax && redFreq < 10000)
//             redMax = redFreq;
        
//         set_color('g');
//         delay_ms(3);
//         greenFreq = _read_color_frequency();
//         if (greenFreq > greenMax && greenFreq < 10000)
//             greenMax = greenFreq;

//         set_color('b');
//         delay_ms(3);
//         blueFreq = _read_color_frequency();
//         // printf("!%d!\t",blueFreq);
//         if (blueFreq > blueMax && blueFreq < 8000)
//             blueMax = blueFreq;
//         break;

//     case 'b':
//         set_color('r',2);
//         delay_ms(3);
//         redFreq = _read_color_frequency();
//         if (redFreq < redMin)
//             redMin = redFreq;

//         set_color('g',2);
//         delay_ms(3);
//         greenFreq = _read_color_frequency();
//         if (greenFreq < greenMin)
//             greenMin = greenFreq;

//         set_color('b',2);
//         delay_ms(3);
//         blueFreq = _read_color_frequency();
//         if (blueFreq < blueMin)
//             blueMin = blueFreq;

//         break;
        
//     default:
//         printf("\n\n/!\\ colors allowed are 'w' for White and 'b' for Black  /!\\\n\n");
//         break;
//     }
// }
