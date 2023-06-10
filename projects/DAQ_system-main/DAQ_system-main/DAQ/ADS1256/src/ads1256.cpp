#include "ads1256.h"
#include "Arduino.h"
#include <SPI.h> //SPI communication

const String ADS1256::DRDYIssue = "*Issue on DRDYPin. Posssible issue - unplugged power, or code is written for "
                                  "multiple boards but only one is connected.";

SPISettings ADS1256SPISettings(SPISpeedADS1256, MSBFIRST, SPI_MODE1);

ADS1256::ADS1256() {}

double ADS1256::convertToVoltage()
{

  if (_registerData >> 23 == 1) // if the 24th bit (sign) is 1, the number is negative
  {
    _registerData = _registerData - 16777216; // conversion for the negative sign
    //"mirroring" around zero
  }
  float a = _registerData * 1.0;
  a = a / 1.66220615;
  // a = a/128.0;
  a = a / 1000.0;
  // a = a/128;
  // _voltage = ((2 * _VREF) / 8388608) * _registerData; // 2.5 = Vref; 8388608 = 2^{23} - 1
  // Basically, dividing the positive range with the resolution and multiplying
  // with the bits
  return a;
}

// wait for DRDY PIN to go LOW. Also if time taken is greater than 500ms, error is printed.
void ADS1256::waitforDRDY()
{
  uint32_t startTime = millis();
  while (digitalRead(_DRDYPin) == true)
  {
    if (millis() - startTime > 500)
    {
      // Serial.println(DRDYIssue);
      delay(1);
    }
  }
}

