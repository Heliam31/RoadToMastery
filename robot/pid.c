
#include <tinyprintf.h>

#include "pid.h"
#include "utils.h"


#define Kp 0.04 //set up the constants value
#define Ki 0
#define Kd 0
#define REFERENCE 3650

#define LEFT_MOTOR_OFFSET 0
#define RIGHT_MOTOR_OFFSET 0

int errors[10] = {0,0,0,0,0,0,0,0,0,0};
int errorPrec = 0;
int errorSum = 0;
const int maxSpeedRight = 20;
const int maxSpeedLeft = 20;
const int baseSpeedRight = 0;
const int baseSpeedLeft = 0;

int errors_sum(int index, int abs) {
    int sum = 0;
    for (int i = 0; i < index; i++) {
    if ((abs == 1) && (errors[i] < 0)) 
        sum += -errors[i];
    else
        sum += errors[i];
    }
    return sum;
}

void save_error(int error) {
    errors[9] = errors[8];
    errors[8] = errors[7];
    errors[7] = errors[6];
    errors[6] = errors[5];
    errors[5] = errors[4];
    errors[4] = errors[3];
    errors[3] = errors[2];
    errors[2] = errors[1];
    errors[1] = errors[0];
    errors[0] = error;
}

int _compute_pid(const int position, const int error) {
    errorSum += error;

    int motorSpeed = error*Kp + errorSum*Ki + (error - errorPrec)*Kd;

    errorPrec = error;
    
    return motorSpeed;
}

void compute_motor_speed(int *motorLeftSpeed, int *motorRightSpeed, const int position) {
    int error = REFERENCE - position;
    // printf("error: %d\n",error);
    int motorSpeed = _compute_pid(position, error);

    *motorLeftSpeed = (baseSpeedLeft + motorSpeed);
    *motorRightSpeed = (baseSpeedRight - motorSpeed);

    *motorLeftSpeed = max(-1, min(*motorLeftSpeed, maxSpeedLeft));
    *motorRightSpeed = max(-1, min(*motorRightSpeed, maxSpeedRight));

    *motorLeftSpeed = *motorLeftSpeed + maxSpeedLeft - (abs(error)/1000)*maxSpeedLeft + LEFT_MOTOR_OFFSET;
    *motorRightSpeed = *motorRightSpeed + maxSpeedRight - (abs(error)/1000)*maxSpeedRight + RIGHT_MOTOR_OFFSET;

    // printf("%d: %d,%d \n",position, *motorLeftSpeed, *motorRightSpeed);
}
