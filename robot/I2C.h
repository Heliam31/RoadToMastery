#ifndef I2C_h
#define I2C_h

#include <stdint.h>

void i2c_config(int SCL, int SDA);
void i2c_send(uint8_t addr, uint8_t *data, int size);
void i2c_receive(uint8_t addr, uint8_t *data, int size);
void tsl2561_init();
int tsl2561_get_channel(int c);
void arduino_init();
int arduino_get_channel(int c);
void init_tim5();

#endif
