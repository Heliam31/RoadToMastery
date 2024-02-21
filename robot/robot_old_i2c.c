// #include "pid.h"
// #include "qtr8rc.h"
// #include "motor_driver.h"
// #include "utils.h"
// #include "i2c.h"


// // GLOBALS
// #define ESP32_ADDR 0x33

// // INIT
// void enable_clk(void) {
//     RCC_AHB1ENR |= RCC_GPIOAEN;
// 	RCC_AHB1ENR |= RCC_GPIODEN;
// 	RCC_AHB1ENR |= RCC_GPIOCEN;

// 	RCC_APB1ENR |= RCC_TIM2EN;
// 	RCC_APB1ENR |= RCC_TIM3EN;
//     RCC_APB1ENR |= RCC_TIM4EN;
// 	RCC_APB1ENR |= RCC_TIM6EN;

//     RCC_APB1ENR |= RCC_I2C1EN | RCC_TIM5EN;
// 	RCC_AHB1ENR |= RCC_GPIOBEN;
//     RCC_APB2ENR |= RCC_ADC1EN;
// }

// void init(void) {
//     qtr8rc_init();
//     motor_init();
    
//     init_tim6();
//     led_init(GREEN_LED);
//     led_init(ORANGE_LED);
//     led_init(RED_LED);
//     led_init(BLUE_LED);
//     button_init(BUTTON);
//     i2c_config(8, 9);
// }

// void calibration(void) {
//     led_turn_on(GREEN_LED);
//     button_wait(BUTTON);
//     led_turn_off(GREEN_LED);

//     // display_calMinValues();
//     // display_calMaxValues();

//     printf("Calibrating...\n");

//     motor_set_speeds(-25, 25);

//     for (int i = 0; i < 100; i++) {
//         qtr8rc_calibrate();
//         // printf("DELAY MS\n");
//         delay_ms(10);
//         // printf("MS DOWN\n");
//     }

//     // display_calMinValues();
//     // display_calMaxValues();

//     printf("Ready !\n");

//     motor_set_speeds(0, 0);
//     // delay_ms(1000); // 1s
// }

// void move_on_line(Direction *direction) {
//     switch (*direction) {
//     case BACK:
//         led_turn_on(ORANGE_LED);
//         set_speed_left(-16);
//         set_speed_right(16);
//         break;
//     case FRONT:
//         // led_turn_on(BLUE_LED);
//         // set_speed_left(16);
//         // set_speed_right(16);
//         break;
//     case LEFT:
//         led_turn_on(RED_LED);
//         set_speed_left(-16);
//         set_speed_right(16);
//         break;
//     case RIGHT:
//         led_turn_on(GREEN_LED);
//         set_speed_left(16);
//         set_speed_right(-16);
//         break;
//     default:
//         break;
//     }
// }

// int on_road(Direction *direction, int *irValues) {
//     switch (*direction) {
//     case BACK:
//         if (irValues[0] > 1000 && irValues[1] > 1000) {
//             return 1;
//         }
//         break;
//     case FRONT:
//         if (irValues[6] > 1000 && irValues[7] > 1000) {
//             return 1;
//         }
//         break;
//     case LEFT:
//         if (irValues[0] > 1000 && irValues[7] > 1000) {
//             return 1;
//         }
//         break;
//     case RIGHT:
//         if (irValues[6] > 1000 && irValues[1] > 1000) {
//             return 1;
//         }
//         break;
//     default:
//         break;
//     }
//     return 0;
// }

// int on_junction(int *roads){
//     return roads[LEFT] | roads[RIGHT]; 
// }

// State choose_direction(int reg){
//     if(reg == 0b0001){
//         return RIGHT;
//     }else if (reg == 0b0010)
//     {
//         return FRONT;
//     }else if (reg == 0b0100)
//     {
//         return LEFT;
//     }else if (reg == 0b1000)
//     {
//         return BACK;
//     }
// }
 

// int main (void) {
//     enable_clk();
//     init();
//     calibration();

//     int irValues [8] = {0};
//     int sonarValue = 0;

//     int roads [4] = {0};
//     int position = 0;

