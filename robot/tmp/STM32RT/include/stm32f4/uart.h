#ifndef STM32F4_UART_H
#define STM32F4_UART_H

#include "io.h"

// registers
#define USART1_BASE        0x40011000
#define USART1_REG(o)      _IOREG(USART1_BASE, o)
#define USART1_SR         USART1_REG(0x00)
#define USART1_DR         USART1_REG(0x04)
#define USART1_BRR          USART1_REG(0x08)
#define USART1_CR1        USART1_REG(0x0C)
#define USART1_CR2            USART1_REG(0x10)
#define USART1_CR3            USART1_REG(0x14)
#define USART1_GTPR            USART1_REG(0x18)


//USART_CR1
#define USART_CR1_UE    (1 << 13)
#define USART_CR1_SBK   (1 << 0)
#define USART_CR1_TE    (1 << 3)
#define USART_CR1_RE    (1 << 2)

//USART_SR
#define USART_SR_TC     (1 << 6)
#define USART_SR_TXE    (1 << 7)
#define USART_SR_RXNE   (1 << 5)

//USART_CR2
#define USART_CR2_CLKEN (1 << 11)
#define USART_CR2_STOP1 (1 << 12)
#define USART_CR2_STOP2 (1 << 13)

//USART_CR3
#define USART_CR3_HDSEL (1 << 3)
#define USART_CR3_RTSE  (1 << 8)
#define USART_CR3_CTSE  (1 << 9)

// interrupts
#define USART1_ER_IRQ   32
#define USART1_EV_IRQ   31

#endif