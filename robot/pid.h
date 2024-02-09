
#ifndef __PID_H__
#define __PID_H__


int abs(int x);
int min(int x, int y);
int max(int x, int y);

void compute_motor_speed(int *motorLeftSpeed, int *motorRightSpeed, const int position);

#endif