
#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/nvic.h>
#include <stm32f4/exti.h>
#include <stm32f4/syscfg.h>
#include <stm32f4/tim.h>
#include <stm32f4/i2c.h>
#include <stm32f4/adc.h>

#define OPEN_DRAIN
//#define NOPULLUP
#define OTHER_PINS

/*
 * Oscilo: Agilent Technologies
 */

/*
 * Good link
 * 
 * https://controllerstech.com/stm32-i2c-configuration-using-registers/
 *
 * SDA/SCL reset https://electronics.stackexchange.com/questions/65719/i2c-slave-address-not-acknowledged-sometimes
 *
 * Arduino+STM32 https://circuitdigest.com/microcontroller-projects/stm32-stm32f103c8-i2c-communication-tutorial
 */

/* Arduino
 * SCL top-right
 * SDA top-right-1
 * 
 * A4(SDA), A5(SCL)
 */

/* I2C configuration */
#ifdef OTHER_PINS
#	define SCL		8		// PB6, I2C1, ALT4
#	define SDA		9		// PB7, I2C1, ALT4
#else
#	define SCL		6		// PB6, I2C1, ALT4
#	define SDA		7		// PB7, I2C1, ALT4
#endif
#define SLAVE	0x33
#define COMMAND	0x66

/**
 * Configure I2C1
 */
void i2c_config() {

	// prepare GPIO
	GPIOB_MODER = REP_BITS(GPIOB_MODER, 2*SCL, 2, GPIO_MODER_ALT);
	GPIOB_MODER = REP_BITS(GPIOB_MODER, 2*SDA, 2, GPIO_MODER_ALT);
#	ifdef OPEN_DRAIN
		printf("Open drain!\n");
		GPIOB_OTYPER |= (1 << SCL);
		GPIOB_OTYPER |= (1 << SDA);
#		ifdef NOPULLUP
			printf("No pullup!\n");
			GPIOB_PUPDR = REP_BITS(GPIOB_PUPDR, 2*SCL, 2, GPIO_PUPDR_NO);
			GPIOB_PUPDR = REP_BITS(GPIOB_PUPDR, 2*SDA, 2, GPIO_PUPDR_NO);
#		else
			printf("Pullup!\n");
			GPIOB_PUPDR = REP_BITS(GPIOB_PUPDR, 2*SCL, 2, GPIO_PUPDR_PU);
			GPIOB_PUPDR = REP_BITS(GPIOB_PUPDR, 2*SDA, 2, GPIO_PUPDR_PU);
#		endif
#	else
		printf("No open drain!\n");
		GPIOB_OTYPER &= ~(1 << SCL);
		GPIOB_OTYPER &= ~(1 << SDA);
		GPIOB_PUPDR = REP_BITS(GPIOB_PUPDR, 2*SCL, 2, GPIO_PUPDR_NO);
		GPIOB_PUPDR = REP_BITS(GPIOB_PUPDR, 2*SDA, 2, GPIO_PUPDR_NO);
#	endif
	GPIOB_OSPEEDR = REP_BITS(GPIOB_OSPEEDR, 2*SCL, 2, GPIO_OSPEEDR_VH);
	GPIOB_OSPEEDR = REP_BITS(GPIOB_OSPEEDR, 2*SDA, 2, GPIO_OSPEEDR_VH);
#	ifdef OTHER_PINS
		printf("Other pins!\n");
		GPIOB_AFRH = REP_BITS(GPIOB_AFRH, 4*(SDA - 8), 4, 4);
		GPIOB_AFRH = REP_BITS(GPIOB_AFRH, 4*(SCL - 8), 4, 4);
#	else
		printf("Normal pins!\n");
		GPIOB_AFRL = REP_BITS(GPIOB_AFRL, 4*SDA, 4, 4);
		GPIOB_AFRL = REP_BITS(GPIOB_AFRL, 4*SCL, 4, 4);
#	endif

	/**
	 * # Timing (from parameter sheet)
	 *	Tw(SCLH) = 4000ns
	 *	Tr(SCL) = 1000ns
	 * 	TPCLK1 = 1/APB1_CLK = 24ns
	 * 
	 * # Configuration
	 * 	CR2 <- APB1_CLK / 1 000 000
	 * 	CCR	<- (Tr(SCL) + Tw(SCLH)) / TPCLK1
	 * 		= (4000 + 1000)/24 = 208
	 * 	TRISE <- Tr(SCL)/TPCLK1 +1
	 * 			= 1000/24 + 1 = 42
	 *  CR1.PE <- 1
	 *
	 * # Generate START
	 * 	CR1.ACK <- 1
	 * 	CR1.START <- 1
	 * 	wait for SR1.SB = 1
	 *
	 * # Send ADDR
	 *	DR <- ADDR
	 * 	wait for SR1.ADDR = 1
	 * 	x <- SR1
	 *	x <- SR2
	 * 
	 * # Write byte DATA
	 *	wait for SR1.TXE = 1
	 * 	DR <- DATA
	 * 	wait for SR1.BTF = 1
	 *
	 * #  Read 1 bye DATA
	 *	CR1.ACK <- 0
	 * 	x <- SR1
	 * 	x <- SR2
	 * 
	  * # Stop
	  *	
	  */

	// reset I2C
	I2C1_CR1 |= I2C_CR1_SWRST;
	I2C1_CR1 &= ~I2C_CR1_SWRST;
	
	// configure the line
	I2C1_CR1 = 0;
	I2C1_CR2 = APB1_CLK / 1000000;
	I2C1_CCR = 208;
	I2C1_TRISE = 42;
	I2C1_CR1 |= I2C_CR1_PE;
}

