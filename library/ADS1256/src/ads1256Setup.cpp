#include "ads1256Setup.h"
#include "ads1256.h"
#include <Arduino.h>
#include <SimpleKalmanFilter.h>
double readingVoltage;

ADS1256 firstADS1256;
ADS1256 secondADS1256;

const int noOfBoards = 1;
double readingValueArray[noOfBoards][8];

int boardNo = 0;
int pinNo = 0;
unsigned long average;
SimpleKalmanFilter strainKalmanFilter(1, 1, 0.001);
unsigned long estimated_strain;

float xn1 = 0;
float yn1 = 0;
int k = 0;
float T = 0.000180;
float Tn1 = micros();
float RC = 0.007957;


void setupADS1256() {
  delay(10);
  firstADS1256.begin(26, 24, 25, PGA_1, DR_30000);
  firstADS1256.resetADS1256();
  firstADS1256.userDefaultRegisters();
  Serial.println("<setupADS1256 finished>");
  Serial.println("");
}

void getDataFromADS1256() {
  boardNo = 0;
  readingVoltage = firstADS1256.readChannelContinuousMode(pinNo);
  readingValueArray[boardNo][pinNo] = readingVoltage;
  pinNo += 1;
  if (pinNo >= 8) {
    // Serial.print("<");
    // Serial.print("   ");
    for (int currentBoard = 0; currentBoard < noOfBoards; currentBoard++) {
      for (int pin = 0; pin <= 0; pin++) {
        // average = readingValueArray[currentBoard][pin];
        // estimated_strain = strainKalmanFilter.updateEstimate(average);
        // float filteredval = f.filterIn(average);
        // Serial.print(readingValueArray[currentBoard][pin],8);
        // Serial.print("\t");
        // Test signal
        float xn = readingValueArray[currentBoard][pin];
        float yn = 0.969*yn1 + 0.0155*xn + 0.0155*xn1;
        xn1 = xn;
        yn1 = yn;
        Tn1 = micros();

        // float Vout = 340.450261*(T*xn+0.0039788*xn1);
          Serial.print("<");
          Serial.print("   ");
          Serial.print(xn);
          Serial.print("\t");
          Serial.print(yn);
          Serial.print("\t");
          Serial.println(">");
        k = k+1;
        readingValueArray[currentBoard][pin] = 0; // reseting the value
      }

      // Serial.print(average/8);
      // Serial.print("\t");
      average = 0;
     }
    // Serial.println(">");
    pinNo = 0;
  }
}
