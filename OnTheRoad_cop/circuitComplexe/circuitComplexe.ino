#include <QTRSensors.h>

QTRSensors qtr;

int E1 = 6;
int M1 = 7;
int E2 = 5;                         
int M2 = 4;  

int cptError =  0;
const double kp = 0.085;
const double ki = 0;
const double kd = 0;
  
int error;
int lasterror = 0;
int i = 0;
int motorspeed;
int rightmotorspeed;
int leftmotorspeed;

int constDecal = 28;
int maxmotorspeed = 200;

const uint8_t SensorCount = 3;
uint16_t sensorValues[SensorCount];

void setup() {

  //setup motors
  pinMode(M1, OUTPUT);   
  pinMode(M2, OUTPUT); 
  
  Serial.begin(9600);
  // put your setup code here, to run once:
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){8,9,10}, SensorCount);

  delay(500);
  for (uint16_t i = 0; i < 400; i++)
  {
    qtr.calibrate();
    if(i==0 || i==60 || i==120 || i==180){
      digitalWrite(M1,HIGH);   
      digitalWrite(M2,HIGH);
      analogWrite(E1, 100);   //PWM Speed Control
      analogWrite(E2, 100+ constDecal);
    } else if(i==10 || i==70 || i==130 || i==190) {
      digitalWrite(M1,LOW);   
      digitalWrite(M2,LOW);
      analogWrite(E1, 100);   //PWM Speed Control
      analogWrite(E2, 100+ constDecal);
    } else if(i==20 || i==80 || i==140 || i==200){
      analogWrite(E1,0);
      analogWrite(E2,0);
    }
  }

  
  
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(qtr.calibrationOn.minimum[i]);
  }
  // print the calibration maximum values measured when emitters were on
  delay(1000);

}

void loop() {

  unsigned long t1 = millis();
  
  uint16_t position = qtr.readLineBlack(sensorValues);
  
  
    
  // print the sensor values as numbers from 0 to 1000, where 0 means maximum
  // reflectance and 1000 means minimum reflectance, followed by the line
  // position
  /*
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(sensorValues[i]);
    Serial.print('\t');
  }
  Serial.println(position);
  */

  error = 1000 - position;
  i = i + error;

  //motorspeed = (0.07 * error + 0.6 * (error-lasterror) + 0.0008 * i)*1;
  //motorspeed = 0.2 * error + 3 * (error-lasterror) + 0 * i;
  //motorspeed = 0.17 * error + 3 * (error-lasterror) + 0.0001 * i;
  // valeurs cool 75 bp 200 mp motorspeed = 0.17 * error +  3 * (error-lasterror) + 0*i;
  //motorspeed = 0.154321 * error +  2.9 * (error-lasterror);// + 0.0008*i;
  //motorspeed = kp * error + ki * i + kp * (error-lasterror);// +  3 * (error-lasterror) ;
  motorspeed = 0.149321 * error +  2.9 * (error-lasterror);// + 0.0008*i;
  
  lasterror = error;
  rightmotorspeed = 60 + motorspeed;
  leftmotorspeed = 60  - motorspeed;
  
  int miniG;
  int miniD;
  
  miniG = max(-30, min(leftmotorspeed,maxmotorspeed));
  miniD = max(-30, min(rightmotorspeed,maxmotorspeed));// + constDecal;  
  
  if(miniG < 0){
    digitalWrite(M1,LOW);
    miniG = abs(miniG);   
  } else {
    digitalWrite(M1,HIGH);     
  }
  if(miniD < 0){
    digitalWrite(M2,LOW);
    miniD = abs(miniD);   
  } else {
    digitalWrite(M2,HIGH);     
  }
  
 
  analogWrite(E1, miniG + 50 - (abs(error)/1000)*100);
  analogWrite(E2, miniD + 50 - (abs(error)/1000)*100);

  int del = millis() - t1;
  if(del < 4){
    delay(4 - del); 
  }
}
