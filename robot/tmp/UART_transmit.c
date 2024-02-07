#include <stdlib.h> 
#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/nvic.h>
#include <stm32f4/exti.h>
#include <stm32f4/syscfg.h>
#include <stm32f4/tim.h>
#include <stm32f4/adc.h>
#include <stm32f4/dac.h>
#include <stm32f4/i2c.h>
#include <stm32f4/uart.h>

// GPIOD
#define GREEN_LED	12
#define ORANGE_LED	13
#define RED_LED		14
#define BLUE_LED	15

//USART ports (GPIOB)

#define TX 9
#define RX 10

void write(){
    while (!(USART1_SR & USART_SR_TXE)) {};
    USART1_DR = 1; 
    // USART1_CR1 |= USART_CR1_SBK; 
    printf("here");
    while((USART1_SR & USART_SR_TC) == 0);
    printf("her2");
}

static void
uart_init()
{
    RCC_AHB1ENR |= RCC_GPIOAEN;
    RCC_APB2ENR |= RCC_USART1EN;
    
    // configure usart pins
    GPIOA_MODER = REP_BITS(GPIOA_MODER, TX*2, 2, GPIO_MODER_ALT);
    GPIOA_AFRH = REP_BITS(GPIOA_AFRH, 1*4, 4, 7);
    // GPIOA_PUPDR = REP_BITS(GPIOA_PUPDR, TX*2 , 2, GPIO_PUPDR_PD);
    GPIOA_OSPEEDR = REP_BITS(GPIOA_OSPEEDR, TX*2, 2, GPIO_OSPEEDR_HI);
    // GPIOA_OTYPER |= (0<<TX);


    GPIOA_MODER = REP_BITS(GPIOA_MODER, RX*2, 2, GPIO_MODER_ALT);
    GPIOA_AFRH = REP_BITS(GPIOA_AFRH, 2*4, 4, 7);
    // GPIOA_PUPDR = REP_BITS(GPIOA_PUPDR, RX*2 , 2, GPIO_PUPDR_PD);
    GPIOA_OSPEEDR = REP_BITS(GPIOA_OSPEEDR, RX*2, 2, GPIO_OSPEEDR_HI);
    // GPIOA_OTYPER |= (0<<RX);

    
    
    // USART1_BRR = 274;           
    // USART1_BRR = (int) (APB1_CLK / (16*9600)) << 4;

    // uint32_t integerdivider = ((25 * APB1_CLK) / (4 * (115200)));
    // uint32_t tmpreg = (integerdivider / 100) << 4;
    // uint32_t fractionaldivider = integerdivider - (100 * (tmpreg >> 4));
    // tmpreg |= ((((fractionaldivider * 16) + 50) / 100)) & ((uint8_t)0x0F);
    USART1_CR1 = 0;
    // USART1_BRR = (uint16_t)tmpreg;
    USART1_CR1 |= USART_CR1_TE | USART_CR1_RE;

    //9600 baud
    // USART1_BRR |= (273<<4);
    // USART1_BRR |= 7;

    //115200 baud
    USART1_BRR |= (22<<4);
    USART1_BRR |= 13;   

    // USART1_CR2 &= ~USART_CR2_STOP1 ;
    // USART1_CR2 &= ~USART_CR2_STOP2 ;
    //USART1_CR3 &= ~USART_CR3_HDSEL;

    USART1_CR1 |= USART_CR1_UE;
}




int main( void )
{
    
   

   //initialize uart
   uart_init();

   while( 1 )
     {
        volatile uint8_t msg[] = "h";
        for (uint32_t i=0; i<sizeof(msg); i++){
            // send character
            USART1_DR = 7;
            // wait for transmit complete
            while(!(USART1_SR & (1 << 6)));
            //for(int i=0; i < 10000000; i++);
        }

        for(int i=0; i < 1000000000; i++);
        //while((USART1_SR & USART_SR_RXNE) == 0); 
        //int c =  USART1_DR;
        //printf("%d\n", c);
        //my_delay_1();
     }
}