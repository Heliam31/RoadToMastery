// Turns an Arduino Nano ESP32 into a Bluetooth® Low Energy peripheral.
// This BLE peripheral is providing a service that allows a BLE central 
// to switch on and off the internal LED of the Arduino Nano ESP32.
// https://tutoduino.fr/
// Copyleft 2023
#include <ArduinoBLE.h>
#include<Wire.h>
BLEService Service("2ce4b983-2934-4e3a-abe5-1034ac6ca3f1"); // Bluetooth® Low Energy LED Service
// Bluetooth® Low Energy LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLECharacteristic writeChar("2ce4b984-2934-4e3a-abe5-1034ac6ca3f1", BLERead | BLEWrite,10);
BLECharacteristic readChar("2ce4b985-2934-4e3a-abe5-1034ac6ca3f1", BLERead | BLEWrite,10);


void setup() {
  Wire.begin(0x33);
  Serial.begin(9600);

  // BLE initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
    while (1);
  }
  // set advertised local name and service UUID:
  BLE.setLocalName("LED");
  BLE.setAdvertisedService(Service);
  // add the characteristic to the service
  Service.addCharacteristic(writeChar);
  Service.addCharacteristic(readChar);
  // add service
  BLE.addService(Service);
  // set the initial value for the characeristic:
  writeChar.writeValue("110");
  readChar.writeValue("100");
  // start advertising
  BLE.advertise();
  Serial.println("BLE LED Peripheral");
}

void receiveEvent(int howMany)
{
  String data="00";//Robot 0
  int remainder =0;
  while(0 < Wire.available()) // loop through all but the last
  {
    data = data + Wire.read();
  }
  Serial.println(data); 
  // Set the characteristic's value to be the array of bytes that is actually a string.
  writeChar.writeValue(data.c_str(),data.length());
  
}


void requestEvent()
{
  int length = 4;
  byte value[length + 1];  // one byte more, to save the '\0' character!
  readChar.readValue(value, length);
  value[length] = '\0';  // make sure to null-terminate!
  Serial.print("Characteristic event, written: ");
  Serial.println((char *) value);

  Wire.write((char *)value);
}

void loop() {
  // wait for a Bluetooth® Low Energy central
  //Serial.println("connecting");
  BLEDevice central = BLE.central();
  //Serial.println("connecting");
  // check if a central is connected to this peripheral
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
    // while the central is still connected to peripheral:
    while (central.connected()) {
      //sans I2C

      String dataToSend = "00010";
      writeChar.writeValue(dataToSend.c_str(), dataToSend.length());
      
      int length = 4;
      byte value[length + 1];  // one byte more, to save the '\0' character!
      readChar.readValue(value, length);
      value[length] = '\0';  // make sure to null-terminate!
      Serial.print("Characteristic event, written: ");
      Serial.println((char *) value);

      delay(1000);
      
      //AVEC I2C
      //Wire.onReceive(receiveEvent);
      //Wire.onRequest(requestEvent);
    
  }
    // the central has disconnected
    Serial.println("Disconnected from central: ");
  }
  
}
