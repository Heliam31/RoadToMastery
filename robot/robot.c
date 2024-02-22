#include "pid.h"
#include "qtr8rc.h"
#include "motor_driver.h"
#include "color_sens.h"
#include "sonar.h"

#include "utils.h"


// GLOBALS

// INIT
void enable_clk(void) {
    RCC_AHB1ENR |= RCC_GPIOAEN;
    RCC_AHB1ENR |= RCC_GPIOBEN;
	RCC_AHB1ENR |= RCC_GPIOCEN;
	RCC_AHB1ENR |= RCC_GPIODEN;

	RCC_APB1ENR |= RCC_TIM2EN;
	RCC_APB1ENR |= RCC_TIM3EN;
    RCC_APB1ENR |= RCC_TIM4EN;
    RCC_APB1ENR |= RCC_TIM5EN;
	RCC_APB1ENR |= RCC_TIM6EN;
	RCC_APB1ENR |= RCC_TIM7EN; // for sync

    RCC_APB2ENR |= RCC_ADC1EN;
}

void init(void) {
    qtr8rc_init();
    motor_init();
    sonar_init();
    color_init();
    
    init_tim6();
    init_tim7();
    led_init(GREEN_LED);
    led_init(ORANGE_LED);
    led_init(RED_LED);
    led_init(BLUE_LED);
    button_init(BUTTON);
}

void calibration(void) {
    printf("Waiting for calibration !\n");
    led_turn_on(GREEN_LED);
    button_wait(BUTTON);
    led_turn_off(GREEN_LED);

    display_calMinValues();
    display_calMaxValues();

    printf("Calibrating...\n");

    motor_set_speeds(-26, 26);

    for (int i = 0; i < 250; i++) {
        qtr8rc_calibrate();
        // printf("DELAY MS\n");
        delay_ms(3);
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
        *leftSpeed = -17;
        *rightSpeed = 17;
        break;
    case FRONT:
        led_turn_on(BLUE_LED);
        *leftSpeed = 17;
        *rightSpeed = 17;
        break;
    case LEFT:
        led_turn_on(RED_LED);
        *leftSpeed = -17;
        *rightSpeed = 17;
        break;
    case RIGHT:
        led_turn_on(GREEN_LED);
        *leftSpeed = 17;
        *rightSpeed = -17;
        break;
    default:
        break;
    }
}

int on_road(Direction *direction, int *irValues) {
    switch (*direction) {
    case BACK:
        if (irValues[0] > 1000 && irValues[1] > 1000) {
            return 1;
        }
        break;
    case FRONT:
        if (irValues[6] > 1000 && irValues[7] > 1000) {
            return 1;
        }
        break;
    case LEFT:
        if (irValues[0] > 1000 && irValues[7] > 1000) {
            return 1;
        }
        break;
    case RIGHT:
        if (irValues[6] > 1000 && irValues[1] > 1000) {
            return 1;
        }
        break;
    default:
        break;
    }
    return 0;
}

int on_junction(int *roads){
    return roads[LEFT] | roads[RIGHT]; 
}

int main (void) {
    enable_clk();
    init();
    calibration();

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
    Direction direction = FRONT;
    roads[BACK] = 1;

    unsigned int distance = 0;
    int isR = 0, isG = 0, isB = 0;

    led_turn_on(GREEN_LED);
    button_wait(BUTTON);
    led_turn_off(GREEN_LED);

    int read_frequency = 0; // Sonar and colors read frequency

    while(1) {
        start_sync(DEFAULT_HYPERPERIOD);

        // IR READS
        qtr8rc_read(irValues, OFF);

        // printf("->%d \n",position);
        // display_irValues(irValues);
        
        switch(state) {
            case FOLLOW:
                compute_position(&position, irValues);
                pid_compute_speeds(&leftSpeed, &rightSpeed, &position);
                get_avaible_roads(roads, irValues);
                // printf("->%d \n",position);
                // display_irValues(irValues);
                // if ((on_junction(roads) == 0) && !stop) {
                //     compute_position(&position, irValues);
                //     pid_compute_speeds(&leftSpeed, &rightSpeed, position);
                //     // printf("%d %d %d\n", position, leftSpeed, rightSpeed);
                // } else 
                if ((on_junction(roads) == 0) && stop) {
                    state = STOP;
                    for (int i = 0; i < 4; i++) {
                        // tmsg->data[i] = roads[i];
                    }
                    stop = 0;
                } else if ((on_junction(roads) == 1)) {
                    stop = 1;
                }
                break;

            case STOP:
                leftSpeed = 0;
                rightSpeed = 0;
                // i2c_send(tmsg); // TODO ???
                // i2c_receive(rmsg); // TODO ??? 
                // get_direction(&direction, rmsg); // TODO
                if (direction == FRONT) {
                    state = FOLLOW;
                } else {
                    state = TURN;
                }
                break;
            
            case TURN:
                // delay_ms(50);
                button_wait(BUTTON);
                move_on_line(&leftSpeed, &rightSpeed, &direction);
                if (direction == LEFT) {
                    state = S_LEFT;
                } else if (direction == RIGHT) {
                    state = S_RIGHT;
                } else {
                    state = S_BACK;
                }
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
                // if (direction != LEFT) {
                //     state = S_LEFT;
                // } else
                if (irValues[7] == 1000) {
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
                    motor_set_speeds(-8, -8);
                    state = FOLLOW;
                }
                break;
            
            case CHECK5:
                motor_set_speeds(-8, -8);
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

        // OTHER READS
        if(read_frequency == 5) {
            sonar_read(&distance);
            color_read(&isG, &isB, &isR);
            read_frequency = 0;
        }
        // while (1);
        // printf("distance : %d\n",distance);
        // printf("%d | %d | %d\n",isR,isG,isB);
        read_frequency++;
        sync();
    }
}