/**
 * Send a message over I2C1.
 * @param addr	Target address.
 * @param data	Bytes to send.
 * @param size	Number of bytes to send.
 */
void 
i2c_send(uint8_t addr, uint8_t *data, int size) {

	// emit START bit
	I2C1_CR1 |= I2C_CR1_ACK;
	I2C1_CR1 |= I2C_CR1_START;
	while((I2C1_SR1 & I2C_SR1_SB) == 0);

	// emit address
	I2C1_DR = addr << 1;
	while((I2C1_SR1 & I2C_SR1_ADDR) == 0);
	int x = I2C1_SR1;
	x = I2C1_SR2;

	// write data
	while(size != 0) {
		while((I2C1_SR1 & I2C_SR1_TXE) == 0);
		I2C1_DR = *data++;
		size--;
	}
	while((I2C1_SR1 & I2C_SR1_BTF) == 0);

	// emit STOP
	I2C1_CR1 |= I2C_CR1_STOP;
}


/**
 * Receive data from I2C1.
 * @param addr	Target addresse.
 * @param data	Buffer to store data.
 * @param size	Expected data size.
 */
void i2c_receive(uint8_t addr, uint8_t *data, int size) {

	// emit START bit
	I2C1_CR1 |= I2C_CR1_ACK;
	I2C1_CR1 |= I2C_CR1_START;
	while((I2C1_SR1 & I2C_SR1_SB) == 0);

	// emit address
	I2C1_DR = (addr << 1) | 1;
	while((I2C1_SR1 & I2C_SR1_ADDR) == 0);
	int x = I2C1_SR1;
	x = I2C1_SR2;

	// read data except last 1
	while(size > 1) {
		while((I2C1_SR1 & I2C_SR1_RXNE) == 0);
		*data++ = I2C1_DR;
		size--;
		if(size > 1)
			I2C1_CR1 = I2C_CR1_ACK;			
	}

	// clear ACK and stop I2C
	I2C1_CR1 &= ~I2C_CR1_ACK;
	I2C1_CR1 |= I2C_CR1_STOP;

	// read last byte
	while((I2C1_SR1 & I2C_SR1_RXNE) == 0);
	*data++ = I2C1_DR;
}



/****** TLS2561 ******/

#define TSL2561_ADDR	0x39
//#define TSL2561_ADDR	0x29

/**
 * Initialize TSL2561 on I2C1.
 */
void tsl2561_init() {
	uint8_t control[2] = { 0x80, 0b11 };
	uint8_t timing[2] = { 0x81, 0b01 };
	printf("Initialize TSL2561\n");
	printf("- control\n");
	i2c_send(TSL2561_ADDR, control, 2);
	printf("- timing\n");
	i2c_send(TSL2561_ADDR, timing, 2);
}

/**
 * Get word data for channel c.
 * @param c		Channel to get (in [0, 1]).
 * @return		Obtained data.
 */
int tsl2561_get_channel(int c) {
	uint8_t reg[2] = { 0xAC + c*2 };
	uint8_t data[2];
	i2c_send(TSL2561_ADDR, reg, 1);
	i2c_receive(TSL2561_ADDR, data, 2);
	return (data[1] << 8) | data[0];
}


/****** Arduino communication ******/

#define ARDUINO_ADDR	0x33

void arduino_init() {	
	uint8_t control[2] = { 0x80, 0b11 };
	uint8_t timing[2] = { 0x81, 0b01 };
	i2c_send(ARDUINO_ADDR, control, 2);
	i2c_send(ARDUINO_ADDR, timing, 2);
}

int arduino_get_channel(int c) {
	uint8_t reg[2] = { 0xAC + c*2 };
	uint8_t data[2];
	i2c_send(ARDUINO_ADDR, reg, 1);
	i2c_receive(ARDUINO_ADDR, data, 2);
	return (data[1] << 8) | data[0];
}

int main() {

	RCC_APB1ENR |= RCC_I2C1EN | RCC_TIM4EN;
	RCC_AHB1ENR |= RCC_GPIOBEN;
	
	TIM4_CR1 = 0;
	TIM4_PSC = 1000 - 1;
	TIM4_ARR = APB1_CLK / 1000;
	TIM4_EGR = TIM_UG;
	TIM4_SR = 0;
	TIM4_CR1 = TIM_CEN;
	
	printf("\nConfiguration\n");
	i2c_config();
	//tsl2561_init();
	arduino_init();

	//arduino_send(0x66);

	printf("Loop\n");
	while(1) {
		/*int x = tsl2561_get_channel(0);
		int y = tsl2561_get_channel(1);
		printf("Channel 0: %05d, Channel 1: %05d\n", x, y);*/
		// int x = arduino_get_channel(0);
		// printf("Channel 0: %04x\n", x);
		uint8_t reg[2] = { 1, 0b0101};
		i2c_send(ARDUINO_ADDR, reg , 2);

		while((TIM4_SR & TIM_UIF) == 0);
		TIM4_SR = 0;
	}
	while(1);
}
