#include "pid.h"
#include "utils.h"


/*==Values that work==*/
/*
batLev: 75&100 (old Ki),        50&50           75&75           100&75
  norm: no (old Ki),            no              no              no
 speed: 27,18,-5/-13 (old Ki)   27,18,-13       23,18,-1        23,18,-1
    Kp: 0.15 (old Ki)           0.039           1.82            0.069
    Ki: 0 (old Ki)              0.001           0.045           0.002
    Kd: 2 (old Ki)              2.69            2.69            2
*/

#define Kp 0.069 // 0.086
#define Ki 0.002 // 0.007 / 0.6
#define Kd 2 // 2.69
#define REFERENCE 3500

#define LEFT_MOTOR_OFFSET 0
#define RIGHT_MOTOR_OFFSET 0

int errors[10] = {0,0,0,0,0,0,0,0,0,0};
int lastError = 0;
int errorSum = 0;
const int maxSpeedRight = 23;
const int maxSpeedLeft = 23;
const int baseSpeedRight = 18;
const int baseSpeedLeft = 18;

int _errors_sum(const int index) {
    int sum = 0;
    for (int i = 0; i < index; i++) {
        sum += errors[i];
    }
    return sum;
}

void _save_errors(int error) {
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

int _compute_pid(const int error) {
    errorSum += error;
    int motorSpeed = error*Kp + _errors_sum(5)*Ki + (error - lastError)*Kd;

    lastError = error;
    
    return motorSpeed;
}

void pid_compute_speeds(int *motorLeftSpeed, int *motorRightSpeed, const int *position) {
    int error = REFERENCE - *position;

    _save_errors(error);

    int motorSpeed = _compute_pid(error);

    *motorLeftSpeed = (baseSpeedLeft + motorSpeed);
    *motorRightSpeed = (baseSpeedRight - motorSpeed);

    *motorLeftSpeed = max(-1, min(*motorLeftSpeed, maxSpeedLeft));
    *motorRightSpeed = max(-1, min(*motorRightSpeed, maxSpeedRight));
    *motorLeftSpeed = max(-1, min(*motorLeftSpeed, maxSpeedLeft));
    *motorRightSpeed = max(-1, min(*motorRightSpeed, maxSpeedRight));

    // printf("%d: %d,%d \n", *position, *motorLeftSpeed, *motorRightSpeed);
}
