
#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/nvic.h>
#include <stm32f4/exti.h>
#include <stm32f4/syscfg.h>
#include <stm32f4/tim.h>
#include <stm32f4/i2c.h>
#include <stm32f4/adc.h>

void TIM5Config (void)
{
	/************** STEPS TO FOLLOW *****************
	1. Enable Timer clock
	2. Set the prescalar and the ARR
	3. Enable the Timer, and wait for the update Flag to set
	************************************************/

// 1. Enable Timer clock
	RCC_APB1ENR |= RCC_TIM5EN;  // Enable the timer6 clock
	
// 2. Set the prescalar and the ARR
	TIM5_PSC = 90-1;  // 90MHz/90 = 1 MHz ~~ 1 uS delay
	TIM5_ARR = 0xffff;  // MAX ARR value
	
// 3. Enable the Timer, and wait for the update Flag to set
	TIM5_CR1 |= (1<<0); // Enable the Counter
	while (!(TIM5_SR & (1<<0)));  // UIF: Update interrupt flag..  This bit is set by hardware when the registers are updated
}

void Delay_us (uint16_t us)
{
	/************** STEPS TO FOLLOW *****************
	1. RESET the Counter
	2. Wait for the Counter to reach the entered value. As each count will take 1 us, 
		 the total waiting time will be the required us delay
	************************************************/
	TIM5_CNT = 0;
	while (TIM5_CNT < us);
}

void Delay_ms (uint16_t ms)
{
	for (uint16_t i=0; i<ms; i++)
	{
		Delay_us (1000); // delay of 1 ms
	}
}



void I2C_Config (void)
{
/**** STEPS FOLLOWED  ************
1. Enable the I2C CLOCK and GPIO CLOCK
2. Configure the I2C PINs for ALternate Functions
	a) Select Alternate Function in MODER Register
	b) Select Open Drain Output 
	c) Select High SPEED for the PINs
	d) Select Pull-up for both the Pins
	e) Configure the Alternate Function in AFR Register
3. Reset the I2C 	
4. Program the peripheral input clock in I2C_CR2 Register in order to generate correct timings
5. Configure the clock control registers
6. Configure the rise time register
7. Program the I2C_CR1 register to enable the peripheral
*/
	
	// Enable the I2C CLOCK and GPIO CLOCK
	RCC_APB1ENR |= RCC_I2C1EN;  // enable I2C CLOCK
	RCC_AHB1ENR |= RCC_GPIOBEN;  // Enable GPIOB CLOCK
	
	#define SDA 9
	#define SCL 8
	// Configure the I2C PINs for ALternate Functions
	GPIOB_MODER = REP_BITS(GPIOB_MODER, 8*2, 2, GPIO_MODER_ALT);
    GPIOB_AFRH = REP_BITS(GPIOB_AFRH, 0*4, 4, 4);

    GPIOB_MODER = REP_BITS(GPIOB_MODER, 9*2, 2, GPIO_MODER_ALT);
    GPIOB_AFRH = REP_BITS(GPIOB_AFRH, 1*4, 4, 4);
	
	
	// Reset the I2C 
	I2C1_CR1 |= I2C_CR1_SWRST;
	I2C1_CR1 &= ~I2C_CR1_SWRST;
	
	// Program the peripheral input clock in I2C_CR2 Register in order to generate correct timings
	I2C1_CR2 |= (42<<0);  // PCLK1 FREQUENCY in MHz
	
	// Configure the clock control registers
	I2C1_CCR = 208<<0;  // check calculation in PDF
	
	// Configure the rise time register
	I2C1_TRISE = 43;  // check PDF again
	
	// Program the I2C_CR1 register to enable the peripheral
	I2C1_CR1 |= I2C_CR1_PE;  // Enable I2C
}

void I2C_Start (void)
{
/**** STEPS FOLLOWED  ************
1. Send the START condition 
2. Wait for the SB ( Bit 0 in SR1) to set. This indicates that the start condition is generated
*/	

	I2C1_CR1 |= I2C_CR1_ACK;  // Enable the ACK
	I2C1_CR1 |= I2C_CR1_START;  // Generate START
	while (!(I2C1_SR1 & I2C_SR1_SB));  // Wait fror SB bit to set
}


