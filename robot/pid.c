
#include <tinyprintf.h>

#include "pid.h"


#define Kp 0.4 //set up the constants value
#define Ki 0
#define Kd 0
#define REFERENCE 4100

int errors[10] = {0,0,0,0,0,0,0,0,0,0};
int errorPrec = 0;
int errorSum = 0;
const int maxSpeedRight = 10;
const int maxSpeedLeft = 10;
const int baseSpeedRight = 9;
const int baseSpeedLeft = 9;

int abs(int x) {
    return x < 0 ? -x : x;
}

int min(int x, int y) {
    return x < y ? x : y;
}

int max(int x, int y) {
    return x < y ? y : x;
}

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

int calculate_pid(const int position, const int error) {
    errorSum += error;

    int motorSpeed = error*Kp + errorSum*Ki + (error - errorPrec)*Kd;

    errorPrec = error;
    
    return motorSpeed;
}

void calculate_motor_speed(int *motorLeftSpeed, int *motorRightSpeed, const int position) {
    int error = REFERENCE - position;
    int motorSpeed = calculate_pid(position, error);

    printf("motorSpeed:%d\n", motorSpeed);

    *motorLeftSpeed = (baseSpeedLeft + motorSpeed);
    *motorRightSpeed = (baseSpeedRight - motorSpeed);

    printf("l = %d\n", *motorLeftSpeed);
    printf("r = %d\n", *motorRightSpeed);

    *motorLeftSpeed = max(-1, min(*motorLeftSpeed, maxSpeedLeft));
    *motorRightSpeed = max(-1, min(*motorRightSpeed, maxSpeedRight));

    *motorLeftSpeed = *motorLeftSpeed + maxSpeedLeft - (abs(error)/1000)*maxSpeedLeft;
    *motorRightSpeed = *motorRightSpeed + maxSpeedRight - (abs(error)/1000)*maxSpeedRight;
}

