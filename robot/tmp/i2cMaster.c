
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

// these should go to cs43l22 related headers
#define ADDRESS 0x55
#define REGID  0x01
#define CHIPID 0x1C // first 5 bits of reg

volatile uint8_t DeviceAddr = 1;

/*************************************************
* function declarations
*************************************************/

static inline void __i2c_start() {

    I2C1_CR1 |= I2C_CR1_START;
    while(!(I2C1_SR1 & I2C_SR1_SB));
}

static inline void __i2c_stop() {
    I2C1_CR1 |= I2C_CR1_STOP;
    while(!(I2C1_SR2 & I2C_SR2_BUSY));
}

void i2c_write(uint8_t regaddr, uint8_t data) {
    // send start condition
    printf("avant start\n");
    __i2c_start();
    printf("started\n");
    // send chipaddr in write mode
    // wait until address is sent
    I2C1_DR = (DeviceAddr<<1) | 0x00;
    while (!(I2C1_SR1 & I2C_SR1_ADDR));
    // dummy read to clear flags
    (void)I2C1_SR2; // clear addr condition
    printf("sorti while1\n");
    // send MAP byte with auto increment off
    // wait until byte transfer complete (BTF)
    I2C1_DR = regaddr;
    while (!(I2C1_SR1 & I2C_SR1_BTF));
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

    // send chipaddr in read mode. LSB is 1
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
    // error handler
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
    /* set system clock to 168 Mhz */

    RCC_AHB1ENR |= RCC_GPIODEN;
    RCC_AHB1ENR |= RCC_GPIOBEN;
    RCC_APB1ENR |= RCC_I2C1EN;

    for(int i = 12; i<=15; i++){
	GPIOD_MODER = REP_BITS(GPIOD_MODER, i*2, 2, GPIO_MODER_OUT);
	GPIOD_OTYPER &= ~(1<<i);
	}

    
    GPIOB_MODER = REP_BITS(GPIOB_MODER, SCL*2, 2, GPIO_MODER_ALT);
    GPIOB_AFRH = REP_BITS(GPIOB_AFRH, 0*4, 4, 4);
    GPIOB_PUPDR = REP_BITS(GPIOB_PUPDR, SCL*2 , 2, GPIO_PUPDR_PD);
    GPIOB_OTYPER &= (1<<SCL);

    GPIOB_MODER = REP_BITS(GPIOB_MODER, SDA*2, 2, GPIO_MODER_ALT);
    GPIOB_AFRH = REP_BITS(GPIOB_AFRH, 1*4, 4, 4);
    GPIOB_PUPDR = REP_BITS(GPIOB_PUPDR, SDA*2 , 2, GPIO_PUPDR_PD);
    GPIOB_OTYPER &= (1<<SDA);

    // reset and clear reg
    //I2C1_CR2 = 42;

    // I2C1_CR1 |= I2C_CR1_SWRST;
    // I2C1_CR1 &= ~I2C_CR1_SWRST;

    I2C1_CR2 |= (I2C_CR2_ITERREN); // enable error interrupt

    // I2C1_CR2 |= (10 << 0); // 10Mhz periph clock
    // I2C1_CCR |= (50 << 0);

    // I2C1_TRISE |= (11 << 0); // program TRISE to 11 for 100khz

    // // I2C1_OAR1 |= (0x00 << 1);
    I2C1_OAR1 |= (1 << 14); // bit 14 should be kept at 1 according to the datasheet
    I2C1_CR2 |= 0x30;
    I2C1_CCR = 0x8028;
    I2C1_TRISE = 0xf;
    // enable error interrupt from NVIC

    init_NVIC();

    I2C1_CR1 |=I2C_CR1_PE; // enable i2c

    

    while(1)
    {
        GPIOD_BSRR = (1 << 13); // orange led on error
        i2c_write(DeviceAddr, 134569);
    }
    return 0;
}
