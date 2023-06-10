#include "ads1256.h"
#include "Arduino.h"
#include <SPI.h> //SPI communication
#include "filters.h"
const String ADS1256::DRDYIssue = "*Issue on DRDYPin. Posssible issue - unplugged power, or code is written for "
                                    "multiple boards but only one is connected.";
SPISettings ADS1256SPISettings(SPISpeedADS1256, MSBFIRST, SPI_MODE1);

ADS1256::ADS1256() {}


double ADS1256::convertToVoltage() {
  if (_registerData >> 23 == 1) // if the 24th bit (sign) is 1, the number is negative
  {
    _registerData = _registerData - 16777216; // conversion for the negative sign
    //"mirroring" around zero
  }
  _voltage = ((2 * _VREF) / 8388608) * _registerData; // 2.5 = Vref; 8388608 = 2^{23} - 1
  // Basically, dividing the positive range with the resolution and multiplying
  // with the bits
  return  _registerData;
}

void ADS1256::waitforDRDY() {
  uint32_t startTime = millis();
  while (digitalRead(_DRDYPin)) {
    if (millis() - startTime > 500) {
      Serial.println(DRDYIssue);
      delay(100);
    }
  }
}

unsigned long ADS1256::readRegister(uint8_t registerAddress) {
  waitforDRDY();
  SPI.beginTransaction(ADS1256SPISettings);
  digitalWrite(_CSPin, LOW);            // CS_pin goes LOW
  SPI.transfer(RREG | registerAddress); // 0x10 = RREG
  SPI.transfer(ZERO);
  delayMicroseconds(5);                 // see t6 in the datasheet
  _registerValueR = SPI.transfer(0xFF); // 0xFF is sent to the ADS1256 which returns us the register value
  digitalWrite(_CSPin, HIGH); // CS_pin goes HIGH
  SPI.endTransaction();
  waitforDRDY();
  return _registerValueR; // return the registers value
}

void ADS1256::writeRegister(uint8_t registerAddress, uint8_t registerValueW) {
  int retry = 0;
  while (true) {
    waitforDRDY();
    SPI.beginTransaction(ADS1256SPISettings);
    digitalWrite(_CSPin, LOW);            // CS_pin goes LOW
    delayMicroseconds(5);                 // see t6 in the datasheet
    SPI.transfer(WREG | registerAddress); // 0x50 = WREG
    SPI.transfer(ZERO);                   //
    SPI.transfer(registerValueW);         // we write the value to the above selected register
    delayMicroseconds(10);                // required register write fails.
    digitalWrite(_CSPin, HIGH);           // CS_pin goes HIGH
    SPI.endTransaction();

    long regRecvValue = readRegister(registerAddress);
    if (registerAddress == 0x00) {
      regRecvValue |= B11110000;
      regRecvValue -= B11110000;

      registerValueW |= B11110000;
      registerValueW -= B11110000;
    }

    Serial.print("required register value ");
    Serial.println(registerValueW);
    Serial.print("achieved register value ");
    Serial.println(regRecvValue);

    // Check if write was succesfull
    if (registerValueW != regRecvValue) {
      retry++;
      delay(50);
    } else {
      retry = 0;
      break;
    }

    retry++;
    if (retry > 5) {
      break;
    }
  }

  if (retry != 2) {
    Serial.print("Write to Register 0x");
    Serial.print(registerAddress, HEX);
    Serial.println(" failed! on SetRegisterValue");
  } else {
    Serial.println("success on SetRegisterValue");
  }

  Serial.println("");
};

void ADS1256 ::sendDirectCommand(uint8_t directCommand) {
  waitforDRDY();
  SPI.beginTransaction(ADS1256SPISettings);
  digitalWrite(_CSPin, LOW);   // REF: P34: "CS must stay low during the entire command sequence"
  delayMicroseconds(5);        // t6 - maybe not necessary
  SPI.transfer(directCommand); // Send Command
  delayMicroseconds(5);        // t6 - maybe not necessary
  digitalWrite(_CSPin, HIGH);  // REF: P34: "CS must stay low during the entire command sequence"
  SPI.endTransaction();        //
  delay(300);                  // required because it takes long time to calibrate when SELFCAL is used.
  waitforDRDY(); 
};

void ADS1256::begin(int CSPin, int DRDYPin, int RESETPin, int PGA, int DRATE) {
  Serial.println("*ADS1256 Initialization begin");
  _CSPin = CSPin;
  _DRDYPin = 24;
  _RESETPin = RESETPin;
  _PGA = PGA;
  _DRATE = DRATE;
  pinMode(_CSPin, OUTPUT);   // Chip select is an output
  digitalWrite(_CSPin, LOW); // Chip select LOW
  pinMode(_DRDYPin, INPUT);     // DRDY is an input
  pinMode(_RESETPin, OUTPUT);   // RESET pin is an output
  digitalWrite(_RESETPin, LOW); // RESET is set to low
  delay(500);                    // Wait
  digitalWrite(_RESETPin, HIGH); // RESET is set to high
  delay(500);                    // Wait
  uint32_t startTime = millis();
  byte currentDRDYPin = digitalRead(_DRDYPin);
  while (true) {
    if (millis() - startTime > 500) {
      Serial.println(DRDYIssue);
      delay(100);
    }
    if (digitalRead(_DRDYPin) != currentDRDYPin) {
      break;
    }
  }
  while (true) {
    if (millis() - startTime > 500) {
      Serial.println(DRDYIssue);
      delay(100);
    }
    if (digitalRead(_DRDYPin) == currentDRDYPin) {
      break;
    }
  }
  Serial.println("*ADS1256 Initialization finished");
}

