#include "ads1256Setup.h" // search first in same directory
#include <Arduino.h>      // search first in same compiler/IDE directory.
#include <SPI.h>
#include "ada4254.h"
const int slaveSPISelectPin = 10;
ADA4254 ada(&SPI, slaveSPISelectPin, 0, 1);

/* SPI commands */
#define AMT22_NOP 0x00
#define AMT22_RESET 0x60
#define AMT22_ZERO 0x70

/* Define special ascii characters */
#define NEWLINE 0x0A
#define TAB 0x09

/* We will use these define macros so we can write code once compatible with 12 or 14 bit encoders */
#define RES12 12
#define RES14 14

/* SPI pins */
#define ENC_0 4
long time = 0;
long pTime = 0;
bool flag = false;
float RPM = 0;
int pos = 0;

void setup()
{ 
  Serial.begin(115200);
  // SPI.setClockDivider(SPI_CLOCK_DIV32);    // 500 kHz
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(11, INPUT);
  SPI.begin();
  // setupADS1256();
  pinMode(ENC_0, OUTPUT);
  digitalWrite(ENC_0, HIGH);
}

void loop()
{
  // mainLoopSerialInput();

  uint16_t encoderPosition;
  // while (1)
  // {
  // getDataFromADS1256();
  //    encoderPosition = getPositionSPI(ENC_0, RES14);
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //  SPISettings ADS1256SPISettings(SPISpeedADS1256, MSBFIRST, SPI_MODE1);
  SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
  uint8_t resolution = RES14;
  uint16_t currentPosition; // 16-bit response from encoder
  bool binaryArray[16];     // after receiving the position we will populate this array and use it for calculating the checksum

  //    currentPosition = spiWriteRead(AMT22_NOP, ENC_0, false) << 8; //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // holder for the received over SPI
  uint8_t data;
  digitalWrite(ENC_0, LOW);
  delayMicroseconds(3);
  data = SPI.transfer(AMT22_NOP);
  delayMicroseconds(3);       // There is also a minimum time after clocking that CS should remain asserted before we release it
  digitalWrite(ENC_0, false); // if releaseLine is high set it high else it stays low
  currentPosition = data << 8;
  delayMicroseconds(3);

  //    currentPosition |= spiWriteRead(AMT22_NOP, ENC_0, true);//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  digitalWrite(ENC_0, LOW);
  delayMicroseconds(3);
  data = SPI.transfer(AMT22_NOP);
  delayMicroseconds(3);      // There is also a minimum time after clocking that CS should remain asserted before we release it
  digitalWrite(ENC_0, HIGH); // if releaseLine is high set it high else it stays low
  currentPosition |= data;
  delayMicroseconds(3);


/*
  for (int i = 0; i < 16; i++)
    binaryArray[i] = (0x01) & (currentPosition >> (i));
  if ((binaryArray[15] == !(binaryArray[13] ^ binaryArray[11] ^ binaryArray[9] ^ binaryArray[7] ^ binaryArray[5] ^ binaryArray[3] ^ binaryArray[1])) && (binaryArray[14] == !(binaryArray[12] ^ binaryArray[10] ^ binaryArray[8] ^ binaryArray[6] ^ binaryArray[4] ^ binaryArray[2] ^ binaryArray[0])))
  {
    currentPosition &= 0x3FFF;
  }
  else
  {
    currentPosition = 0xFFFF; // bad position
  }

  */
  if ((resolution == RES12) && (currentPosition != 0xFFFF))
    currentPosition = currentPosition >> 2;

  encoderPosition = currentPosition;
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //    while (encoderPosition == 0xFFFF && ++attempts < 3)
  //    {
  //      encoderPosition = getPositionSPI(ENC_0, RES14); //try again
  //
  //    }
  if (encoderPosition == 0xFFFF) // position is bad, let the user know how many times we tried
  {
    //            Serial.print("Encoder 0 error. Attempts: ");
    //            Serial.print(attempts, DEC); //print out the number in decimal format. attempts - 1 is used since we post incremented the loop
    //            Serial.write(NEWLINE);
  }
  else // position was good, print to serial stream
  {
    Serial.print("<");
    Serial.print("\t");
    Serial.print("Encoder 0: ");
    Serial.print(encoderPosition, DEC); // print the position in decimal format
    Serial.print("\t");
    Serial.println(">");
  }
  pos = encoderPosition;
  Serial.println(pos);
  if (pos > 15000 && pos < 16000 && flag == true)
  {
    RPM = (millis() - pTime) / 1000.0;
    RPM = 1 / RPM;
    RPM = RPM * 60;
    Serial.print("RPM = ");
    Serial.println(RPM);
    flag = false;
    pTime = millis();
  }
  if (encoderPosition < 2000)
  {
    flag = true;
  }
  SPI.endTransaction();
  // }

  // Serial.print(" ");
  // Serial.print(RPM);
  // Serial.print("   RPM");
  // Serial.print(" ");
  Serial.println(">");
}