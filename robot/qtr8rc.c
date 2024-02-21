
#include "qtr8rc.h"


// SENSORS
#define QTR8RC_NB_SENSORS 8

// GPIOD
#define IR1_LED 9
#define IR2_LED 10
#define IR3_LED 0
#define IR4_LED 1
#define IR5_LED 2
#define IR6_LED 3
#define IR7_LED 6
#define IR8_LED 7
#define ON_LED 11
const int QTR8RC_IR_LEDS[8] = {IR1_LED, IR2_LED, IR3_LED, IR4_LED, IR5_LED, IR6_LED, IR7_LED, IR8_LED};

// TIM4
#define QTR8RC_TIME 0.0025
#define QTR8RC_PSC 2
#define QTR8RC_PERIOD (QTR8RC_TIME*APB1_CLK)/QTR8RC_PSC

// SENSOR VALUES
const int TIMEOUT = QTR8RC_PERIOD;
int calMaxValues[QTR8RC_NB_SENSORS] = {0};
int calMinValues[QTR8RC_NB_SENSORS] = {0};

// =============== DEBUG ===============
void display_calMaxValues(void){
    printf("\nMax - ");
    for (int i =0; i < QTR8RC_NB_SENSORS; i++) {
        printf("[%d]:%d, ", i+1, calMaxValues[i]);
    }
    printf("\n");
}

void display_calMinValues(void){
    printf("\nMin - ");
    for (int i =0; i < QTR8RC_NB_SENSORS; i++) {
        printf("[%d]:%d, ", i+1, calMinValues[i]);
    }
    printf("\n");
}

void display_irValues(int* irValues){
    printf("\nsensValues: \n");
    for (int i =0; i < QTR8RC_NB_SENSORS; i++) {
        printf("[%d]:%d, ", i+1, irValues[i]);
    }
    printf("\n");
}

// =============== UTILS ===============
void output_mode(const int *leds) {
    for (int i = 0; i < QTR8RC_NB_SENSORS; i++) {
        GPIOD_MODER = REP_BITS(GPIOD_MODER, leds[i]*2, 2, GPIO_MODER_OUT);
        GPIOD_OTYPER &= ~(1 << leds[i]);
    }
}

void input_mode(const int *leds) {
    for (int i = 0; i < QTR8RC_NB_SENSORS; i++) {
        GPIOD_MODER = REP_BITS(GPIOD_MODER, leds[i]*2, 2, GPIO_MODER_IN);
        GPIOD_OTYPER &= ~(1 << leds[i]);
    }
}

void drive_high(const int *leds) { // make sensor line an output and drive high
    for (int i = 0; i < QTR8RC_NB_SENSORS; i++) {
        GPIOD_ODR |= 1 << leds[i];
    }
}

// =============== INIT ===============
void init_gpio() {
    // IR GPIO init
    output_mode(QTR8RC_IR_LEDS);

    // LEDON GPIO init
    GPIOD_MODER = REP_BITS(GPIOD_MODER, ON_LED * 2, 2, GPIO_MODER_OUT);
    GPIOD_OTYPER &= ~(1 << ON_LED);
}

void init_tim4(){
	TIM4_CR1 = 0;
	TIM4_PSC = QTR8RC_PSC-1;
	TIM4_ARR = QTR8RC_PERIOD;
	TIM4_EGR = TIM_UG;
	TIM4_SR = 0;
	TIM4_CR1 = TIM_CEN;
}

void qtr8rc_init(void) {
    init_gpio();
	init_tim4();
    set_tab(calMinValues, QTR8RC_NB_SENSORS, TIMEOUT);    
    // Init minirValues
    // for (int i = 0; i < QTR8RC_NB_SENSORS; i++) {
    //     calMinValues[i] = TIMEOUT;
    //     delay_ms(100); // ????
    // }
}


// =============== FUNCTIONS ===============
void normalize(int *irValues) {
    uint16_t calmin = 0, calmax = 0;
    for (unsigned int i = 0; i < QTR8RC_NB_SENSORS; i++) {

        calmin = calMinValues[i];
        calmax = calMaxValues[i];

        // Appliquer la formule de calibration
        int denominator = calmax - calmin;
        int value = 0;

        if (denominator != 0) {
            value = ((irValues[i] - calmin) * 1000) / denominator;
        }

        if (value < 0) { value = 0; } 
        else if (value > 1000) { value = 1000; }

        irValues[i] = value;
    }
}