void ADS1256::resetADS1256() {
  Serial.println("*Reset Started!");
  SPI.beginTransaction(ADS1256SPISettings); // initialize SPI with  clock, MSB first, SPI Mode1
  digitalWrite(_CSPin, LOW);                                        // _CSPin goes LOW
  delayMicroseconds(10);                                            // should be - issue RESET after DRDY goes low
  SPI.transfer(RESET);                                              // Reset
  delay(2);                                                         // Minimum 0.6 ms required for Reset to finish.
  SPI.transfer(SDATAC);                                             // Issue SDATAC to read data continously
  delayMicroseconds(100);                                           //
  digitalWrite(_CSPin, HIGH);                                       // _CSPin goes HIGH
  SPI.endTransaction();                                             //
  Serial.println("*Reset DONE!");                                   // confirmation message
}

void ADS1256::userDefaultRegisters() {
  Serial.println("*Register defaults started!");
  writeRegister(STATUSREG, ZERO | ZERO | ZERO | ZERO | ORDER_MSB | ACAL_OFF | BUFEN_OFF | ZERO);
  writeRegister(MUXREG, P_AIN0 | N_AINCOM);                  // MUX PIN0 on +ve and PIN Common on -ve
  writeRegister(ADCONREG, ZERO | CLK_OFF | SDCS_OFF | _PGA); // CLK_OFF:2, SDCS_OFF:2, _PGA:3
  writeRegister(DRATEREG, _DRATE);                           // DRATE - DEC[99] - 50 SPS
  sendDirectCommand(SELFCAL);                                // SELFCAL
  Serial.println("*Register defaults updated!");
}

double ADS1256::readChannelContinuousMode(int pinNo) {
  _pinNo = pinNo;
  _registerData = 0;
  SPI.beginTransaction(ADS1256SPISettings);
  waitforDRDY();
  digitalWrite(_CSPin, LOW);   // CS must stay LOW during the entire sequence [Ref: P34, T24]
  SPI.transfer(WREG | MUXREG); // 0x50 = WREG //1 = MUX
  SPI.transfer(ZERO);
  // switch (_pinNo) {
  // case 0:                            // set for channel 1
  //   SPI.transfer(P_AIN1 | N_AINCOM); //
  //   break;                           //
  // case 1:                            // set for channel 2
  //   SPI.transfer(P_AIN2 | N_AINCOM); //
  //   break;                           //
  // case 2:                            // set for channel 3
  //   SPI.transfer(P_AIN3 | N_AINCOM); //
  //   break;                           //
  // case 3:                            // set for channel 4
  //   SPI.transfer(P_AIN4 | N_AINCOM); //
  //   break;                           //
  // case 4:                            // set for channel 5
  //   SPI.transfer(P_AIN5 | N_AINCOM); //
  //   break;                           //
  // case 5:                            // set for channel 6
  //   SPI.transfer(P_AIN6 | P_AIN7); //
  //   break;                           //
  // case 6:                            // set for channel 7
  //   SPI.transfer(P_AIN1 | N_AINCOM); //
  //   break;                           //
  // case 7:                            // set for channel 0
  //   SPI.transfer(P_AIN0 | N_AINCOM); //
  //   break;                           //
  //  }
  SPI.transfer(P_AIN6 | N_AIN7); //
  // SPI.transfer(P_AIN0 |  N_AINCOM); //
  SPI.transfer(SYNC); // SYNC
  delayMicroseconds(4); // t11 delay 24*tau = 3.125 us //delay should be larger, so we delay by 4 us
  SPI.transfer(WAKEUP); // WAKEUP
  SPI.transfer(RDATA);
  delayMicroseconds(7);
  _registerData |= SPI.transfer(0x0F); // MSB comes in, first 8 bit is updated | '|=' compound bitwise OR operator
  _registerData <<= 8;                 // MSB gets shifted LEFT by 8 bits
  _registerData |= SPI.transfer(0x0F); // MSB | Mid-byte
  _registerData <<= 8;                 // MSB | Mid-byte gets shifted LEFT by 8 bits
  _registerData |= SPI.transfer(0x0F); //(MSB | Mid-byte) | LSB - final result |
  _voltage = convertToVoltage();
  SPI.endTransaction();
  digitalWrite(_CSPin, HIGH); // We finished the command sequence, so we switch it back to HIGH
  return _voltage;
}
