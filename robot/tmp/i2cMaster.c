
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

// GPIOD
#define GREEN_LED	12
#define ORANGE_LED	13
#define RED_LED		14
#define BLUE_LED	15

//I2C ports (GPIOB)
#define SDA 9
#define SCL 8

volatile uint8_t DeviceAddr = 0x32;

/*************************************************
* function declarations
*************************************************/

void delay_ms(uint32_t ms) {
    // Set the prescaler value for 1ms ticks
    TIM2_PSC = APB1_CLK / 1000 - 1;

    // Set the auto-reload value for 1ms
    TIM2_ARR = 71999;

    // Enable the timer
    TIM2_CR1 |= TIM_CEN;

    for (uint32_t i = 0; i < ms; ++i) {
        // Wait until the update event occurs (1ms)
        while ((TIM2_SR & TIM_UIF)==0) {
            // Wait
        }
        printf("sorti UIF \n");
        // Clear the update event flag
        TIM2_ARR = 71999;
        TIM2_SR = 0;
    }

    // Disable the timer
    TIM2_CR1 &= ~TIM_CEN;
}

static inline void __i2c_start() {
    uint16_t reg;
    I2C1_CR1 |= I2C_CR1_START;
    while(!(I2C1_SR1 & I2C_SR1_SB));
    printf("%d\n", I2C1_SR1); 
    
    printf("STarted\n");
    reg = I2C1_SR1;
    (void) reg;
}

static inline void __i2c_stop() {
    I2C1_CR1 |= I2C_CR1_STOP;
    while(!(I2C1_SR2 & I2C_SR2_BUSY));
}

void i2c_write(uint8_t regaddr, uint8_t data) {
    // send start condition
    
    printf("avant start\n");
    __i2c_start();
    
    // send chipaddr in write mode
    // wait until address is sent
    
    I2C1_DR = (0x09 << 1) | 0;
    while (!(I2C1_SR1 & I2C_SR1_ADDR));//{ 
    //uint8_t temp = I2C1_SR1 | I2C1_SR2;  // read SR1 and SR2 to clear the ADDR bit
    //printf("%d\n", I2C1_SR1);};
    // dummy read to clear flags
    printf("sorti while1\n");
    
    (void)I2C1_SR2; // clear addr condition
    // send MAP byte with auto increment off
    // wait until byte transfer complete (BTF)
    I2C1_DR = data;
    while (!(I2C1_SR1 & I2C_SR1_TXE));
    printf("sorti while2");
    // send data
    // wait until byte transfer complete
    I2C1_DR = data;
    while (!(I2C1_SR1 & I2C_SR1_BTF));
    printf("sorti while3");
    // send stop condition
    __i2c_stop();
}

uint8_t i2c_read(uint8_t regaddr) {
    uint8_t reg;

    // send start condition
    __i2c_start();
    // send chipaddr in write mode
    // wait until address is sent
    I2C1_DR = DeviceAddr;
    while (!(I2C1_SR1 & I2C_SR1_ADDR));
    // dummy read to clear flags
    (void)I2C1_SR2; // clear addr condition

    // send MAP byte with auto increment off
    // wait until byte transfer complete (BTF)
    I2C1_DR = regaddr;
    while (!(I2C1_SR1 & I2C_SR1_BTF));

    // restart transmission by sending stop & start
    __i2c_stop();
    __i2c_start();

    // send chipaddr in reaGPIOB -> PUPDR |= GPIO_PUPDR_PUPD6_0 | GPIO_PUPDR_PUPD7_0;d mode. LSB is 1
    // wait until address is sent
    I2C1_DR = DeviceAddr | 0x01; // read
    while (!(I2C1_SR1 & I2C_SR1_ADDR));
    // dummy read to clear flags
    (void)I2C1_SR2; // clear addr condition

    // wait until receive buffer is not empty
    while (!(I2C1_SR1 & I2C_SR1_RXNE));
    // read content
    reg = (uint8_t)I2C1_DR;

    // send stop condition
    __i2c_stop();

    return reg;
}

