#ifndef __QTR8RC_H__
#define __QTR8RC_H__

void qtr8rc_read_line(int *sensorValues, int calibration_mode);
void qtr8rc_normalize(int *sensorValues);

void qtr8rc_init(void);
void qtr8rc_calibrate(void);
void qtr8rc_read_calibrated(int *position, int *junctions);

void display_calMaxValues(void);
void display_calMinValues(void);
void display_sensorValues(int* sensorValues);

#endif