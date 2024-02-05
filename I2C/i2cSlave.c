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
#define SDA 7
#define SCL 8



/*************************************************
* function declarations
*************************************************/


// void delay_ms(uint32_t ms) {

//     TIM2_CR1 = 0;
//     TIM2_PSC = APB1_CLK / 1000 - 1;
//     TIM2_ARR = 41999;
//     TIM2_EGR = TIM_UG;
//     TIM2_SR = 0;
//     TIM2_CR1 = TIM_CEN;
//     for (uint32_t i = 0; i < ms; ++i) {
//         // Wait until the update event occurs (1ms)
//         while ((TIM2_SR & TIM_UIF)==0) {
//             // Wait
//         }
//         printf("sorti UIF \n");
//         // Clear the update event flag
//         TIM2_ARR = 41999;
//         TIM2_SR = 0;
//     }

//     // Disable the timer
//     TIM2_CR1 = 0;
// }


static void
my_delay_1( void )
{
   int i = 72e6/2/4;

   while( i > 0 )
     {
        i--;
        __asm__( "nop" );
     }
}

#define MYSLAVE_ADDRESS 0x09
//Set Commands
#define MYSLAVE_SET_REG 0x01
//GET commands
#define MYSLAVE_GET_ADD_RESULT 0x02
#define MYSLAVE_GET_SUB_RESULT 0x03
#define MYSLAVE_GET_MUL_RESULT 0x04

void I2C1_ER_IRQHandler(){
    // error handler
    GPIOD_BSRR = (1 << 14); // red LED
    if (I2C1_SR1 & I2C_SR1_BERR) {
        printf("Bus Error \n");
    }

    if (I2C1_SR1 & I2C_SR1_AF) {
        printf("AF \n");
        // Acknowledge Failure
    }
    (void)I2C1_SR1;
    (void)I2C1_SR2;
    //delay_ms(500);
    printf("sorti delay");
    GPIOD_BSRR = (1 << (16+14)); 
}

volatile uint8_t *read_p;
volatile uint8_t *write_p;
volatile uint8_t writing;
volatile uint8_t reading;

volatile uint8_t buf[3];
volatile uint16_t val;

//i2c1 event ISR
void i2c1_ev_isr(void)
{
   uint32_t sr1, sr2;

   sr1 = I2C1_SR1;

   // Address matched (Slave)
   printf("TestADDR \n");
   if (sr1 & I2C_SR1_ADDR)
     {
        reading = 0;
        read_p = buf;
        write_p = ((volatile uint8_t *)(&val) + 1);
        writing = 2;
        //Clear the ADDR sequence by reading SR2.

        sr2 = I2C1_SR2;
        (void) sr2;
     }
   // Receive buffer not empty
   else if (sr1 & I2C_SR1_RXNE)
     {
        //ignore more than 3 bytes reading
        if (reading > 3)
          return;
        //read bytes from slave
        *read_p++ = I2C1_DR & 0xff;
        reading++;
     }
   // Transmit buffer empty & Data byte transfer not finished
   else if ((sr1 & I2C_SR1_TXE) && !(sr1 & I2C_SR1_BTF))
     {
        //send data to master in MSB order
        I2C1_DR = *write_p--;
        writing--;
     }
   // done by master by sending STOP
   //this event happens when slave is in Recv mode at the end of communication
   else if (sr1 & I2C_SR1_STOPF)
     {
        I2C1_CR1 |=I2C_CR1_PE;

        if (buf[0] == MYSLAVE_GET_ADD_RESULT)
          val = buf[1] + buf[2];
        else if (buf[0] == MYSLAVE_GET_SUB_RESULT)
          val = buf[1] - buf[2];
        else if (buf[0] == MYSLAVE_GET_MUL_RESULT)
          val = buf[1] * buf[2];
     }
   //this event happens when slave is in transmit mode at the end of communication
   else if (sr1 & I2C_SR1_AF)
     {
        //(void) I2C_SR1(I2C1);
        I2C1_SR1 &= ~(I2C_SR1_AF);
     }
}


