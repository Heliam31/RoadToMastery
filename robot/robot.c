#include "pid.h"
#include "qtr8rc.h"
#include "motor_driver.h"
#include "utils.h"
#include "I2C.h"


#define NB_ROADS 4

// INIT
void enable_clk(void) {
    RCC_AHB1ENR |= RCC_GPIOAEN;
	RCC_AHB1ENR |= RCC_GPIODEN;
	RCC_AHB1ENR |= RCC_GPIOCEN;

	RCC_APB1ENR |= RCC_TIM2EN;
	RCC_APB1ENR |= RCC_TIM3EN;
    RCC_APB1ENR |= RCC_TIM4EN;
	RCC_APB1ENR |= RCC_TIM6EN;

    RCC_APB1ENR |= RCC_I2C1EN | RCC_TIM5EN;
	RCC_AHB1ENR |= RCC_GPIOBEN;
    RCC_APB2ENR |= RCC_ADC1EN;
}

void init(void) {
    qtr8rc_init();
    motor_init();
    
    init_tim6();
    led_init(GREEN_LED);
    led_init(ORANGE_LED);
    led_init(RED_LED);
    led_init(BLUE_LED);
    button_init(BUTTON);
    i2c_config(8, 9);

}

void calibration(void) {
    led_turn_on(GREEN_LED);
    button_wait(BUTTON);
    led_turn_off(GREEN_LED);

    // display_calMinValues();
    // display_calMaxValues();

    printf("Calibrating...\n");

    motor_set_speeds(-25, 25);

    for (int i = 0; i < 250; i++) {
        qtr8rc_calibrate();
        // printf("DELAY MS\n");
        delay_ms(1);
        // printf("MS DOWN\n");
    }

    display_calMinValues();
    display_calMaxValues();

    printf("Ready !\n");

    motor_set_speeds(0, 0);
    // delay_ms(1000); // 1s
}

void move_on_line(int *leftSpeed, int *rightSpeed, Direction *direction) {
    switch (*direction) {
    case BACK:
        led_turn_on(ORANGE_LED);
        *leftSpeed = -15;
        *rightSpeed = 15;
        break;
    case FRONT:
        led_turn_on(BLUE_LED);
        *leftSpeed = 15;
        *rightSpeed = 15;
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

int main (void) {
    enable_clk();
    init();
    //calibration();

    int irValues [8] = {0};
    int sonarValue = 0;

    int roads [4] = {0};
    int position = 0;

    int leftSpeed = 0;
    int rightSpeed = 0;

    int stop = 0;

    // msg_t *tmsg = {ESP32_ADDR, {0}, 8};
    // msg_t *rmsg = {ESP32_ADDR, {0}, 8};

    State state = FOLLOW;
    Direction direction = LEFT;
    roads[BACK] = 1;

    led_turn_on(GREEN_LED);
    button_wait(BUTTON);
    led_turn_off(GREEN_LED);

    int tmpRoads[NB_ROADS] = {0};

    while(1) {
        
        qtr8rc_read(irValues, OFF);

        //compute_position(&position, irValues);
        
        
        state = STOP;
        switch(state) {
            case FOLLOW:
                pid_compute_speeds(&leftSpeed, &rightSpeed, &position);
                get_avaible_roads(roads, irValues);
                if ((on_junction(roads) == 0) && stop) {
                    state = STOP;
                    motor_disable(M_LEFT); leftSpeed = 0;
                    motor_disable(M_RIGHT); rightSpeed = 0;
                    stop = 0;
                } else if ((on_junction(roads) == 1)) {
                    tmpRoads[LEFT] = roads[LEFT];
                    tmpRoads[RIGHT] = roads[RIGHT];
                    stop = 1;
                }
                break;

            case STOP:
                // delay_ms(50);
                //motor_disable(M_LEFT); leftSpeed = 0;
                //motor_disable(M_RIGHT); rightSpeed = 0;
                //button_wait(BUTTON);
                ///////////////I2C AJOUT/////////////////
                uint8_t reg_send[2] = { 1, 0b0101}; //reg à send
                uint8_t reg_receive[1] = {0}; //reg pour demander des données
                printf("Loop\n");
                i2c_send(ESP32_ADDR, reg_send , 2); //on send des données
                printf("bien send\n");
                i2c_receive(ESP32_ADDR, reg_receive, 1); //on demande des données
                printf("receive\n");
                while((TIM5_SR & TIM_UIF) == 0);
         		TIM5_SR = 0;
                /////////////////////////////////////////
                direction = choose_direction(reg_receive[0]);
                printf("La direction est : %d\n", direction);
                if (direction == FRONT) {
                    state = FOLLOW;
                } else {
                    state = TURN;
                }
                state = STOP;
                break;
            
            case TURN:
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
                if (irValues[0] == 1000) {
                    state = CHECK2;
                }
                break;
            
            case S_LEFT:
                if (irValues[7] == 1000) {
                    state = CHECK7;
                }
                break;
            
            case S_BACK:
                if (tmpRoads[LEFT] != 1) {
                    tmpRoads[LEFT] = 0;
                    tmpRoads[RIGHT] = 0;
                    state = S_LEFT;
                } else if (irValues[7] == 1000) {
                    state = CHECK8_WHITE;
                }
                break;
            
            case CHECK2:
                if (irValues[1] == 1000) {
                    state = CHECK3;
                }
                break;
            
            case CHECK3:
                if (irValues[2] == 1000) {
                    state = CHECK4;
                }
                break;
            
            case CHECK4:
                if (irValues[3] == 1000) {
                    state = FOLLOW;
                }
                break;
            
            case CHECK5:
                if (irValues[4] == 1000) {
                    state = FOLLOW;
                }
                break;
            
            case CHECK6:
                if (irValues[5] == 1000) {
                    state = CHECK5;
                }
                break;
            
            case CHECK7:
                if (irValues[6] == 1000) {
                    state = CHECK6;
                }
                break;
            
            case CHECK8_WHITE:
                if (irValues[7] < 1000) {
                    state = S_LEFT;
                }
                break;
        }
        // display_irValues(irValues);
        // printf("%d:", position);
        // printf("%d;%d\n", leftSpeed, rightSpeed);
        // display_direction(direction);
        // display_state(state);

        motor_set_speeds(leftSpeed, rightSpeed);


    }
}