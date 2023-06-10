#include "ada4254.h"
const int slaveSPISelectPin = 10;
ADA4254 ada(&SPI, slaveSPISelectPin, 0, 1);

void setup() {
  Serial.begin(115200);
  pinMode(slaveSPISelectPin, OUTPUT);
  SPI.begin();
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  while (Serial.println(ada.available())) {
    break;
  }
  ada.init();
  ada.calibration();
  //  ada.connectInputP20mv();
  //  ada.connectInputM20mv();
  ada.connectInputA();
  ada.writeRegister(0x0A, 223);
  ada.writeRegister(0x0A, 191);
  ada.writeRegister(0x07, 2);
  //  ada.writeRegister(0x03, 2);
  ada.setGain(GAIN128);
  for ( int i = 0; i < 10; i++) {
    ada.readRegister(i);
  }
}

void loop()
{
  //  ada.writeRegister(1, 1);
  //  aGain(GAIN1);
  //  ada.readRegister(0);
  //  ada.readRegister(6);
  //  ada.readRegister(3);
  //  ada.readRegister(4);
  //  ada.readRegister(4);
  //  ada.readRegister(0x0B);
  //  ada.readRegister(0x0A);
  //  Serial.println("______________________________");
  //  ada.connectInputB();
  //  Serial.print("inpuda.sett mux setting B: ");
  //  Serial.println(ada.connectInputB(), BIN);
  //  Serial.println("");
  //  delay(1000);
  //  ada.readRegister(0);
  //  Serial.println(ada.chekRegister(6, 12), BIN);
  //  delay(10000);
  //  for ( int i = 0; i < 10; i++) {
      ada.readRegister(0);
  //  }
  delay(1000);
}
