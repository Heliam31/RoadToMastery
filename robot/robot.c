#include "pid.h"
#include "qtr8rc.h"
#include "motor_driver.h"
#include "color_sens.h"
#include "sonar.h"

#include "utils.h"
#include "I2C.h"


#define LINE 2000

// PROTOTYPES
void init_tim7_and_interrupts();

#define NB_ROADS 4
unsigned int distance = 99999;
int isR = 0, isG = 0;
volatile int TIM7_triggered = 0;
int sonarFreq = 0;


// INIT
void enable_clk(void) {
    RCC_AHB1ENR |= RCC_GPIOAEN;
    RCC_AHB1ENR |= RCC_GPIOBEN;
	RCC_AHB1ENR |= RCC_GPIOCEN;
	RCC_AHB1ENR |= RCC_GPIODEN;

	RCC_APB1ENR |= RCC_TIM2EN; // SONAR 
	RCC_APB1ENR |= RCC_TIM3EN; // PWM
    RCC_APB1ENR |= RCC_TIM4EN; // IR
	RCC_APB1ENR |= RCC_TIM5EN; // I2C
	RCC_APB1ENR |= RCC_TIM6EN; // delays
	RCC_APB1ENR |= RCC_TIM7EN; // interruption
	// RCC_APB1ENR |= RCC_TIM12EN; // colors

    RCC_APB1ENR |= RCC_I2C1EN | RCC_TIM5EN;
	RCC_AHB1ENR |= RCC_GPIOBEN;
    RCC_APB2ENR |= RCC_ADC1EN;
}

void init(void) {
    qtr8rc_init();
    motor_init();
    sonar_init();
    color_init();
    i2c_config(8, 9);
    
    init_tim6();
    init_tim7_and_interrupts();
    led_init(GREEN_LED);
    led_init(ORANGE_LED);
    led_init(RED_LED);
    led_init(BLUE_LED);
    button_init(BUTTON);
}

// INTERUPTS
void handle_SONAR() {
    TIM7_SR = ~TIM_UIF;
    TIM7_triggered = 1;

    sonarFreq = (sonarFreq + 1) % 8;
}

void init_tim7_and_interrupts() {
	DISABLE_IRQS;

	NVIC_ICER(TIM7_IRQ >> 5) = 1 << (TIM7_IRQ & 0X1f);
	NVIC_IRQ(TIM7_IRQ) = (uint32_t)handle_SONAR;
	NVIC_IPR(TIM7_IRQ) = 0;

	// purge pending IRQ
	NVIC_ICPR(TIM7_IRQ >> 5) = 1 << (TIM7_IRQ & 0X1f);

    init_tim7();

	// enable IRQ
	NVIC_ISER(TIM7_IRQ >> 5) = 1 << (TIM7_IRQ & 0X1f);
	TIM7_DIER = TIM_UIE;
	
}

// CALIB
void calibrate_ir(void) {
    printf("Waiting for IR calibration !\n");
    led_turn_on(GREEN_LED);
    button_wait(BUTTON);
    led_turn_off(GREEN_LED);

    display_calMinValues();
    display_calMaxValues();

    printf("Calibrating...\n");

    motor_set_speeds(-26, 26);

    for (int i = 0; i < 20; i++) {
        qtr8rc_calibrate();
        delay_ms(2);
    }

    display_calMinValues();
    display_calMaxValues();

    printf("Ready !\n");

    motor_set_speeds(0, 0);
}

// MISC
void start(void) {
    // Start
    led_turn_on(GREEN_LED);
    button_wait(BUTTON);
    led_turn_off(GREEN_LED);

    // Start interruptions
	ENABLE_IRQS;
	TIM7_CR1 = TIM_CEN;

    // Reset distance value
    distance = 0;
}

// STATE MACHINE
void move_on_line(int *leftSpeed, int *rightSpeed, Direction *direction) {
    switch (*direction) 
{
    case BACK:
        led_turn_on(ORANGE_LED);
        *leftSpeed = -15;
        *rightSpeed = 15;
        break;
    case FRONT:
        led_turn_on(BLUE_LED);
        *leftSpeed = 25;
        *rightSpeed = 25;
        break;
    case LEFT:
        led_turn_on(RED_LED);
        *leftSpeed = -15;
        *rightSpeed = 15;
        break;
    case RIGHT:
        led_turn_on(GREEN_LED);
        *leftSpeed = 15;
        *rightSpeed = -15;
        break;
    default:
        break;
    }
}

int on_junction(int *roads){
    return roads[LEFT] | roads[RIGHT]; 
}

int on_road(int *irValues) {
    int onRoad = 0;
    for (int i = 0; i < 8; i++) {
        if (irValues[i] == LINE) {
            onRoad = 1;
            i = 8;
        }
    }
    return onRoad;
}

State choose_direction(int reg){
    if(reg == 1){
        return RIGHT;
    }else if (reg == 2)
    {
        return FRONT;
    }else if (reg == 4)
    {
        return LEFT;
    }else if (reg == 8)
    {
        return BACK;
    }
}

