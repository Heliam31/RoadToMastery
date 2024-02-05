#ifndef __MOTOR_DRIVER_H__
#define __MOTOR_DRIVER_H__

void motor_init(void);
void motor_set_speeds(int speedMotorLeft, int speedMotorRight);
void set_speed_right(int speed);
void set_speed_left(int speed);

#endif