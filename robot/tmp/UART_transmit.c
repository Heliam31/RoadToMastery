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
    //while (!(USART1_SR & USART_SR_TXE)) {};
    USART1_DR = 7; 
    //USART1_CR1 |= USART_CR1_SBK; 
    printf("here");
    while((USART1_SR & USART_SR_TC) == 0);
    printf("her2");
}

static void
uart_init()
{

    RCC_APB2ENR |= RCC_USART1EN;
    
    // configure usart pins
    GPIOA_MODER = REP_BITS(GPIOA_MODER, TX*2, 2, GPIO_MODER_ALT);
    GPIOA_AFRH = REP_BITS(GPIOA_AFRH, 1*4, 4, 7);
    // GPIOA_PUPDR = REP_BITS(GPIOA_PUPDR, TX*2 , 2, GPIO_PUPDR_PD);
    // GPIOA_OSPEEDR = REP_BITS(GPIOA_OSPEEDR, TX*2, 2, GPIO_OSPEEDR_HI);
    // GPIOA_OTYPER |= (0<<TX);


    GPIOA_MODER = REP_BITS(GPIOA_MODER, RX*2, 2, GPIO_MODER_ALT);
    GPIOA_AFRH = REP_BITS(GPIOA_AFRH, 2*4, 4, 7);
    // GPIOA_PUPDR = REP_BITS(GPIOA_PUPDR, RX*2 , 2, GPIO_PUPDR_PD);
    // GPIOA_OSPEEDR = REP_BITS(GPIOA_OSPEEDR, RX*2, 2, GPIO_OSPEEDR_HI);
    // GPIOA_OTYPER |= (0<<RX);

    USART1_CR1 |= USART_CR1_UE;
    USART1_BRR |=274;           //(int) (APB1_CLK / (16*9600)) << 4;
    //USART1_BRR |= 4375;
    USART1_CR2 &= ~USART_CR2_STOP1 ;
    USART1_CR2 &= ~USART_CR2_STOP2 ;
    //USART1_CR3 &= ~USART_CR3_HDSEL;
    USART1_CR1 |= USART_CR1_TE | USART_CR1_RE;
}




int main( void )
{
    RCC_AHB1ENR |= RCC_GPIOAEN;
   

   //initialize uart
   uart_init();

   while( 1 )
     {
        
        write();
        //  for(int i=0; i < 200000; i++);
        //while((USART1_SR & USART_SR_RXNE) == 0); 
        //int c =  USART1_DR;
        //printf("%d\n", c);
        //my_delay_1();
     }
}