int convert_road_bits(int *roads){
    int res = 0;
    if(roads[LEFT] == 1){
        res += 4;
    }
    if(roads[RIGHT] == 1){
        res += 1;
    }
    if(roads[BACK] == 1){
        res += 8;
    }
    if(roads[FRONT] == 1){
        res += 2;
    }
    return res+16;
}

int main (void) {
    enable_clk();
    init();
    calibrate_ir();

    int irValues [8] = {0};
    int sonarValue = 0;

    int roads [4] = {0};
    int position = 0;

    int leftSpeed = 0;
    int rightSpeed = 0;

    int stop = 0;

    State state = FOLLOW;
    Direction direction = BACK;
    roads[BACK] = 1;

    start();

    int tmpRoads[NB_ROADS] = {0};

    // COM
    uint8_t reg_send[1] = {0}; //reg to send
    uint8_t reg_receive[1] = {0}; //reg to recieve

    while(1) {

        qtr8rc_read(irValues, OFF);
        compute_position(&position, irValues);
        
        switch(state) {
            case FOLLOW:
                pid_compute_speeds(&leftSpeed, &rightSpeed, &position);
                get_avaible_roads(roads, irValues);
                if ((on_junction(roads) == 0) && stop) {
                    state = IDLE;
                    motor_disable(M_LEFT); leftSpeed = 0;
                    motor_disable(M_RIGHT); rightSpeed = 0;
                    reg_send[0] = convert_road_bits(roads);
                    stop = 0;
                } else if ((on_junction(roads) == 1)) {
                    tmpRoads[LEFT] = roads[LEFT];
                    tmpRoads[RIGHT] = roads[RIGHT];
                    stop = 1;
                }
                break;

            case IDLE:
                DISABLE_IRQS;
                delay_ms(30);
                motor_disable(M_LEFT); leftSpeed = 0;
                motor_disable(M_RIGHT); rightSpeed = 0;
                // delay_ms(2000);
                // led_turn_on(BLUE_LED);
                // button_wait(BUTTON);
                
                ///////////// I2C AJOUT /////////////////
                printf("Loop\n");
                i2c_send(ESP32_ADDR, reg_send , 1); //on send des données
                // printf("bien send\n");
                i2c_receive(ESP32_ADDR, reg_receive, 1); //on demande des données
                // printf("receive\n");
                while((TIM5_SR & TIM_UIF) == 0);
         		TIM5_SR = 0;
                direction = choose_direction(reg_receive[0]);
                // printf("La direction est : %d\n", direction);
                /////////////////////////////////////////
                state = DECISION;

                // if (direction == FRONT) {
                //     if ((on_road(irValues) == 1) && roads[FRONT]) {
                //         state = FOLLOW;
                //     }
                // }
                // led_turn_on(BLUE_LED);
                // isR = 0;
                // isG = 0;
                // TIM7_triggered = 0;
                // sonarFreq = 0;
                // ENABLE_IRQS;
                
                break;

            case DECISION:
                isR = 0;
                isG = 0;
                TIM7_triggered = 0;
                sonarFreq = 0;
                ENABLE_IRQS;

                if (direction == LEFT) {
                    state = S_LEFT;
                } else if (direction == RIGHT) {
                    state = S_RIGHT;
                } else {
                    state = S_BACK;
                }
                move_on_line(&leftSpeed, &rightSpeed, &direction);
                break;

            case S_RIGHT:
                if (irValues[0] == LINE) {
                    state = CHECK2;
                }
                break;
            
            case S_LEFT:
                if (irValues[7] == LINE) {
                    state = CHECK7;
                }
                break;
            
            case S_BACK:
                if (tmpRoads[LEFT] != 1) {
                    tmpRoads[LEFT] = 0;
                    tmpRoads[RIGHT] = 0;
                    state = S_LEFT;
                } else if (irValues[7] == LINE) {
                    state = CHECK8_WHITE;
                }
                break;
            
            case CHECK2:
                if (irValues[1] == LINE) {
                    state = CHECK3;
                }
                break;
            
            case CHECK3:
                if (irValues[2] == LINE) {
                    state = CHECK4;
                }
                break;
            
            case CHECK4:
                if (irValues[3] == LINE) {
                    state = FOLLOW;
                }
                break;
            
            case CHECK5:
                if (irValues[4] == LINE) {
                    state = FOLLOW;
                }
                break;
            
            case CHECK6:
                if (irValues[5] == LINE) {
                    state = CHECK5;
                }
                break;
            
            case CHECK7:
                if (irValues[6] == LINE) {
                    state = CHECK6;
                }
                break;
            
            case CHECK8_WHITE:
                if (irValues[7] < LINE) {
                    state = S_LEFT;
                }
                break;
        }

        motor_set_speeds(leftSpeed, rightSpeed);

        if (TIM7_triggered) {
            if (sonarFreq >= 4) {
                init_tim2_sonar();
                sonar_read(&distance);
                if (distance <= 15 && distance > 0){
                    state = IDLE;
                    reg_send[0] = 0b0100000;
                }
            }

            else {
                color_read(&isR, &isG);
                if (isR && !isG){
                    state = IDLE;
                    reg_send[0] = 0b0110000; 
                }
            }

            TIM7_triggered = 0;
        }
    }
}