void I2C_Write (uint8_t data)
{
/**** STEPS FOLLOWED  ************
1. Wait for the TXE (bit 7 in SR1) to set. This indicates that the DR is empty
2. Send the DATA to the DR Register
3. Wait for the BTF (bit 2 in SR1) to set. This indicates the end of LAST DATA transmission
*/	
	while (!(I2C1_SR1 & I2C_SR1_TXE));  // wait for TXE bit to set
	I2C1_DR = data;
	while (!(I2C1_SR1 & I2C_SR1_BTF));  // wait for BTF bit to set
}

void I2C_Address (uint8_t Address)
{
/**** STEPS FOLLOWED  ************
1. Send the Slave Address to the DR Register
2. Wait for the ADDR (bit 1 in SR1) to set. This indicates the end of address transmission
3. clear the ADDR by reading the SR1 and SR2
*/	
	I2C1_DR = Address;  //  send the address
	while (!(I2C1_SR1 & I2C_SR1_ADDR));  // wait for ADDR bit to set
	uint8_t temp = I2C1_SR1 | I2C1_SR2;  // read SR1 and SR2 to clear the ADDR bit
}
	
void I2C_Stop (void)
{
	I2C1_CR1 |= I2C_CR1_STOP;  // Stop I2C
}

void I2C_WriteMulti (uint8_t *data, uint8_t size)
{
/**** STEPS FOLLOWED  ************
1. Wait for the TXE (bit 7 in SR1) to set. This indicates that the DR is empty
2. Keep Sending DATA to the DR Register after performing the check if the TXE bit is set
3. Once the DATA transfer is complete, Wait for the BTF (bit 2 in SR1) to set. This indicates the end of LAST DATA transmission
*/	
	while (!(I2C1_SR1 & (1<<7)));  // wait for TXE bit to set 
	while (size)
	{
		while (!(I2C1_SR1 & (1<<7)));  // wait for TXE bit to set 
		I2C1_DR = (uint32_t )*data++;  // send data
		size--;
	}
	
	while (!(I2C1_SR1 & (1<<2)));  // wait for BTF to set
}

void I2C_Read (uint8_t Address, uint8_t *buffer, uint8_t size)
{
/**** STEPS FOLLOWED  ************
1. If only 1 BYTE needs to be Read
	a) Write the slave Address, and wait for the ADDR bit (bit 1 in SR1) to be set
	b) the Acknowledge disable is made during EV6 (before ADDR flag is cleared) and the STOP condition generation is made after EV6
	c) Wait for the RXNE (Receive Buffer not Empty) bit to set
	d) Read the data from the DR

2. If Multiple BYTES needs to be read
  a) Write the slave Address, and wait for the ADDR bit (bit 1 in SR1) to be set
	b) Clear the ADDR bit by reading the SR1 and SR2 Registers
	c) Wait for the RXNE (Receive buffer not empty) bit to set
	d) Read the data from the DR 
	e) Generate the Acknowlegment by settint the ACK (bit 10 in SR1)
	f) To generate the nonacknowledge pulse after the last received data byte, the ACK bit must be cleared just after reading the 
		 second last data byte (after second last RxNE event)
	g) In order to generate the Stop/Restart condition, software must set the STOP/START bit 
	   after reading the second last data byte (after the second last RxNE event)
*/		
	
	int remaining = size;
	
/**** STEP 1 ****/	
	if (size == 1)
	{
		/**** STEP 1-a ****/	
		I2C1_DR = Address;  //  send the address
		while (!(I2C1_SR1 & (1<<1)));  // wait for ADDR bit to set
		
		/**** STEP 1-b ****/	
		I2C1_CR1 &= ~(1<<10);  // clear the ACK bit 
		uint8_t temp = I2C1_SR1 | I2C1_SR2;  // read SR1 and SR2 to clear the ADDR bit.... EV6 condition
		I2C1_CR1 |= (1<<9);  // Stop I2C

		/**** STEP 1-c ****/	
		while (!(I2C1_SR1 & (1<<6)));  // wait for RxNE to set
		
		/**** STEP 1-d ****/	
		buffer[size-remaining] = I2C1_DR;  // Read the data from the DATA REGISTER
		
	}

/**** STEP 2 ****/		
	else 
	{
		/**** STEP 2-a ****/
		I2C1_DR = Address;  //  send the address
		while (!(I2C1_SR1 & (1<<1)));  // wait for ADDR bit to set
		
		/**** STEP 2-b ****/
		uint8_t temp = I2C1_SR1 | I2C1_SR2;  // read SR1 and SR2 to clear the ADDR bit
		
		while (remaining>2)
		{
			/**** STEP 2-c ****/
			while (!(I2C1_SR1 & (1<<6)));  // wait for RxNE to set
			
			/**** STEP 2-d ****/
			buffer[size-remaining] = I2C1_DR;  // copy the data into the buffer			
			
			/**** STEP 2-e ****/
			I2C1_CR1 |= 1<<10;  // Set the ACK bit to Acknowledge the data received
			
			remaining--;
		}
		
		// Read the SECOND LAST BYTE
		while (!(I2C1_SR1 & (1<<6)));  // wait for RxNE to set
		buffer[size-remaining] = I2C1_DR;
		
		/**** STEP 2-f ****/
		I2C1_CR1 &= ~(1<<10);  // clear the ACK bit 
		
		/**** STEP 2-g ****/
		I2C1_CR1 |= (1<<9);  // Stop I2C
		
		remaining--;
		
		// Read the Last BYTE
		while (!(I2C1_SR1 & (1<<6)));  // wait for RxNE to set
		buffer[size-remaining] = I2C1_DR;  // copy the data into the buffer
	}	
	
}



