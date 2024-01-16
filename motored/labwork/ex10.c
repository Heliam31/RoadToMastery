/* Embedded Systems - Exercise 3 */

#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/nvic.h>
#include <stm32f4/exti.h>
#include <stm32f4/syscfg.h>
#include <stm32f4/tim.h>

// GPIOD
#define GREEN_LED	12
#define ORANGE_LED	13
#define RED_LED		14
#define BLUE_LED	15
#define LED 		2
// GPIODA
#define BUT	0
void initGPIO(){
	GPIOB_MODER = REP_BITS(GPIOB_MODER, BUT*2 , 2, GPIO_MODER_IN);
	GPIOB_PUPDR = REP_BITS(GPIOB_PUPDR, BUT*2 , 2, GPIO_PUPDR_PD);

	GPIOD_MODER = REP_BITS(GPIOD_MODER, LED*2, 2, GPIO_MODER_OUT);
	GPIOD_OTYPER = GPIOD_OTYPER && ~(1<<LED);
}


volatile int b_state = 0;
void handleB(){
	if((GPIOA_IDR & (1 << BUT)) == 0){
		GPIOD_BSRR = 1 << LED;
	}
	else{
	   GPIOD_BSRR = 1 << (16+LED);
	}
	EXTI_PR |= 1 << 0;
	NVIC_ICPR(EXTI0_IRQ >> 5) = 1 << (EXTI0_IRQ & 0x1f);
}

void initInterrup(){
	DISABLE_IRQS;
	
	SYSCFG_EXTICR1 = REP_BITS(SYSCFG_EXTICR1, 0, 4, 0);
	EXTI_RTSR |= 1 << 0; //rising edge
	EXTI_FTSR |= 1 << 0; //falling edge
	EXTI_IMR |= 1 << 0; //enable
	EXTI_PR |= 1 << 0; //clear pending

	NVIC_ICER ( EXTI0_IRQ >> 5 ) = 1 << ( EXTI0_IRQ & 0X1f ) ;
	NVIC_IRQ ( EXTI0_IRQ ) = (uint32_t) handleB;
	NVIC_IPR ( EXTI0_IRQ ) = 0 ;
	NVIC_ICPR ( EXTI0_IRQ >> 5 ) = 1 << ( EXTI0_IRQ & 0X1f ) ;
	NVIC_ISER ( EXTI0_IRQ >> 5 ) = 1 << ( EXTI0_IRQ & 0X1f ) ;

	ENABLE_IRQS;
}

int main() {
	printf("Starting...\n");

	// RCC init
	RCC_AHB1ENR |= RCC_GPIOAEN;
	RCC_AHB1ENR |= RCC_GPIODEN;
	RCC_AHB1ENR |= RCC_GPIOBEN;
	RCC_APB1ENR |= RCC_TIM4EN;
	

	// GPIO init
	initGPIO();
	initInterrup();
	

	printf("Endless loop!\n");
	while(1) {
	}
}

//Appui eteind appuie allum
//if((GPIOA_IDR & (1 << BUT)) == 0){
//		b_state = 1;
//	}
//	if(b_state){
//		b_state = 0;
//		if((GPIOD_ODR & (1<<LED)) != 0){
//			GPIOD_BSRR = 1 << (LED+16);
//		}else{
//			GPIOD_BSRR = 1 << LED;
//		}
//	}
//	EXTI_PR |= 1 << 0;
//	NVIC_ICPR(EXTI0_IRQ >> 5) = 1 << (EXTI0_IRQ & 0x1f);