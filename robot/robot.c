
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
    init_timer_sync();
    init_gpio_led();
    init_gpio_button();
    qtr8rc_init();
    motor_init();
}

// MISC
void calibrate(void) {
    turn_on(GREEN_LED);
    wait_button();
    turn_off(GREEN_LED);

    printf("Calibrating...\n");

    set_speed_left(-18);
    set_speed_right(18);

    for (size_t i = 0; i < 25; i++) {
        qtr8rc_calibrate();
        robot_wait_seconds(0.2);
    }

    display_calMinValues();
    display_calMaxValues();

    printf("Ready !\n");

    set_speed_left(0);
    set_speed_right(0);

    robot_wait_seconds(1);
}

void robot_stop() {
    // arrete des moteurs
    set_speed_left(0);
    set_speed_right(0);
}

void robot_follow_line() {
    compute_motor_speed(&motorLeftSpeed, &motorRightSpeed, position);
    set_speed_left(motorLeftSpeed);
    set_speed_right(motorRightSpeed);
}

void robot_move_on_line(Direction direction) {
    switch (direction) {
    case BACK:
        break;
    case FRONT:
        break;            
    case LEFT:
        break;
    case RIGHT:
        set_speed_left(20);
        set_speed_right(-20);
        for (int i = 0; i < 3; i++) sync();
        set_speed_left(0);
        set_speed_right(0);
        break;
    default:
        break;
    }
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

    turn_on(GREEN_LED);
    wait_button();
    turn_off(GREEN_LED);
    printf("Start !\n");

    junctions[BACK] = 1;

    while(1){
        qtr8rc_read_calibrated(&position, junctions);

        if ((junctions[LEFT] | junctions[RIGHT]) == 1) {
            state = JONCTION;
        } else if (stop == 0) {
            state = FOLLOW;
        }

        switch (state) {
        case FOLLOW:
            robot_follow_line();
            break;

        case JONCTION:
            printf("Checking front...\n");
            // verifie si il y a une ligne apres le croisement
            qtr8rc_read_calibrated(&position, junctions);
            printf("BACK:%d;FRONT:%d;LEFT:%d;RIGHT:%d\n", junctions[0], junctions[1], junctions[2], junctions[3]);

            // attendre que les roues soient au niveau du croisement avant de stopper 
            for (int i = 0; i < 3; i++) sync();
            // arrete des moteurs
            set_speed_left(0);
            set_speed_right(0);

            printf("sync 20\n");
            for (int i = 0; i < 20; i++) sync();

            // tmsg->data = junctions;
            // tmsg->size = 4;
            // printf("Sending jucntions...\n");
            // // envoyer les junctions detectees au serveur
            // i2c_send(tmsg);
            // printf("Jucntions sent, waiting direction...\n");
            // // lire la direction a prendre pour continuer
            // i2c_read(rmsg);
            Direction direction =  RIGHT; // decode_msg(rmsg);

            printf("Moving on the line...\n");
            // deplacer le robot pour qu'il match avec la ligne selon la direction
            robot_move_on_line(direction);
            printf("Ready to follow the line !\n");

            set_speed_left(0);
            set_speed_right(0);
            wait_button();

            // clear junctions
            junctions[LEFT] = 0;
            junctions[RIGHT] = 0;

            state = FOLLOW;
            break;
        
        case STOP:
            robot_stop();

        default:
            break;
        }
    }
}
