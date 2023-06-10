#include "ads1256Setup.h" // search first in same directory
#include <Arduino.h>      // search first in same compiler/IDE directory.
#include <SPI.h>
int a  = 1;
void setup() {
  Serial.begin(1000000);
  SPI.begin();
  setupADS1256();
  Serial.println("leaving setup");
}
void loop() {
  getDataFromADS1256();
  delay(1);
}

