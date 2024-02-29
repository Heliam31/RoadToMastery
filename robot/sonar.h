#ifndef __SONAR_H__
#define __SONAR_H__

void sonar_init(void);
void sonar_read(unsigned int* distance);
void init_tim2_sonar();

#endif