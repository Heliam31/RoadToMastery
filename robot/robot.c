
#include "pid.h"
#include "qtr8rc.h"
#include "motor_driver.h"
#include "utils.h"


// GLOBALS
int position = 0;
int junctions[4] = {0};
int motorLeftSpeed = 25;
int motorRightSpeed = 25;

int stop = 0;
State state = FOLLOW;

msg_t tmsg = {ESP32_ADDR, {0}, 8};
msg_t rmsg = {ESP32_ADDR, {0}, 8};


Direction direction = RIGHT;

// UTILS 
void _robot_delay(int cycles) {
    for(int i = 0; i < cycles; i++) NOP;
}

void robot_wait_seconds(float seconds) {
    int cycles = seconds*APB1_CLK;
    _robot_delay(cycles);
}

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

// MISC
void calibrate(void) {
    led_turn_on(GREEN_LED);
    button_wait(BUTTON);
    led_turn_off(GREEN_LED);

    printf("Calibrating...\n");

    set_speed_left(-18);
    set_speed_right(18);

    for (size_t i = 0; i < 25; i++) {
        qtr8rc_calibrate();
        robot_wait_seconds(0.2);
    }

    // display_calMinValues();
    // display_calMaxValues();

    printf("Ready !\n");

    set_speed_left(0);
    set_speed_right(0);

    robot_wait_seconds(1);
}

void robot_stop() {
    // arrete des moteurs
    // printf("Do nothing!\n");
    set_speed_left(0);
    set_speed_right(0);
}

void robot_follow_line() {
    qtr8rc_read_calibrated(&position, junctions);
    if ((junctions[2] | junctions[3]) == 1) {
        state = JONCTION;
    } else {
        compute_motor_speed(&motorLeftSpeed, &motorRightSpeed, position);
        set_speed_left(motorLeftSpeed);
        set_speed_right(motorRightSpeed);
    }
}

void robot_move_on_line() {
    switch (direction) {
    case BACK:
        led_turn_on(ORANGE_LED);
        set_speed_left(-16);
        set_speed_right(16);
        for (int i=0; i < 40000000; i++) NOP;
        set_speed_left(0);
        set_speed_right(0);
        break;
    case FRONT:
        led_turn_on(BLUE_LED);
        break;
    case LEFT:
        led_turn_on(RED_LED);
        set_speed_left(-16);
        set_speed_right(16);
        for (int i=0; i < 20500000; i++) NOP;
        set_speed_left(0);
        set_speed_right(0);
        break;
    case RIGHT:
        led_turn_on(GREEN_LED);
        set_speed_left(16);
        set_speed_right(-16);
        for (int i=0; i < 20500000; i++) NOP;
        set_speed_left(0);
        set_speed_right(0);
        break;
    default:
        break;
    }
}

void capteurs_read(void) {
    
    qtr8rc_read_calibrated(&position, junctions);
        
} 

int main(void) {
    printf("\n\n\nInitialization...\n");

    RCC_AHB1ENR |= RCC_GPIOAEN;
	RCC_AHB1ENR |= RCC_GPIODEN;
	RCC_AHB1ENR |= RCC_GPIOCEN;

	RCC_APB1ENR |= RCC_TIM2EN;
	RCC_APB1ENR |= RCC_TIM3EN;
    RCC_APB1ENR |= RCC_TIM4EN;
	RCC_APB1ENR |= RCC_TIM6EN;

    RCC_APB2ENR |= RCC_ADC1EN;

    init();
    calibrate();

    led_turn_on(GREEN_LED);
    button_wait(BUTTON);
    led_turn_off(GREEN_LED);
    printf("Start !\n");

    junctions[BACK] = 1;

    while(1){
        // switch (state) {
        // case FOLLOW:
        //     robot_follow_line();
        //     break;

        // case JONCTION:
        //     // verifie si il y a une ligne apres le croisement
        //     // printf("Checking front...\n");
        //     qtr8rc_read_calibrated(&position, junctions);
        //     // printf("BACK:%d;FRONT:%d;LEFT:%d;RIGHT:%d\n", junctions[0], junctions[1], junctions[2], junctions[3]);

        //     // attendre que les roues soient au niveau du croisement avant de stopper 
        //     for (int i = 0; i < 200; i++) sync();
            
        //     // arrete des moteurs
        //     set_speed_left(0);
        //     set_speed_right(0);
            
        //     for (int i = 0; i < 1000; i++) sync();
            
        //     // tmsg->data = junctions;
        //     // tmsg->size = 4;
        //     // printf("Sending jucntions...\n");
        //     // // envoyer les junctions detectees au serveur
        //     // i2c_send(tmsg);
        //     // printf("Jucntions sent, waiting direction...\n");
        //     // // lire la direction a prendre pour continuer
        //     // i2c_read(rmsg);
        //     // Direction direction = RIGHT; // decode_msg(rmsg);
        //     // if (direction == RIGHT) direction = LEFT;
        //     // else if (direction == LEFT) direction = BACK;
        //     // else if (direction == RIGHT) direction = RIGHT;
        //     // direction = decode_msg(rmsg);

        //     // deplacer le robot pour qu'il match avec la ligne selon la direction
        //     // printf("Moving on the line...\n");
        //     robot_move_on_line(direction);
        //     // printf("Ready to follow the line !\n");

        //     // printf("Stop motor\n");
        //     set_speed_left(0);
        //     set_speed_right(0);
        //     // wait_button();

        //     // clear junctions
        //     // printf("Clear junctions\n");
        //     junctions[LEFT] = 0;
        //     junctions[RIGHT] = 0;
            
        //     // printf("Stop !\n");
        //     state = FOLLOW;
        //     break;
        
        // case STOP:
        //     robot_stop();

        // default:
        //     break;
        // }

        timer_reset();
        timer_enable();
    
        capteurs_read();

        timer_sync();

        switch (state) {
        case FOLLOW:
            robot_follow_line();
            break;
        case JONCTION:
            robot_move_on_line();
            state = STOP;
            break;
        case STOP:
            robot_stop();
            button_wait(BUTTON);
            state = FOLLOW;
            break;
        default:
            break;
        }

        timer_sync();
    }
}
