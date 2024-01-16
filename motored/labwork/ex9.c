/* Embedded Systems - Exercise  9*/


#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/tim.h>
#include <stm32f4/nvic.h>

// GPIOD
#define GREEN_LED	12
#define ORANGE_LED	13
#define RED_LED		14
#define BLUE_LED	15
#define LED_1    5
#define LED_2	 3
#define LED_3    1
#define LED_4 	 6
#define LED_5    4
#define LED_6  	 2

// GPIODA
#define USER_BUT	0

volatile int TIM4_triggered = 0;

void handle_TIM4()  {   //handle l'interruption
	TIM4_SR = ~TIM_UIF;
	TIM4_ARR = 50000;
	printf("handled");
	TIM4_triggered = 1;
	
}


void init_TIM4(){
	DISABLE_IRQS;

	//NVIC
	NVIC_ICER(TIM4_IRQ >> 5) = 1 << (TIM4_IRQ & 0X1f);
	NVIC_IRQ(TIM4_IRQ) = (uint32_t)handle_TIM4;
	NVIC_IPR(TIM4_IRQ) = 0;

	//purge IRQ
	NVIC_ICPR(TIM4_IRQ >> 5) = 1<<(TIM4_IRQ & 0X1f);

	//configure TIM4
	TIM4_CR1 = 0;
	TIM4_PSC = 1000-1;
	TIM4_ARR =  50000;
	//reset Timer
	TIM4_EGR = TIM_UG;
	TIM4_SR = 0;

	//enable IRQ
	NVIC_ISER(TIM4_IRQ >> 5) = 1 <<(TIM4_IRQ & 0X1f);
	TIM4_DIER = TIM_UIE;

	//start
	ENABLE_IRQS;
	TIM4_CR1 = TIM_CEN;
}




int main() {
	printf("\nStarting...\n");

	// RCC init
	RCC_AHB1ENR |= RCC_GPIOAEN;
	RCC_AHB1ENR |= RCC_GPIODEN;
	RCC_APB1ENR |= RCC_TIM4EN;

	//GPIO INIT
	//LED1
	GPIOD_MODER = REP_BITS(GPIOD_MODER, LED_1*2, 2, GPIO_MODER_OUT);
	GPIOD_OTYPER &= ~(1<<LED_1);
	//LED2
	GPIOD_MODER = REP_BITS(GPIOD_MODER, LED_2*2, 2, GPIO_MODER_OUT);
	GPIOD_OTYPER &= ~(1<<LED_2);
	//LED3
	GPIOD_MODER = REP_BITS(GPIOD_MODER, LED_3*2, 2, GPIO_MODER_OUT);
	GPIOD_OTYPER &= ~(1<<LED_3);
	//LED4
	GPIOD_MODER = REP_BITS(GPIOD_MODER, LED_4*2, 2, GPIO_MODER_OUT);
	GPIOD_OTYPER &= ~(1<<LED_4);
	//LED5
	GPIOD_MODER = REP_BITS(GPIOD_MODER, LED_5*2, 2, GPIO_MODER_OUT);
	GPIOD_OTYPER &= ~(1<<LED_5);
	//LED6
	GPIOD_MODER = REP_BITS(GPIOD_MODER, LED_6*2, 2, GPIO_MODER_OUT);
	GPIOD_OTYPER &= ~(1<<LED_6);

	init_TIM4();

	// main loop
	printf("Endless loop!\n");
	int i = 1;
	int allum = 1;
	while(1) {
		if(TIM4_triggered){
			printf("start interr");
			TIM4_triggered = 0;

			if (allum == 1){	//allumer des LEDS
				printf("allum");
				switch (i){
					case 1:
						printf("1");
						GPIOD_BSRR = 1 << LED_1;
						i ++;
						break;
					case 2:
						printf("2");
						GPIOD_BSRR = 1 << LED_2;
						i ++;
						break;
					case 3:
						printf("3");
						GPIOD_BSRR = 1 << LED_3;
						i ++;
						break;
					case 4:
						printf("4");
						GPIOD_BSRR = 1 << LED_4;
						i ++;
						break;
					case 5:
						printf("5");
						GPIOD_BSRR = 1 << LED_5;
						i ++;
						break;
					case 6:
						printf("6");
						GPIOD_BSRR = 1 << LED_6;
						allum = 0;
						break;
				}
			}
			else{				//eteindre des LEDS
				printf("eteind");
				switch (i)
				{
					case 6:
						GPIOD_BSRR = 1 << (LED_6 + 16);
						i--;
						break;
					case 5:
						GPIOD_BSRR = 1 << (LED_5 + 16);
						i--;
						break;
					case 4:
						GPIOD_BSRR = 1 << (LED_4 + 16);
						i--;
						break;
					case 3:
						GPIOD_BSRR = 1 << (LED_3 + 16);
						i--;
						break;
					case 2:
						GPIOD_BSRR = 1 << (LED_2 + 16);
						i--;
						break;
					case 1:
						GPIOD_BSRR = 1 << (LED_1 + 16);
						allum = 1;
						break;
				}
			}
		}
	}
}