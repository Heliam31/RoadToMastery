
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
#include "RccConfig.h"
#include "Delay.h"
#include "I2C.h"


void MPU_Write (uint8_t Address, uint8_t Reg, uint8_t Data)
{
	I2C_Start ();
	I2C_Address (Address);
	I2C_Write (Reg);
	I2C_Write (Data);
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


#define MPU6050_ADDR 0xD0


#define SMPLRT_DIV_REG 0x19
#define GYRO_CONFIG_REG 0x1B
#define ACCEL_CONFIG_REG 0x1C
#define ACCEL_XOUT_H_REG 0x3B
#define TEMP_OUT_H_REG 0x41
#define GYRO_XOUT_H_REG 0x43
#define PWR_MGMT_1_REG 0x6B
#define WHO_AM_I_REG 0x75


int16_t Accel_X_RAW = 0;
int16_t Accel_Y_RAW = 0;
int16_t Accel_Z_RAW = 0;

int16_t Gyro_X_RAW = 0;
int16_t Gyro_Y_RAW = 0;
int16_t Gyro_Z_RAW = 0;

float Ax, Ay, Az, Gx, Gy, Gz;

uint8_t check;

void MPU6050_Init (void)
{
	uint8_t check;
	uint8_t Data;

	// check device ID WHO_AM_I

	MPU_Read (MPU6050_ADDR,WHO_AM_I_REG, &check, 1);

	if (check == 104)  // 0x68 will be returned by the sensor if everything goes well
	{
		// power management register 0X6B we should write all 0's to wake the sensor up
		Data = 0;
		MPU_Write (MPU6050_ADDR, PWR_MGMT_1_REG, Data);

		// Set DATA RATE of 1KHz by writing SMPLRT_DIV register
		Data = 0x07;
		MPU_Write(MPU6050_ADDR, SMPLRT_DIV_REG, Data);

		// Set accelerometer configuration in ACCEL_CONFIG Register
		// XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> ? 2g
		Data = 0x00;
		MPU_Write(MPU6050_ADDR, ACCEL_CONFIG_REG, Data);

		// Set Gyroscopic configuration in GYRO_CONFIG Register
		// XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=0 -> ? 250 ?/s
		Data = 0x00;
		MPU_Write(MPU6050_ADDR, GYRO_CONFIG_REG, Data);
	}

}

void MPU6050_Read_Accel (void)
{
	
	uint8_t Rx_data[6];
	
	// Read 6 BYTES of data starting from ACCEL_XOUT_H register

	MPU_Read (MPU6050_ADDR, ACCEL_XOUT_H_REG, Rx_data, 6);

	Accel_X_RAW = (int16_t)(Rx_data[0] << 8 | Rx_data [1]);
	Accel_Y_RAW = (int16_t)(Rx_data[2] << 8 | Rx_data [3]);
	Accel_Z_RAW = (int16_t)(Rx_data[4] << 8 | Rx_data [5]);

	/*** convert the RAW values into acceleration in 'g'
	     we have to divide according to the Full scale value set in FS_SEL
	     I have configured FS_SEL = 0. So I am dividing by 16384.0
	     for more details check ACCEL_CONFIG Register              ****/

	Ax = Accel_X_RAW/16384.0;
	Ay = Accel_Y_RAW/16384.0;
	Az = Accel_Z_RAW/16384.0;
}


int main ()
{
	SysClockConfig ();
	TIM6Config ();
	I2C_Config ();	
	
	MPU6050_Init ();
	while (1)
	{
		MPU6050_Read_Accel ();
		Delay_ms (1000);
	}
}

