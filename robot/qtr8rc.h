#ifndef __QTR8RC_H__
#define __QTR8RC_H__

void qtr8rc_init(void);
void qtr8rc_read_calibrated(int* position);
void display_calMaxValues();
void display_calMinValues();
#endif