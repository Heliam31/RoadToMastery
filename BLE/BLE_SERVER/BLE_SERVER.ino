// Turns an Arduino Nano ESP32 into a Bluetooth® Low Energy peripheral.
// This BLE peripheral is providing a service that allows a BLE central 
// to switch on and off the internal LED of the Arduino Nano ESP32.
// https://tutoduino.fr/
// Copyleft 2023
#include <ArduinoBLE.h>
BLEService Service("19b10000-e8f2-537e-4f6c-d104768a1214"); // Bluetooth® Low Energy LED Service
// Bluetooth® Low Energy LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLECharacteristic switchCharacteristic("19b10000-e8f2-537e-4f6c-d104768a1214", BLERead | BLEWrite,5);
//const int ledPin = LED_BUILTIN; // internal LED pin
void setup() {
  Serial.begin(9600);
  // set LED pin to output mode
  //pinMode(ledPin, OUTPUT);
  // BLE initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
    while (1);
  }
  // set advertised local name and service UUID:
  BLE.setLocalName("LED");
  BLE.setAdvertisedService(Service);
  // add the characteristic to the service
  Service.addCharacteristic(switchCharacteristic);
  // add service
  BLE.addService(Service);
  // set the initial value for the characeristic:
  switchCharacteristic.writeValue("couco");
  // start advertising
  BLE.advertise();
  Serial.println("BLE LED Peripheral");
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
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:
      if (switchCharacteristic.written()) {
        byte dataReceived= 0;
        switchCharacteristic.readValue(dataReceived);
        Serial.println(dataReceived);
        switchCharacteristic.writeValue("gg");
      }
    }
    // the central has disconnected
    Serial.println("Disconnected from central: ");
  }
}
