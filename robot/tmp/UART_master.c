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

//I2C ports (GPIOB)
#define TX 6
#define RX 7


void write(){
    //while (!(USART1_SR & USART_SR_TXE)) {};
    USART1_DR = 0x07; 
    USART1_CR1 |= USART_CR1_SBK; 
    printf("here");
    while((USART1_SR & USART_SR_TC) == 0);
    printf("her2");
}

static void
uart_init()
{


    //init_NVIC();

    // configure i2c pins
    GPIOB_MODER = REP_BITS(GPIOB_MODER, TX*2, 2, GPIO_MODER_ALT);
    GPIOB_AFRL = REP_BITS(GPIOB_AFRL, 6*4, 4, 7);
    GPIOB_PUPDR = REP_BITS(GPIOB_PUPDR, TX*2 , 2, GPIO_PUPDR_PD);
    GPIOB_OTYPER |= (0<<TX);

    GPIOB_MODER = REP_BITS(GPIOB_MODER, RX*2, 2, GPIO_MODER_ALT);
    GPIOB_AFRL = REP_BITS(GPIOB_AFRL, 7*4, 4, 7);
    GPIOB_PUPDR = REP_BITS(GPIOB_PUPDR, RX*2 , 2, GPIO_PUPDR_PD);
    GPIOB_OTYPER |= (0<<RX);

    USART1_CR1 |= USART_CR1_UE;
    //USART1_BRR |= (int) (APB1_CLK / (16*9600)) << 4;
    USART1_BRR |= 4375;
    USART1_CR2 &= ~USART_CR2_STOP1 ;
    USART1_CR2 &= ~USART_CR2_STOP2 ;
    //USART1_CR3 &= ~USART_CR3_HDSEL;
    USART1_CR1 |= USART_CR1_TE | USART_CR1_RE;
}




int main( void )
{
    RCC_AHB1ENR |= RCC_GPIOBEN;
    RCC_APB1ENR |= RCC_I2C1EN;
    RCC_APB2LPENR |= RCC_USART1LPEN;

   //initialize i2c slave
   uart_init();

   while( 1 )
     {
        
        //write();
        //for(int i=0; i < 200000000; i++);
        while((USART1_SR & USART_SR_RXNE) == 0); 
        int c =  USART1_DR;
        printf("%d\n", c);
        //my_delay_1();
     }
}