unsigned long ADS1256::readRegister(uint8_t registerAddress)
{
  // Relevant video: https://youtu.be/KQ0nWjM-MtI
  waitforDRDY();
  SPI.beginTransaction(ADS1256SPISettings);
  // SPI_MODE1 = output edge: rising, data capture: falling; clock polarity: 0, clock phase: 1.

  // CS must stay LOW during the entire sequence [Ref: P34, T24]

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

void ADS1256::writeRegister(uint8_t registerAddress, uint8_t registerValueW)
{
  // Relevant video: https://youtu.be/KQ0nWjM-MtI
  int retry = 0;
  while (true)
  {
    waitforDRDY();
    SPI.beginTransaction(ADS1256SPISettings);
    // SPI_MODE1 = output edge: rising, data capture: falling; clock polarity: 0, clock phase: 1.

    // CS must stay LOW during the entire sequence [Ref: P34, T24]
    digitalWrite(_CSPin, LOW);            // CS_pin goes LOW
    delayMicroseconds(5);                 // see t6 in the datasheet
    SPI.transfer(WREG | registerAddress); // 0x50 = WREG
    SPI.transfer(ZERO);                   //
    SPI.transfer(registerValueW);         // we write the value to the above selected register
    delayMicroseconds(10);                // required register write fails.
    digitalWrite(_CSPin, HIGH);           // CS_pin goes HIGH
    SPI.endTransaction();

    long regRecvValue = readRegister(registerAddress);

    // note that for 0x00 STATUSREG, bits 7-4 are factory ID and they are read only. So set and write value will differ.
    // Hence doing bitwise or and removing first four characters.
    if (registerAddress == 0x00)
    {
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
    if (registerValueW != regRecvValue)
    {
      retry++;
      delay(10);
    }
    else
    {
      retry = 0;
      break;
    }

    retry++;
    if (retry > 5)
    {
      break;
    }
  }

  if (retry != 0)
  {
    Serial.print("Write to Register 0x");
    Serial.print(registerAddress, HEX);
    Serial.println(" failed! on SetRegisterValue");
  }
  else
  {
    Serial.println("success on SetRegisterValue");
  }

  Serial.println("");
};

void ADS1256 ::sendDirectCommand(uint8_t directCommand)
{
  // Direct commands can be found in the datasheet Page 34, Table 24.
  // Use binary, hex or dec format.
  // Here, we want to use everything EXCEPT: RDATA, RDATAC, SDATAC, RREG, WREG
  // We don't want to involve DRDY here. We just write, but don't read anything.

  // Start SPI
  waitforDRDY();
  SPI.beginTransaction(ADS1256SPISettings);
  digitalWrite(_CSPin, LOW);   // REF: P34: "CS must stay low during the entire command sequence"
  delayMicroseconds(5);        // t6 - maybe not necessary
  SPI.transfer(directCommand); // Send Command
  delayMicroseconds(5);        // t6 - maybe not necessary
  digitalWrite(_CSPin, HIGH);  // REF: P34: "CS must stay low during the entire command sequence"
  SPI.endTransaction();        //
  delay(300);                  // required because it takes long time to calibrate when SELFCAL is used.

  waitforDRDY(); // these are not directly required but still issued to make sure no command is send after SELFCAL is
                 // sent.
};

// required CSPin, drate, adc speed, gain.
void ADS1256::begin(int CSPin, int DRDYPin, int RESETPin, int PGA, int DRATE)
{

  Serial.println("*ADS1256 Initialization begin");
  // SPI.begin();

  // setting up private variables.
  _CSPin = CSPin;
  _DRDYPin = DRDYPin;
  _RESETPin = RESETPin;
  _PGA = PGA;
  _DRATE = DRATE;

  // Setting up the pins
  // Chip select
  pinMode(_CSPin, OUTPUT);   // Chip select is an output
  digitalWrite(_CSPin, LOW); // Chip select LOW

  // DRDY
  pinMode(_DRDYPin, INPUT);     // DRDY is an input
  pinMode(_RESETPin, OUTPUT);   // RESET pin is an output
  digitalWrite(_RESETPin, LOW); // RESET is set to low

  // reseting the board
  delay(500);                    // Wait
  digitalWrite(_RESETPin, HIGH); // RESET is set to high
  delay(500);                    // Wait
  // start of - check if ads1256 is functioning by monitoring DRDY pin change
  uint32_t startTime = millis();
  byte currentDRDYPin = digitalRead(_DRDYPin);
  while (true)
  {
    if (millis() - startTime > 500)
    {
      // Serial.println(DRDYIssue);
      delay(100);
    }
    if (digitalRead(_DRDYPin) != currentDRDYPin)
    {
      break;
    }
  }
  while (true)
  {
    if (millis() - startTime > 500)
    {
      // Serial.println(DRDYIssue);
      delay(100);
    }
    if (digitalRead(_DRDYPin) == currentDRDYPin)
    {
      break;
    }
  }
  // end of - check ads1256 is functioning by monitoring DRDY pin change

  Serial.println("*ADS1256 Initialization finished");
}

void ADS1256::resetADS1256()
{
  Serial.println("*Reset Started!");
  SPI.beginTransaction(ADS1256SPISettings); // initialize SPI with  clock, MSB first, SPI Mode1
  digitalWrite(_CSPin, LOW);                // _CSPin goes LOW
  delayMicroseconds(10);                    // should be - issue RESET after DRDY goes low
  SPI.transfer(RESET);                      // Reset
  delay(2);                                 // Minimum 0.6 ms required for Reset to finish.
  SPI.transfer(SDATAC);                     // Issue SDATAC to read data continously
  delayMicroseconds(100);                   //
  digitalWrite(_CSPin, HIGH);               // _CSPin goes HIGH
  SPI.endTransaction();                     //
  Serial.println("*Reset DONE!");           // confirmation message
}

void ADS1256::userDefaultRegisters()
{
  // This function is "manually" updating the values of the registers then reads
  // them back. This function should be used in the setup() after performing an
  // initialization-reset process
  // I use the below listed settings for my "startup configuration"
  /*
    REG   VAL     USE
    0     54      Status Register, Everything Is Default, Except ACAL and BUFEN
    1     1       Multiplexer Register, AIN0 POS, AIN1 POS
    2     0       ADCON, Everything is OFF, PGA = 1
    3     99      DataRate = 50 SPS
*/
  // We update the 4 registers that we are going to use

  Serial.println("*Register defaults started!");
  // note that for STATUSREG, bits 7-4 are factory ID and they are read only. So set and write value will differ.
  writeRegister(STATUSREG, ZERO | ZERO | ZERO | ZERO | ORDER_MSB | ACAL_OFF | BUFEN_OFF | ZERO);
  // STATUS.
  //    Disabled BUFEN as max input voltage cannot be higher than AVDD-2 V.
  //    Disabled ACAL as After RESET, self-calibration is performed, regardless of ACAL bit before RESET.
  writeRegister(MUXREG, P_AIN0 | N_AINCOM);                  // MUX PIN0 on +ve and PIN Common on -ve
  writeRegister(ADCONREG, ZERO | CLK_OFF | SDCS_OFF | _PGA); // CLK_OFF:2, SDCS_OFF:2, _PGA:3
  writeRegister(DRATEREG, _DRATE);                           // DRATE - DEC[99] - 50 SPS
  sendDirectCommand(SELFCAL);                                // SELFCAL
  // delay(200);

  Serial.println("*Register defaults updated!");
}

// read channel 0-7.
double ADS1256::readChannelContinuousMode(int pinNo)
{
  // Relevant video: https://youtu.be/GBWJdyjRIdM

  _pinNo = pinNo;

  _registerData = 0;
  SPI.beginTransaction(ADS1256SPISettings);

  /*
    we pinNo through all the 8 single-ended channels with the RDATAC INFO:
    RDATAC = B00000011
    SYNC = B11111100
    WAKEUP = B11111111
    //---------------------------------------------------------------------------------------------
    Some comments regarding the cycling:
    When we start the ADS1256, the preconfiguration already sets the MUX to
    [AIN0+AINCOM]. When we start the RDATAC (this function), the default MUX
    ([AIN0+AINCOM]) will be included in the cycling which means that the first
    readout will be the [AIN0+AINCOM]. But, before we read the data from the
    [AIN0+AINCOM], we have to switch to the next register already, then start
    RDATA. This is demonstrated in Figure 19 on Page 21 of the datasheet.

    Therefore, in order to get the 8 channels nicely read and formatted, we
    have to start the pinNo with the 2nd input of the ADS1256 ([AIN1+AINCOM])
    and finish with the first ([AIN0+AINCOM]).

       \ CH1 | CH2 CH3 CH4 CH5 CH6 CH7 CH8 \ CH1 | CH2 CH3 ...

    The switch-case is between the  two '|' characters
    The output (one line of values) is between the two '\' characters.
    */

  //-------------------------------------------------------------------------------------------
  // Steps are on Page21
  // Step 1. - Updating MUX
  waitforDRDY();

  digitalWrite(_CSPin, LOW);   // CS must stay LOW during the entire sequence [Ref: P34, T24]
  SPI.transfer(WREG | MUXREG); // 0x50 = WREG //1 = MUX
  SPI.transfer(ZERO);
  switch (_pinNo)
  {
  // // Channels are written manually, so we save time on switching the SPI.beginTransaction on and off.
  case 0:                          // set for channel 1
    SPI.transfer(P_AIN0 | N_AIN1); //
    break;                         //
  case 1:                          // set for channel 2
    SPI.transfer(P_AIN2 | N_AIN3); //
    break;                         //
  case 2:                          // set for channel 3
    SPI.transfer(P_AIN4 | N_AIN5); //
    break;                         //
    // case 3:                            // set for channel 4
    //   SPI.transfer(P_AIN4 | N_AINCOM); //
    //   break;                           //
    // case 4:                            // set for channel 5
    //   SPI.transfer(P_AIN5 | N_AINCOM); //
    //   break;                           //
    // case 5:                            // set for channel 6
    //   SPI.transfer(P_AIN6 | N_AINCOM); //
    //   break;                           //
    // case 6:                            // set for channel 7
    //   SPI.transfer(P_AIN7 | N_AINCOM); //
    //   break;                           //
    // case 7:                            // set for channel 0
    // SPI.transfer(P_AIN0 | N_AINCOM); //
    //   break;                           //
  }

  // Step 2 - Issue SYNC (1111 1100) command
  SPI.transfer(SYNC); // SYNC

  delayMicroseconds(4); // t11 delay 24*tau = 3.125 us //delay should be larger, so we delay by 4 us
  // SPI.transfer(B11111111); // WAKEUP
  SPI.transfer(WAKEUP); // WAKEUP

  // Step 3 - Issue RDATA (0000 0001) command
  // SPI.transfer(B00000001);
  SPI.transfer(RDATA);

  // Wait t6 time (~6.51 us) REF: P34, FIG:30.
  delayMicroseconds(7);

  // step out the data: MSB | mid-byte | LSB,
  // registerData is ZERO
  _registerData |= SPI.transfer(0x0F); // MSB comes in, first 8 bit is updated | '|=' compound bitwise OR operator
  _registerData <<= 8;                 // MSB gets shifted LEFT by 8 bits
  _registerData |= SPI.transfer(0x0F); // MSB | Mid-byte
  _registerData <<= 8;                 // MSB | Mid-byte gets shifted LEFT by 8 bits
  _registerData |= SPI.transfer(0x0F); //(MSB | Mid-byte) | LSB - final result |
                                       // Now DRDY should go HIGH automatically
  // return _registerData;

  _voltage = convertToVoltage();
  SPI.endTransaction();
  digitalWrite(_CSPin, HIGH); // We finished the command sequence, so we switch it back to HIGH

  return _voltage;

  // Expected output when using a resistor ladder of 1k resistors and the
  // ~+5V output of the Arduino: Formatting  Channel 1 Channel 2 Channel 3
  // Channel 4 Channel 5 Channel 6 Channel 7 Channel 8
  /*
  12:41:40.280 -> 4.78714609  4.16558074  3.55143761  2.96154289  2.37305951
  1.78396224  1.19539093  0.60204453 12:41:40.450
  -> 4.78708410  4.16603088  3.55298733  2.96177434  2.37242603  1.78440055
  1.19551980
  0.60218434 12:41:40.620
  -> 4.78826045  4.16563510  3.55332374  2.96192693  2.37245225  1.78419756
  1.19552350
  0.60213699
  */
}
