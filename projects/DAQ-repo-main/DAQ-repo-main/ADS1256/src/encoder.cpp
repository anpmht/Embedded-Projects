#define ENCODER_H
#include "encoder.h"
#include <SPI.h>
#include "Arduino.h"
long time = 0;
long pTime = 0;
bool flag = false;
float RPM = 0;
int pos = 0;
uint8_t resolution = RES14;
uint16_t currentPosition; // 16-bit response from encoder
bool binaryArray[16];     // after receiving the position we will populate this array and use it for calculating the checksum
int prevPos = 0;
uint16_t encoderPosition = 0;

void getPosition()
{
    SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
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
        // Serial.print("<");
        // Serial.print("\t");
        // Serial.print("Encoder 0: ");
        // Serial.print(encoderPosition, DEC); // print the position in decimal format
        // Serial.print("\t");
        // Serial.println(">");
    }
    pos = encoderPosition;
    // Serial.println(pos);
    if (pos < 16000 && pos > 14000 && flag == true)
    {
        RPM = (millis() - pTime) / 1000.0;
        RPM = 1 / RPM;
        RPM = RPM * 60;
        flag = false;
        pTime = millis();
        flag = false;
    }
    if (pos < 1000){
        flag = true;
    }
    SPI.endTransaction();
    prevPos = pos;
    // }
    Serial.print("     ");
    Serial.print(RPM);
    Serial.print("  RPM");
    Serial.print("     ");
    Serial.print(map(pos,0,16384,0,360));
    Serial.print("deg");
    Serial.print("     ");
    Serial.print(analogRead(27));
    Serial.print("  mv");
    Serial.print("     ");
    // Serial.print(pos);
    // Serial.print("  ADC");
    // Serial.print("     ");
}