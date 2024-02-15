#include "pid.h"
#include "qtr8rc.h"
#include "motor_driver.h"
#include "utils.h"


// GLOBALS

// INIT
void init(void) {
    timer_init();
    led_init(GREEN_LED);
    led_init(ORANGE_LED);
    led_init(RED_LED);
    led_init(BLUE_LED);
    button_init(BUTTON);
    qtr8rc_init();
    motor_init();
}

void move_on_line(Direction *direction) {
    switch (*direction) {
    case BACK:
        led_turn_on(ORANGE_LED);
        set_speed_left(-16);
        set_speed_right(16);
        break;
    case FRONT:
        led_turn_on(BLUE_LED);
        set_speed_left(16);
        set_speed_right(16);
        break;
    case LEFT:
        led_turn_on(RED_LED);
        set_speed_left(-16);
        set_speed_right(16);
        break;
    case RIGHT:
        led_turn_on(GREEN_LED);
        set_speed_left(16);
        set_speed_right(-16);
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

void enable_clk(void) {
    RCC_AHB1ENR |= RCC_GPIOAEN;
	RCC_AHB1ENR |= RCC_GPIODEN;
	RCC_AHB1ENR |= RCC_GPIOCEN;

	RCC_APB1ENR |= RCC_TIM2EN;
	RCC_APB1ENR |= RCC_TIM3EN;
    RCC_APB1ENR |= RCC_TIM4EN;
	RCC_APB1ENR |= RCC_TIM6EN;

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
}

void calibration(void) {
    led_turn_on(GREEN_LED);
    button_wait(BUTTON);
    led_turn_off(GREEN_LED);

    printf("Calibrating...\n");

    motor_set_speeds(-18, 18);

    for (size_t i = 0; i < 25; i++) {
        qtr8rc_calibrate();
        delay_ms(20);
    }

    printf("Ready !\n");

    motor_set_speeds(0, 0);
    delay_ms(1000); // 1s
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

    msg_t *tmsg = {ESP32_ADDR, {0}, 8};
    msg_t *rmsg = {ESP32_ADDR, {0}, 8};

    State state = FOLLOW;
    Direction direction = FRONT;
    roads[BACK] = 1;

    led_turn_on(GREEN_LED);
    button_wait(BUTTON);
    led_turn_off(GREEN_LED);
    while(1) {
        start_timer(); // TODO
        
        qtr8rc_read(&irValues, OFF);
        read_sonar(&sonarValue); // TODO ???
        
        delay_ms(1);
        
        switch(state) {
            case FOLLOW:
                get_avaible_roads(&roads, &irValues);
                if (on_junction(&roads) == 1) { // TODO
                    state = STOP;
                    leftSpeed = 0;
                    rightSpeed = 0;
                    for (int i = 0; i < 4; i++) {
                        tmsg->data[i] = roads[i];
                    }
                } else {
                    compute_position(&position, &irValues);
                    pid_compute_speeds(&leftSpeed, &rightSpeed, &position);
                }
                break;
            
            case STOP:
                leftSpeed = 0;
                rightSpeed = 0;
                i2c_send(tmsg); // TODO ???
                i2c_receive(rmsg); // TODO ??? 
                get_direction(&direction, rmsg); // TODO
                if (direction == FRONT) {
                    state = FOLLOW;
                } else {
                    state = TURN;
                }
                break;
            
            case TURN:
                move_on_line(&direction);
                if (direction == LEFT) {
                    state = S_LEFT;
                } else if (direction == RIGHT) {
                    state = S_RIGHT;
                } else {
                    state = S_BACK;
                }
                break;

            case S_LEFT:
                if (irValues[0] == 1) {
                    state = CHECK2;
                }
                break;
            
            case S_RIGHT:
                if (irValues[7] == 1) {
                    state = CHECK7;
                }
                break;
            
            case S_BACK:
                if (direction != RIGHT) {
                    state = S_RIGHT;
                } else if (irValues[7] == 1) {
                    state = CHECK8_WHITE;
                }
                break;
            
            case CHECK2:
                if (irValues[1] == 1) {
                    state = CHECK3;
                }
                break;
            
            case CHECK3:
                if (irValues[2] == 1) {
                    state = CHECK4;
                }
                break;
            
            case CHECK4:
                if (irValues[3] == 1) {
                    state = FOLLOW;
                }
                break;
            
            case CHECK5:
                if (irValues[4] == 1) {
                    state = FOLLOW;
                }
                break;
            
            case CHECK6:
                if (irValues[5] == 1) {
                    state = CHECK5;
                }
                break;
            
            case CHECK7:
                if (irValues[6] == 1) {
                    state = CHECK6;
                }
                break;
            
            case CHECK8_WHITE:
                if (irValues[7] == 0) {
                    state = S_RIGHT;
                }
                break;
        }
        set_speeds(&leftSpeed, &rightSpeed);
     
        sync(); // TODO
    }
}
