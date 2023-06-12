#include "ads1256Setup.h"
#include "ads1256.h"
#include <Arduino.h>
// #include <encoder.h>
double readingVoltage;

ADS1256 firstADS1256;
ADS1256 secondADS1256;

const int noOfBoards = 1;
double readingValueArray[noOfBoards][8];

int boardNo = 0;
int pinNo = 0;

// Begin - ADS1256 setup
/*--------------------------------------------------------------------------------
--------------------------------------------------------------------------------*/

// required CSPin, drate, adc speed, gain.
void setupADS1256()
{
  delay(10);
  firstADS1256.begin(26, 24, 25, PGA_1, DR_30000);
  // firstADS1256.begin(10, 2, 3, PGA_1, DR_2000);
  firstADS1256.resetADS1256();
  firstADS1256.userDefaultRegisters();
  // secondADS1256.begin(9, 4, 5, PGA_1, DR_30000);
  // secondADS1256.resetADS1256();
  // secondADS1256.userDefaultRegisters();
  Serial.println("<setupADS1256 finished>");
  Serial.println("");
}

void getDataFromADS1256()
{

  // boardNo = 0;
  // readingVoltage = firstADS1256.readChannelContinuousMode(0);
  // readingValueArray[0][0] = readingVoltage;
  // boardNo = 1;
  // readingVoltage = secondADS1256.readChannelContinuousMode(1);
  // readingValueArray[boardNo][1] = readingVoltage;

  // pinNo += 1;
  // if (pinNo >= 3)
  // {
  //   Serial.print(micros());
  //   Serial.print(";");

  // for (int pin = 0; pin < 2; pin++)
  // {
  //   Serial.print("Value");
  //   Serial.print(pin);
  //   Serial.print(":");
  //   Serial.print(readingValueArray[0][pin], 8);
  //   Serial.print("   mV");
  //   Serial.print("  ,  ");
  //   readingValueArray[0][pin] = 0; // reseting the value
  // }
  // pinNo = 0;
  // getPosition();
  // }

  // Serial.println(">");
  // Serial.print(micros());
  // Serial.print(",");
  readingVoltage = firstADS1256.readChannelContinuousMode(0);
  // Serial.print("R2,\t");
  Serial.print(readingVoltage, 2);
  Serial.print(",");
  readingVoltage = readingVoltage*206/(64*10.71*2);
  
  // Serial.print(readingVoltage, 2);
   
  readingVoltage = firstADS1256.readChannelContinuousMode(1);
  // Serial.print("\t,L1,\t");
  Serial.print(readingVoltage, 2);
  Serial.print(",");
  readingVoltage = readingVoltage*206/(64*10.71*2);
  // Serial.print(readingVoltage, 2);

  readingVoltage = firstADS1256.readChannelContinuousMode(2);
  // Serial.print("\t,L2,\t");
  Serial.print(readingVoltage, 2);
  Serial.print(",");
  // readingVoltage = readingVoltage*206/(64*10.71*2);
  // Serial.print(readingVoltage, 2);

  readingVoltage = firstADS1256.readChannelContinuousMode(3);
  // Serial.print("\t,R1,\t");
  Serial.print(readingVoltage, 2);
  Serial.print(",");
  // readingVoltage = readingVoltage*206/(64*10.71*2);
  // Serial.print(readingVoltage, 2);
  // Serial.print("\t");
  // Serial.print(micros());
}

// End - ADS1256 setup