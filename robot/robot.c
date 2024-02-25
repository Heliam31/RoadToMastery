#include "pid.h"
#include "qtr8rc.h"
#include "motor_driver.h"
#include "color_sens.h"
#include "sonar.h"

#include "utils.h"

// PROTOTYPES
void init_tim7_and_interrupts();

// GLOBALS
unsigned int distance = 0;
volatile int TIM7_triggered = 0;

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
    // color_init();
    
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

    for (int i = 0; i < 30; i++) {
        qtr8rc_calibrate();
        delay_ms(3);
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
    calibrate_ir();

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
    Direction direction = BACK;
    roads[BACK] = 1;

    start();

    while(1) {
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
        // printf("left: %d | right: %d\n",leftSpeed, rightSpeed);
        // display_direction(direction);
        // display_state(state);
        motor_set_speeds(leftSpeed, rightSpeed);

        if (TIM7_triggered) {
            sonar_read(&distance);
            if (distance < 11) state = STOP;
            // printf("distance: %d\n",distance);
            TIM7_triggered = 0;
        }
    }
}