void init_NVIC(){
	DISABLE_IRQS;

	//NVIC ER
	NVIC_ICER(I2C1_ER_IRQ >> 5) = 1 << (I2C1_ER_IRQ & 0X1f);
	NVIC_IRQ(I2C1_ER_IRQ) = (uint32_t)I2C1_ER_IRQHandler;
	NVIC_IPR(I2C1_ER_IRQ) = 0;

    //NVIC EV
    NVIC_ICER(I2C1_EV_IRQ >> 5) = 1 << (I2C1_EV_IRQ & 0X1f);
	NVIC_IRQ(I2C1_EV_IRQ) = (uint32_t)i2c1_ev_isr;
	NVIC_IPR(I2C1_EV_IRQ) = 0;


	//purge IRQ
	NVIC_ICPR(I2C1_ER_IRQ >> 5) = 1<<(I2C1_ER_IRQ & 0X1f);

    NVIC_ICPR(I2C1_EV_IRQ >> 5) = 1<<(I2C1_EV_IRQ & 0X1f);

	//enable IRQ
	NVIC_ISER(I2C1_ER_IRQ >> 5) = 1 <<(I2C1_ER_IRQ & 0X1f);

    NVIC_ISER(I2C1_EV_IRQ >> 5) = 1 <<(I2C1_EV_IRQ & 0X1f);
	//start
	ENABLE_IRQS;



}

static void
i2c_slave_init(uint8_t ownaddress)
{
    RCC_AHB1ENR |= RCC_GPIOBEN;
    RCC_APB1ENR |= RCC_I2C1EN;

    init_NVIC();

    // configure i2c pins
    GPIOB_MODER = REP_BITS(GPIOB_MODER, SCL*2, 2, GPIO_MODER_ALT);
    GPIOB_AFRH = REP_BITS(GPIOB_AFRH, 0*4, 4, 4);
    // GPIOB_PUPDR = REP_BITS(GPIOB_PUPDR, SCL*2 , 2, GPIO_PUPDR_PD);
    //GPIOB_OTYPER &= (1<<SCL);

    GPIOB_MODER = REP_BITS(GPIOB_MODER, SDA*2, 2, GPIO_MODER_ALT);
    GPIOB_AFRL = REP_BITS(GPIOB_AFRL, 7*4, 4, 4);
    // GPIOB_PUPDR = REP_BITS(GPIOB_PUPDR, SDA*2 , 2, GPIO_PUPDR_PD);
    //GPIOB_OTYPER &= (1<<SDA);

    I2C1_CR1 |= I2C_CR1_SWRST;
    I2C1_CR1 = 0;


    //set speed
    uint16_t reg16;
	reg16 = I2C1_CR2 & 0xffc0; /* Clear bits [5:0]. */
	reg16 |= 45;
	I2C1_CR2 = reg16;

    I2C1_CCR &= ~(1 << 15);

	reg16 = I2C1_CCR & 0xf000; /* Clear bits [11:0]. */
	reg16 |= 225;
	I2C1_CCR = reg16;
    
	/* Sm mode, (100kHz) freqMhz + 1 */
	I2C1_TRISE = 46;

    I2C1_OAR1 = (ownaddress << 1)|(1 << 14);


    I2C1_CR2 |= I2C_CR2_ITERREN|I2C_CR2_ITEVTEN|I2C_CR2_ITBUFFEN;
    I2C1_CR1 |=I2C_CR1_PE|I2C_CR1_ACK;
 // enable i2c

    // slave needs to acknowledge on receiving bytes
    // set it after enabling Peripheral i.e. PE = 1
}




int main( void )
{

   // Enable GPIOC clock
   RCC_AHB1ENR |= RCC_GPIODEN;
   
    GPIOD_MODER = REP_BITS(GPIOD_MODER, GREEN_LED*2, 2, GPIO_MODER_OUT);
	GPIOD_OTYPER &= ~(1<<GREEN_LED);

   //initialize i2c slave
   i2c_slave_init(MYSLAVE_ADDRESS);

   while( 1 )
     {
        if (I2C1_SR1 & I2C_SR1_ADDR){
            printf("recu addresse");
        }
        GPIOD_BSRR = (1<<GREEN_LED);
        my_delay_1();
        GPIOD_BSRR = (1<<(16+GREEN_LED));
        my_delay_1();
     }
}