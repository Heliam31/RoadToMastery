#ifndef __QTR8RC_H__
#define __QTR8RC_H__

#include "utils.h"


void qtr8rc_init(void);
void qtr8rc_calibrate(void);
void qtr8rc_read(int *irValues, const Calibration calibration);

void compute_position(int *position, int *irValues);
void get_avaible_roads(int *roads, int *irValues);

void display_calMaxValues(void);
void display_calMinValues(void);
void display_irValues(int* irValues);

#endif