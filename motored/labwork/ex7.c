/* Embedded Systems - Exercise 7 */

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

// GPIODA
#define USER_BUT	0

volatile int b_state = 0;

void handle_button() {
	if((GPIOA_IDR & (1 << USER_BUT)) == 0){
		b_state = 1;
	}
	if(b_state){
		b_state = 0;
		if((GPIOD_ODR & (1<<GREEN_LED)) != 0){
			GPIOD_BSRR = 1 << (GREEN_LED+16);
		}else{
			GPIOD_BSRR = 1 << GREEN_LED;
		}
	}
	EXTI_PR |= 1 << 0;
	NVIC_ICPR(EXTI0_IRQ >> 5) = 1 << (EXTI0_IRQ & 0x1f);
}

void init_GPIO(){

	GPIOA_MODER = REP_BITS(GPIOA_MODER, USER_BUT*2 , 2, GPIO_MODER_IN);
	GPIOA_PUPDR = REP_BITS(GPIOA_PUPDR, USER_BUT*2 , 2, GPIO_PUPDR_PD);

	for(int i = 12; i<=15; i++){
		GPIOD_MODER = REP_BITS(GPIOD_MODER, i*2, 2, GPIO_MODER_OUT);
		GPIOD_OTYPER = GPIOD_OTYPER && ~(1<<i);
	}
}

void init_TIM4(){
	DISABLE_IRQS;
	//Butt mapped to PA0/EXTI0
	//config EXTI for GPIOA(0)
	SYSCFG_EXTICR1 = REP_BITS(SYSCFG_EXTICR1, 0, 4, 0);
	EXTI_RTSR |= 1 << 0; //rising edge
	EXTI_FTSR |= 1 << 0; //falling edge
	EXTI_IMR |= 1 << 0; //enable
	EXTI_PR |= 1 << 0; //clear pending

	NVIC_ICER ( EXTI0_IRQ >> 5 ) = 1 << ( EXTI0_IRQ & 0X1f ) ;
	NVIC_IRQ ( EXTI0_IRQ ) = (uint32_t) handle_button;
	NVIC_IPR ( EXTI0_IRQ ) = 0 ;
	NVIC_ICPR ( EXTI0_IRQ >> 5 ) = 1 << ( EXTI0_IRQ & 0X1f ) ;
	NVIC_ISER ( EXTI0_IRQ >> 5 ) = 1 << ( EXTI0_IRQ & 0X1f ) ;

	ENABLE_IRQS;
}

int main() {
	printf("\nStarting...\n");

	// RCC init
	RCC_AHB1ENR |= RCC_GPIOAEN;
	RCC_AHB1ENR |= RCC_GPIODEN;
	RCC_APB1ENR |= RCC_TIM4EN;

	init_GPIO();
	init_TIM4();
	// main loop
	printf("Endless loop!\n");
	while(1) {
	}

}