/*
**  @brief: Start the timer 4
**  @param: None
**  @return: None
*/
void start_tim4(void){
    TIM4_CNT = 0; // Reset the counter
    TIM4_CR1 = TIM_CEN; // Start the timer
    TIM4_SR = 0; // Clear the update event flag
}

/*
**  @brief: Stop the timer 4
**  @param: None
**  @return: None
*/
void stop_tim4(void){
    TIM4_CR1 &= ~TIM_CEN; // Stop the timer
}

/*
**  @brief: Measure the time for the output to fall to low
**  @param: irValues[in/out]: the array of the ir values
**  @param: calibration_mode[in]: the mode of the calibration
**  @return: None
*/
void measure(int *irValues, const Calibration calibration) {
    uint32_t startTime = TIM4_CNT; // Get the current time
    uint32_t elapsedTime = 0; 
    uint32_t currentTime = 0;

    while (((currentTime=TIM4_CNT) + elapsedTime) < TIMEOUT) {
        elapsedTime = currentTime - startTime; // Compute the elapsed time

        // Compute each led's value
        for (unsigned int i = 0; i < QTR8RC_NB_SENSORS; i++) {
            if (((GPIOD_IDR & (1 << QTR8RC_IR_LEDS[i])) == 0) && (elapsedTime < irValues[i])){
                if (calibration == ON) {
                    if (elapsedTime < calMinValues[i])
                        calMinValues[i] = elapsedTime;
                    if (elapsedTime > calMaxValues[i])
                        calMaxValues[i] = elapsedTime;
                } else {
                    irValues[i] = elapsedTime;
                }
            }
        }
    }
}

/*
**  @brief: Read the ir values
**  @param: irValues[in/out]: the array of the ir values
**  @return: None
*/
void qtr8rc_read(int *irValues, const Calibration calibration) {
    set_tab(irValues, QTR8RC_NB_SENSORS, TIMEOUT);
    // Turn on the IR LEDs
    led_turn_on(ON_LED);
    // Set the I/O line to an output
    output_mode(QTR8RC_IR_LEDS);
    // Drive the I/O line high
    drive_high(QTR8RC_IR_LEDS);
    // Allow at least 10 μs for the sensor output to rise
    delay_us(10);
    // Make the I/O line an input (high impedance)
    input_mode(QTR8RC_IR_LEDS);
    // Start the timer for the measure
    start_tim4();
    // Measure the time for the output to fall to low
    measure(irValues, calibration);
    // Disable the timer
    stop_tim4();

    // display_irValues(irValues);

    // Turn off IR LEDs
    led_turn_off(ON_LED);
    // Normalize the ir values
    normalize(irValues);
}

void qtr8rc_calibrate(void) {
    int irValues[QTR8RC_NB_SENSORS] = {0};
    qtr8rc_read(irValues, ON);
}

void compute_position(int *position, int *irValues) { 
    int onLine = 0;
    uint32_t avg = 0; // this is for the weighted total
    uint16_t sum = 0; // this is for the denominator, which is <= 64000

    for (uint8_t i = 0; i < QTR8RC_NB_SENSORS; i++) {
        uint16_t value = irValues[i];

        // keep track of whether we see the line at all
        if (value > 350) {
            onLine = 1;
        }

        // only average in values that are above a noise threshold
        if (value > 50) {
            avg += (uint32_t)value * (i * 1000);
            sum += value;
        }
    }

    // printf("online==%d\n", onLine);

    if (!onLine) {
        // If it last read to the left of center, return 0.
        if (*position < (QTR8RC_NB_SENSORS - 1) * 1000 / 2) {
            *position = 0;
            return;
        }
        // If it last read to the right of center, return the max.
        else {
            *position = (QTR8RC_NB_SENSORS - 1) * 1000;
            return;
        }
    }

    *position = avg / sum;
}

/*
** @brief : Get the avaible roads
** @param[in-out] int *roads : array of the avaible roads 
** @param[in] int *irValues : values of the sensors
** @return : None
*/
void get_avaible_roads(int *roads, int *irValues) {
    int sensors[QTR8RC_NB_SENSORS] = {0};
    for (int i = 0; i < QTR8RC_NB_SENSORS; i++) {
        if (irValues[i] == 1000) {
            sensors[i] = 1;
        } else {
            sensors[i] = 0;
        }
    }
    roads[FRONT] = sensors[3] | sensors[4];
    roads[RIGHT] = sensors[0] & sensors[1] & sensors[2];
    roads[LEFT] = sensors[5] & sensors[6] & sensors[7];
}
