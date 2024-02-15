#include "pid.h"
#include "utils.h"


#define Kp 0.04 //set up the constants value
#define Ki 0
#define Kd 0
#define REFERENCE 3650

#define LEFT_MOTOR_OFFSET 0
#define RIGHT_MOTOR_OFFSET 0

int errorPrec = 0;
int errorSum = 0;
const int maxSpeedRight = 20;
const int maxSpeedLeft = 20;
const int baseSpeedRight = 0;
const int baseSpeedLeft = 0;

int compute_pid(const int *error) {
    int motorSpeed = 0;

    errorSum += (*error);
    motorSpeed = (*error)*Kp + errorSum*Ki + ((*error) - errorPrec)*Kd;
    errorPrec = (*error);
    
    return motorSpeed;
}

void pid_compute_speeds(int *motorLeftSpeed, int *motorRightSpeed, const int *position) {
    int error = REFERENCE - *position;
    int motorSpeed = compute_pid(&error);

    *motorLeftSpeed = (baseSpeedLeft + motorSpeed);
    *motorRightSpeed = (baseSpeedRight - motorSpeed);

    *motorLeftSpeed = max(-1, min(*motorLeftSpeed, maxSpeedLeft));
    *motorRightSpeed = max(-1, min(*motorRightSpeed, maxSpeedRight));

    *motorLeftSpeed = *motorLeftSpeed + maxSpeedLeft - (abs(error)/1000)*maxSpeedLeft + LEFT_MOTOR_OFFSET;
    *motorRightSpeed = *motorRightSpeed + maxSpeedRight - (abs(error)/1000)*maxSpeedRight + RIGHT_MOTOR_OFFSET;
}
