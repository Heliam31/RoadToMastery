#include <Arduino.h>
#include <Wire.h>

#define I2C_SLAVE_ADDR 0x09

void receiveEvent(int howMany);

void setup()
{
    Serial.begin(9600);

    bool success = Wire.begin(I2C_SLAVE_ADDR);
    if (!success) {
        Serial.println("I2C slave init failed");
        while(1) delay(100);
    }

    Wire.onReceive(receiveEvent);
}

void loop()
{
    // the slave response time is directly related to how often
    // this update() method is called, so avoid using long delays
    // inside loop(), and be careful with time-consuming tasks

    // let I2C and other ESP32 peripherals interrupts work
    delay(1);
}

// function that executes whenever a complete and valid packet
// is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
    while (1 < Wire.available()) // loop through all but the last byte
    {
        char c = Wire.read();  // receive byte as a character
        Serial.print(c);            // print the character
    }

    int x = Wire.read();   // receive byte as an integer
    Serial.println(x);          // print the integer
}