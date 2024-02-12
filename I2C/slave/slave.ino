/*
* Good links:
* https://deepbluembedded.com/arduino-i2c-tutorial-examples/
* https://www.arduino.cc/reference/en/language/functions/communication/wire/
*/

#include <Wire.h>

int cnt = 0;

int slave_addr = 0x33;
int last_reg = 0;
int msg2send;

#define CONTROL   0
#define TIMING    1
#define DATA0LOW  0xC
#define DATA0HIGH 0xD
#define DATA1LOW  0xE
#define DATA1HIGH 0xF

void i2c_receive(int size) {
  Serial.print("Receive ");
  Serial.println(size);
  int cmd = Wire.read();
    size--;
    while(size > 0) {
      int data = Wire.read();
      if(data < 0) {
        Serial.println("Error!");
        break;
      }

      Serial.print("Data: ");
      Serial.println(data);
      msg2send = data;
      size--;

      BluetoothSend();
    }
  
}

uint8_t get_reg() {
  int i = last_reg;
  last_reg = (last_reg + 1) & 0xF;
  if(last_reg < DATA0HIGH)
    return regs[i];
  else if((regs[CONTROL] & 0b11) == 0b11)
    return i;
  else
    return 0;
}

void i2c_request() {
  Serial.print("Request ");
  
  Serial.println(last_reg);
  int x = get_reg();
  Serial.print("Answer ");
  Serial.println(x);
  Wire.write(x);
  x = get_reg();
  Serial.print("Answer ");
  Serial.println(x);
  Wire.write(x);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Starting");
  Wire.begin(slave_addr);
  Wire.onReceive(i2c_receive);
  Wire.onRequest(i2c_request);
}

void loop() {
  delay(10000);
  Serial.print("alive... ");
  Serial.println(cnt++);
}