void MPU_Write (uint8_t Address, uint8_t Reg, uint8_t Data)
{
	printf("debut\n");
	printf ((void *)I2C1_SR1);
	I2C_Start ();
	printf("started\n");
	printf ((void *)I2C1_SR1);
	I2C_Address (Address);
	printf("adressé\n");
	printf ((void *)I2C1_SR1);
	I2C_Write (Reg);
	printf("reged\n");
	printf ((void *)I2C1_SR1);
	I2C_Write (Data);
	printf("ecrit\n");
	printf ((void *)I2C1_SR1);
	I2C_Stop ();
}

void MPU_Read (uint8_t Address, uint8_t Reg, uint8_t *buffer, uint8_t size)
{
	I2C_Start ();
	I2C_Address (Address);
	I2C_Write (Reg);
	I2C_Start ();  // repeated start
	I2C_Read (Address+0x01, buffer, size);
	I2C_Stop ();
}


#define MPU6050_ADDR 0x09
#define REG 		 0x01 		

uint8_t check;

// void MPU6050_Init (void)
// {
// 	uint8_t check;
// 	uint8_t Data;

// 	// check device ID WHO_AM_I

// 	MPU_Read (MPU6050_ADDR,WHO_AM_I_REG, &check, 1);

// 	if (check == 104)  // 0x68 will be returned by the sensor if everything goes well
// 	{
// 		// power management register 0X6B we should write all 0's to wake the sensor up
// 		Data = 0;
// 		MPU_Write (MPU6050_ADDR, PWR_MGMT_1_REG, Data);

// 		// Set DATA RATE of 1KHz by writing SMPLRT_DIV register
// 		Data = 0x07;
// 		MPU_Write(MPU6050_ADDR, SMPLRT_DIV_REG, Data);

// 		// Set accelerometer configuration in ACCEL_CONFIG Register
// 		// XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> ? 2g
// 		Data = 0x00;
// 		MPU_Write(MPU6050_ADDR, ACCEL_CONFIG_REG, Data);

// 		// Set Gyroscopic configuration in GYRO_CONFIG Register
// 		// XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=0 -> ? 250 ?/s
// 		Data = 0x00;
// 		MPU_Write(MPU6050_ADDR, GYRO_CONFIG_REG, Data);
// 	}

// }



int main ()
{
	RCC_AHB1ENR |= RCC_GPIODEN;
	GPIOD_MODER = REP_BITS(GPIOD_MODER, 12*2, 2, GPIO_MODER_OUT);
	GPIOD_OTYPER &= ~(1<<12);
	printf("1\n");
	TIM5Config ();
	printf("2\n");
	I2C_Config ();
	printf("3\n");	
	
	// MPU6050_Init ();
	// printf("4\n");
	while (1)
	{
		GPIOD_BSRR = (1<<12);
		MPU_Write (MPU6050_ADDR, REG, 0x08);
		Delay_ms (500);
		GPIOD_BSRR = (1<<(16+12));
		Delay_ms (500);
		
	}
}

