#ifndef STM32F4_I2C_H
#define STM32F4_I2C_H

#include "io.h"

// registers
#define I2C1_BASE		0x40005400
#define I2C_REG(o)		_IOREG(I2C_BASE, o)
#define I2C_CR1		    I2C_REG(0x00)
#define I2C_CR2		    I2C_REG(0x04)
#define I2C_OAR1		I2C_REG(0x08)
#define I2C_OAR2		I2C_REG(0x0C)
#define I2C_DR		    I2C_REG(0x10)
#define I2C_SR1			I2C_REG(0x14)
#define I2C_SR2			I2C_REG(0x18)
#define I2C_CCR			I2C_REG(0x1C)
#define I2C_TRISE		I2C_REG(0x20)
#define I2C_FLTR		I2C_REG(0x24)


//I2C_CR1
#define I2C_CR1_PE          (1 << 0)
#define I2C_CR1_START       (1 << 8)
#define I2C_CR1_STOP        (1 << 9)
#define I2C_CR1_SWRST       (1 << 15)

//I2C_CR2
#define I2C_CR2_ITERREN     (1 << 8)

//I2C_SR1
#define I2C_SR1_ADDR    (1 << 1)
#define I2C_SR1_BTF     (1 << 2)
#define I2C_SR1_RXNE    (1 << 6)
#define I2C_SR1_SB      (1 << 0)

//I2C_SR2
#define I2C_SR2_BUSY    (1 << 1)

//I2C_OAR1
#define I2C_OAR1_14        (1 << 14) //Always keep at 1

#endif