void I2C1_ER_IRQHandler(){
    // erroe the APB1 Peripheral Clock is at 45 MHz, and that’s the r handler
    GPIOD_BSRR = (1 << 14); // red LED
    if (I2C1_SR1 & I2C_SR1_BERR) {
        printf("Bus Error \n");
    }

    if (I2C1_SR1 & I2C_SR1_AF) {
        printf("AF \n");
        // Acknowledge Failure
        __i2c_stop();
    }
    (void)I2C1_SR1;
    (void)I2C1_SR2;
    //delay_ms(500);
    printf("sorti delay");
    GPIOD_BSRR = (1 << (16+14)); 
}

void init_NVIC(){
	DISABLE_IRQS;

	//NVIC
	NVIC_ICER(I2C1_ER_IRQ >> 5) = 1 << (I2C1_ER_IRQ & 0X1f);
	NVIC_IRQ(I2C1_ER_IRQ) = (uint32_t)I2C1_ER_IRQHandler;
	NVIC_IPR(I2C1_ER_IRQ) = 0;

	//purge IRQ
	NVIC_ICPR(I2C1_ER_IRQ >> 5) = 1<<(I2C1_ER_IRQ & 0X1f);

	//enable IRQ
	NVIC_ISER(I2C1_ER_IRQ >> 5) = 1 <<(I2C1_ER_IRQ & 0X1f);

	//start
	ENABLE_IRQS;

}

/*************************************************
* main code starts from here
*************************************************/
int main(void)
{
    
    RCC_APB1ENR |= RCC_TIM2EN;
    RCC_AHB1ENR |= RCC_GPIODEN;
    RCC_AHB1ENR |= RCC_GPIOBEN;
    RCC_APB1ENR |= RCC_I2C1EN;

    // for(int i = 12; i<=15; i++){
	// GPIOD_MODER = REP_BITS(GPIOD_MODER, i*2, 2, GPIO_MODER_OUT);
	// GPIOD_OTYPER &= ~(1<<i);
	// }
    
    GPIOB_MODER = REP_BITS(GPIOB_MODER, SCL*2, 2, GPIO_MODER_ALT);
    GPIOB_AFRL = REP_BITS(GPIOB_AFRL, 6*4, 4, 4);
    //GPIOB_OSPEEDR = REP_BITS(GPIOB_OSPEEDR, SCL*2, 2, 3);
    //GPIOB_PUPDR = REP_BITS(GPIOB_PUPDR, SCL*2 , 2, GPIO_PUPDR_PU);
    //GPIOB_OTYPER |= (1<<SCL);

    GPIOB_MODER = REP_BITS(GPIOB_MODER, SDA*2, 2, GPIO_MODER_ALT);
    GPIOB_AFRL = REP_BITS(GPIOB_AFRL, 7*4, 4, 4);
    //GPIOB_OSPEEDR = REP_BITS(GPIOB_OSPEEDR, SDA*2, 2, 3);
    //GPIOB_PUPDR = REP_BITS(GPIOB_PUPDR, SDA*2 , 2, GPIO_PUPDR_PU);
    //GPIOB_OTYPER |= (1<<SDA);

    // reset and clear reg
    //I2C1_CR2 = 42;

    I2C1_CR1 |= I2C_CR1_SWRST;
    I2C1_CR1 = 0;

    // I2C1_CR2 |= (I2C_CR2_ITERREN); // enable error interrupt
<<<<<<< HEAD
    //I2C1_CR2 &= 0xffc0;
    I2C1_CR2 = 42; // 10Mhz periph clock
    //I2C1_CCR &= 0xf000;
    //I2C1_CCR |= 42;
    I2C1_CCR = 0xd2; // 210d * ((1/42)*1000)
    I2C1_TRISE = 43;
=======

<<<<<<< HEAD
    I2C1_CR2 = 42; // 10Mhz periph clock
    I2C1_CCR = 208; // 210d * ((1/42)*1000)
    I2C1_TRISE = 42;
=======
    I2C1_CR2 = 45; // 10Mhz periph clock
    I2C1_CCR = 225; // 210d * ((1/42)*1000)
    I2C1_TRISE = 46;
>>>>>>> 02e533c4d9c33d1916b802c30c126fcc52003598
>>>>>>> 0785943827771453b64a91d0ac490e3df45791be
    // enable error interrupt from NVIC

    //init_NVIC();

    I2C1_CR1 |=I2C_CR1_PE; // enable i2c
    
    while(1)
    {
        i2c_write(DeviceAddr, 0x00);
        //GPIOD_BSRR = (1 << 13); // orange led on error
    }
    return 0;
}
