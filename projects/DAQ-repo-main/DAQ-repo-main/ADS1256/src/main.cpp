#include "ads1256Setup.h" // search first in same directory
#include <Arduino.h>      // search first in same compiler/IDE directory.
#include <SPI.h>
#include "encoder.h"
#include "ada4254.h"
const int slaveSPISelectPin1 = 10;
const int slaveSPISelectPin2 = 9;
const int slaveSPISelectPin3 = 8;
ADA4254 ada(&SPI, slaveSPISelectPin1, 0, 1);
void setup()
{ 
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  Serial.begin(115200);
  SPI.begin();
  /////////////////////////////////////////////////////////////
  for (int i = 8; i < 11; i++)
  {
    ada.init(i);
    ada.calibration(i);
    ada.connectInputA(i);
    ada.writeRegister(0x0A, 223, i);
    ada.writeRegister(0x0A, 191, i);
    ada.writeRegister(0x07, 2, i);
    //  ada.writeRegister(0x03, 2);
    ada.setGain(GAIN128, i);
    for (int j = 0; j < 11; j++)
    {
      ada.readRegister(j, 10);
    }
  }
  digitalWrite(8,HIGH);
  digitalWrite(9,HIGH);
  digitalWrite(10,HIGH);
  /////////////////////////////////////////////////////////////
  setupADS1256();
  pinMode(ENC_0, OUTPUT);
  pinMode(27, INPUT);
  digitalWrite(ENC_0, HIGH);
  Serial.println("program started");
}

void loop()
{
  getDataFromADS1256();
}