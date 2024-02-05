// #include <tinyprintf.h>
// #include <stm32f4/rcc.h>
// #include <stm32f4/gpio.h>
// #include <stm32f4/nvic.h>
// #include <stm32f4/exti.h>
// #include <stm32f4/syscfg.h>
// #include <stm32f4/tim.h>
// #include <stm32f4/adc.h>

// // GPIOD
// #define S0	12
// #define S1	13
// #define S2	14
// #define S3	15
// #define OUT	11

// #define NUM_CYCLES	500

// // TIM4
// #define WAIT_PSC 	1000
// #define WAIT_DELAY	(APB1_CLK / WAIT_PSC)
// int PERIOD = WAIT_DELAY;

// // Init
// void gpio_init() 
// {
//     // Output init
//     GPIOD_MODER = REP_BITS(GPIOD_MODER, S0 * 2, 2, GPIO_MODER_OUT);
//     GPIOD_OTYPER &= ~(1 << S0);
//     GPIOD_MODER = REP_BITS(GPIOD_MODER, S1 * 2, 2, GPIO_MODER_OUT);
//     GPIOD_OTYPER &= ~(1 << S1);
//     GPIOD_MODER = REP_BITS(GPIOD_MODER, S2 * 2, 2, GPIO_MODER_OUT);
//     GPIOD_OTYPER &= ~(1 << S2);
//     GPIOD_MODER = REP_BITS(GPIOD_MODER, S3 * 2, 2, GPIO_MODER_OUT);
//     GPIOD_OTYPER &= ~(1 << S3);
    
//     // Signal init
//     GPIOD_MODER = REP_BITS(GPIOD_MODER, OUT * 2, 2, GPIO_MODER_IN);
//     GPIOB_PUPDR = REP_BITS(GPIOB_PUPDR, OUT * 2, 2, GPIO_PUPDR_PU);
//     GPIOD_OTYPER &= ~(1 << OUT);
// }

// void tim4_init()
// {
//     TIM4_CR1 = 0;
// 	TIM4_PSC = WAIT_PSC-1;
// 	TIM4_ARR = PERIOD;
// 	TIM4_EGR = TIM_UG;
// 	TIM4_SR = 0;
// 	TIM4_CR1 = TIM_CEN;
// }
// void calc(uint16_t raw, char* cN, uint8_t *out)
// {
//     uint32_t start_time = 0;
//     uint32_t elapsed_time = 0;

//     uint16_t redMin = 0;
//     uint16_t redMax = 1;
//     uint16_t greenMin = 0;
//     uint16_t greenMax = 1;
//     uint16_t blueMin = 0;
//     uint16_t blueMax = 1;

//     uint8_t out_from = 255;
//     uint8_t out_to = 0;
    
//     uint8_t colourTime = 1;

//     // Frequency measurement of the specific color
//     for (int impulse_count = 0; impulse_count < NUM_CYCLES; impulse_count++) {
//         // GPIO.wait_for_edge(signal, GPIO.RISING)
//         if (colourTime) {
//             start_time = TIM4_CNT;
//             colourTime = 0;
//         }
//     }

//     elapsed_time = TIM4_CNT - start_time;
//     raw = (uint16_t)(NUM_CYCLES / elapsed_time);
//     printf("red frequency - %d Hz\n", raw);

//     // Assignment of the measured color to an RGB value between 0-255
//     uint8_t out_range = out_from - out_to;
//     uint16_t in_range = redMax - redMin;
//     uint16_t in_val = raw - redMin;

//     float val = ((float)in_val / in_range) * out_range;
//     *out = out_to + (uint8_t)val;

//     if (*out < 0)
//         *out = 0;
//     else if (*out > 255)
//         *out = 255;

//     printf("%s = %d\n", *cN, *out);
// }


// int main(void) {

// 	// RCC init
// 	RCC_AHB1ENR |= RCC_GPIOAEN;
//     RCC_AHB1ENR |= RCC_GPIODEN;
//     RCC_APB1ENR |= RCC_TIM4EN;
//     RCC_APB2ENR |= RCC_ADC1EN;

//     gpio_init();
//     tim4_init();

//     /*  
//         Determination of the output frequency scaling :
//         S0/S1
//         LOW/LOW=/, LOW/HIGH=2%,
//         HIGH/LOW=20%, HIGH/HIGH=100%
//     */
//     GPIOD_BSRR |= 1 << S0; // Low
//     GPIOD_BSRR |= 1 << S1; // Low

//     int temp = 0;

//     while(1){
//         /*#################################################################################################*/
//         wait_seconds(000003);
//         printf("-----------------------------------------------\n");
//         wait_seconds(000003);
//         printf("Measured frequencys of Red, Green and Blue in Hz\n");
        
//         /*
//             Determination of the photodiode type :
//             S2/S3
//             LOW/LOW=RED, LOW/HIGH=BLUE,
//             HIGH/HIGH=GREEN, HIGH/LOW=CLEAR
//         */
//         GPIOD_BSRR |= 1 << (S2 + 16); // Low
//         GPIOD_BSRR |= 1 << (S3 + 16); // Low
//         wait_seconds(000003);
//         uint16_t redraw = 0;
//         uint8_t out_val1 = 0;
//         calc(&redraw, "R", &out_val1);

//         /*#################################################################################################*/
//         GPIOD_BSRR |= 1 << S2; // Low
//         GPIOD_BSRR |= 1 << S3; // Low
//         wait_seconds(0.000003);
//         uint16_t greenraw = 0;
//         uint8_t out_val2 = 0;
//         calc(&greenraw, "G", &out_val2);

//         /*#################################################################################################*/
//         GPIOD_BSRR |= 1 << (S2 + 16); // Low
//         GPIOD_BSRR |= 1 << S3; // Low
//         wait_seconds(000003);
//         uint16_t blueraw = 0;
//         uint8_t out_val3 = 0;
//         calc(&blueraw, "B", &out_val3);

//         /*#################################################################################################*/
//         uint8_t red = out_val1;
//         uint8_t green = out_val2;
//         uint8_t blue = out_val3;

//         wait_seconds(0.000003);
//         printf("-----------------------------------------------\n");
//         wait_seconds(0.000003);

//         // Determination of the measured color by comparison with the values of the other measured colors
//         if (red > green && red > blue) {
//             printf("Red detected\n");
//             temp = 1;
//         } else if (green > red && green > blue) {
//             printf("Green detected\n");
//             temp = 1;
//         } else if (blue > red && blue > green) {
//             printf("Blue detected\n");
//             temp = 1;
//         } else if (blue > red && green > red) {
//             printf("Turquoise detected\n");
//             temp = 1;
//         } else if (blue > green && red > green) {
//             printf("Purple detected\n");
//             temp = 1;
//         } else if (temp) {
//             printf("Place an object in front of the Sensor\n");
//             temp = 0;
//             wait_seconds(0.000003);
//         }
//     }
// }