//     int leftSpeed = 0;
//     int rightSpeed = 0;

//     // msg_t *tmsg = {ESP32_ADDR, {0}, 8};
//     // msg_t *rmsg = {ESP32_ADDR, {0}, 8};

//     State state = FOLLOW;
//     Direction direction = BACK;
//     roads[BACK] = 1;

//     led_turn_on(GREEN_LED);
//     button_wait(BUTTON);
//     led_turn_off(GREEN_LED);

//     arduino_init();
//     printf("Loop\n");
//     while(1) {
//         // start_timer(); // TODO
        
//         qtr8rc_read(irValues, OFF);
//         // sonar_read(&sonarValue); // TODO ???
        
//         switch(state) {
//             case FOLLOW:
//                 set_tab(roads, 4, 0);
//                 get_avaible_roads(roads, irValues);
//                 if (on_junction(roads) == 0) {
//                     compute_position(&position, irValues);
//                     pid_compute_speeds(&leftSpeed, &rightSpeed, position);
//                     // printf("%d %d %d\n", position, leftSpeed, rightSpeed);
//                 } else {
//                     state = STOP;
//                     leftSpeed = 15;
//                     rightSpeed = 15;
//                     for (int i = 0; i < 4; i++) {
//                         // tmsg->data[i] = roads[i];
//                     }
//                 }
//                 break;
            
//             case STOP:
//                 delay_ms(100);
//                 leftSpeed = 0;
//                 rightSpeed = 0;
//                 ///////////////I2C AJOUT/////////////////
//                 uint8_t reg_send[2] = { 1, 0b0101}; //reg à send
//                 uint8_t reg_receive[1] = {0}; //reg pour demander des données
//                 i2c_send(ESP32_ADDR, reg_send , 2); //on send des données
//                 i2c_receive(ESP32_ADDR, reg_receive, 1); //on demande des données
                
//                 /////////////////////////////////////////
//                 direction = choose_direction(reg_receive);
//                 if (direction == FRONT) {
//                     state = FOLLOW;
//                 } else {
//                     state = TURN;
//                 }
//                 break;
            
//             case TURN:
//                 move_on_line(&direction);
//                 if (direction == LEFT) {
//                     state = S_LEFT;
//                 } else if (direction == RIGHT) {
//                     state = S_RIGHT;
//                 } else {
//                     state = S_BACK;
//                 }
//                 break;

//             case S_RIGHT:
//                 if (irValues[0] == 1000) {
//                     state = CHECK2;
//                 }
//                 break;
            
//             case S_LEFT:
//                 if (irValues[7] == 1000) {
//                     state = CHECK7;
//                 }
//                 break;
            
//             case S_BACK:
//                 // if (direction != LEFT) {
//                 //     state = S_LEFT;
//                 // } else
//                 if (irValues[7] == 1000) {
//                     state = CHECK8_WHITE;
//                 }
//                 break;
            
//             case CHECK2:
//                 if (irValues[1] == 1000) {
//                     state = CHECK3;
//                 }
//                 break;
            
//             case CHECK3:
//                 if (irValues[2] == 1000) {
//                     state = CHECK4;
//                 }
//                 break;
            
//             case CHECK4:
//                 if (irValues[3] == 1000) {
//                     state = FOLLOW;
//                 }
//                 break;
            
//             case CHECK5:
//                 if (irValues[4] == 1000) {
//                     state = FOLLOW;
//                 }
//                 break;
            
//             case CHECK6:
//                 if (irValues[5] == 1000) {
//                     state = CHECK5;
//                 }
//                 break;
            
//             case CHECK7:
//                 if (irValues[6] == 1000) {
//                     state = CHECK6;
//                 }
//                 break;
            
//             case CHECK8_WHITE:
//                 if (irValues[7] < 1000) {
//                     state = S_LEFT;
//                 }
//                 break;
//         }
//         display_irValues(irValues);
//         printf("%d:", position);
//         printf("%d;%d\n", leftSpeed, rightSpeed);
//         display_direction(direction);
//         display_state(state);

//         motor_set_speeds(leftSpeed, rightSpeed);
//         // sync();
//     }
// }
