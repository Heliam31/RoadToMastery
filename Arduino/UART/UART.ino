void setup() {

  Serial.begin(9600);    // Initialize the Serial monitor for debugging
  pinMode(7, INPUT);
  // Serial1.begin(9600);   // Initialize Serial1 for sending data

}


void loop() {
  Serial.println("oiu");
  delay(500);
  if (Serial.available() > 0) {
      // read the incoming byte:
      int incomingByte = Serial.read();

      // say what you got:
      Serial.print("I received: ");
      Serial.println(incomingByte);
    }
}