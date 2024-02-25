#ifndef __MOTOR_DRIVER_H__
#define __MOTOR_DRIVER_H__


typedef enum motor_e {M_LEFT, M_RIGHT} Motor;

void motor_init(void);
void motor_set_speeds(int speedMotorLeft, int speedMotorRight);
void motor_disable(Motor motor);

void set_speed_right(int speed);
void set_speed_left(int speed);

#endif