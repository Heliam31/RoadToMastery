// Turns an Arduino Nano ESP32 into a Bluetooth® Low Energy peripheral.
// https://tutoduino.fr/
// Copyleft 2023
#include <ArduinoBLE.h>
#include<Wire.h>
BLEService Service("2ce4b983-2934-4e3a-abe5-1034ac6ca3f1"); // Bluetooth® Low Energy LED Service
// Bluetooth® Low Energy LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLECharacteristic writeChar("2ce4b984-2934-4e3a-abe5-1034ac6ca3f1", BLERead | BLEWrite,10);
BLECharacteristic readChar("2ce4b985-2934-4e3a-abe5-1034ac6ca3f1", BLERead | BLEWrite,10);

void setup() {
  Wire.begin(0x33); //Join I2C bus with address 0x33
  Serial.begin(9600);

  // BLE initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
    while (1);
  }
  // set advertised local name and service UUID:
  BLE.setLocalName("ROBOT");
  BLE.setAdvertisedService(Service);
  // add the characteristic to the service
  Service.addCharacteristic(writeChar);
  Service.addCharacteristic(readChar);
  // add service
  BLE.addService(Service);
  // set the initial value for the characeristic:
  writeChar.writeValue("0000");
  readChar.writeValue("0001");
  // start advertising
  BLE.advertise();
  Serial.println("BLE LED Peripheral");

  Wire.onReceive(receiveEvent); // If I2C data received then receiEvent
  Wire.onRequest(requestEvent); //If the other device request data the requestEvent
}

void receiveEvent(int howMany)
{
  Serial.println("received I2C");
  String id="00";//Robot 0, put "01" if other 
  int data=0;
  int remainder =0;
  String binaryNumber="";
  while(0 < Wire.available()) // get all of the data
  {
    data = data + Wire.read();
  }
  Serial.println(data);
  while (data > 0) { // Convert to binary
        remainder = data % 2;
        binaryNumber = String(remainder) + binaryNumber ;
        data = data / 2;
      }
      
  while (binaryNumber.length() < 7) { // put the missing 0 before
    binaryNumber = "0" +binaryNumber;
  }
  Serial.print("Message received : ");
  Serial.println(binaryNumber);
  id = id + binaryNumber; // add the id in front of the data received

  writeChar.writeValue(id.c_str(),id.length()); // write the data on the characteristic

  //Check if the write char has the right value :
  /*
  int length = 4;
  byte value[length + 1];  // one byte more, to save the '\0' character!
  writeChar.readValue(value, length);
  value[length] = '\0';  // make sure to null-terminate!
  Serial.print("Characteristic write : ");
  Serial.println((char *) value);
  */

}


void requestEvent()
{
  int length = 4;
  byte value[length + 1];  
  readChar.readValue(value, length); //read the BLE response 
  value[length] = '\0';  // make sure to null-terminate!
  Serial.print("Characteristic event, written: ");
  Serial.println((char *) value);
  if (value != "0000"){
  Wire.write((char *)value); // send the response with I2C
  }
}

void loop() {
  // wait for a Bluetooth® Low Energy central
  BLEDevice central = BLE.central();
  // check if a central is connected to this peripheral
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
    // while the central is still connected to peripheral:
    while (central.connected()) {
      
      //If needed for test without I2C :
      // -------------
      /*
      String dataToSend = "000011111";
      writeChar.writeValue(dataToSend.c_str(), dataToSend.length());
      
      int length = 4;
      byte value[length + 1];  // one byte more, to save the '\0' character!
      readChar.readValue(value, length);
      value[length] = '\0';  // make sure to null-terminate!
      //byte lastByte = value[length - 2];
      //byte lastFourBits = lastByte & 0b00001111;
      Serial.print("Characteristic event, written: ");
      Serial.println((char *) value);

      delay(1000);*/
      //----------------


      //Already in setup() but just in case:
      //Wire.onReceive(receiveEvent);
      //Wire.onRequest(requestEvent);

      String dataToSend = "0000"; //Reset the value if nothing received by I2C
      writeChar.writeValue(dataToSend.c_str(), dataToSend.length());

      delay(1000);
  }
    // the central has disconnected
    Serial.println("Disconnected from central: ");
  }
  
}
