
int position;

float Kp = 0.002; //set up the constants value
float Ki = 0.001;
float Kd = 15;
float Kr = 0;
int P, I, D, R;
int lastError = 0;
int errors[10] = {0,0,0,0,0,0,0,0,0,0};
int errorSum = 0;
int lastEnd = 0;	// 0 -> Left, 1 -> Right 
int lastIdle = 0;

const uint8_t maxSpeedRight = 100;
const uint8_t maxSpeedLeft = 100;
const uint8_t baseSpeedRight = 92;
const uint8_t baseSpeedLeft = 92;
const int ARR = 10;

int actives = 0;

int errors_sum(int index, int abs) {
    int sum = 0;
    for (int i = 0; i < index; i++) {
    if (abs == 1 & errors[i] < 0) 
        sum += -errors[i];
    else
        sum += errors[i];
    }
    return sum;
}


void compute_speed(int *sensor_values) {
    int motorLeft = 0;
    int motor_Right = 0;

    if (sensor_values[3] && sensor_values[4] == 1) {
        motorLeft = 50;
        motor_Right = 50;
    }

    action_motor(motorLeft, motor_Right);
}


void calc_PID(int position) {
    int error = 4500 - position; // 
    
    // save error
    for (int i = 9; i > 0; i--) {
        errors[i] = errors[i-1];
    }
    errors[0] = error;

    P = error;
    I = errors_sum(5, 1);
    D = error - lastError;
    R = errors_sum(5, 1);
    lastError = error;

    int motorSpeed = P*Kp + I*Ki + D*Kd;

    int motorSpeedRight = baseSpeedRight - motorSpeed - R*Kr;
    int motorSpeedLeft = baseSpeedLeft + motorSpeed - R*Kr;

    if (motorSpeedLeft > maxSpeedLeft) {
        motorSpeedLeft = maxSpeedLeft;
    }
    if (motorSpeedRight > maxSpeedRight) {
        motorSpeedRight = maxSpeedRight;
    }

    control_motor(motorLSpeedLeft, motorLSpeedRight);
}
