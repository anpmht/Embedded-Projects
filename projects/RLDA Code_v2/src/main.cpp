#include "ads1256Setup.h" // search first in same directory
#include <Arduino.h>      // search first in same compiler/IDE directory.
#include <SPI.h>
#include "ada4254.h"
#include "Wire.h"
#include <Adafruit_Sensor.h>
#include "Adafruit_ADXL345_U.h"

const int slaveSPISelectPin = 10;
ADA4254 ada(&SPI, slaveSPISelectPin, 0, 1);

/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12344);
Adafruit_ADXL345_Unified accel1 = Adafruit_ADXL345_Unified(12344);

void setup()
{
  Serial.begin(115200);
  pinMode(21, INPUT);
  pinMode(22, INPUT);
  // pinMode(slaveSPISelectPin, OUTPUT);
  // SPI.begin();
  // SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  // while (Serial.println(ada.available())) {
  //   break;
  // }
  // ada.init();
  // ada.calibration();
  // //  ada.connectInputP20mv();
  // //  ada.connectInputM20mv();
  // ada.connectInputA();
  // ada.writeRegister(0x0A,223);
  // ada.writeRegister(0x0A,191);
  // ada.writeRegister(0x07,2);
  //   for( int i = 0; i<20;i++){
  //   ada.readRegister(i);
  //   }

  // ada.setGain(GAIN128);
  // SPI.end();
  // Serial.println("This demo expects 3 pieces of data - text, an integer and a floating point value");
  // Serial.println("Enter data in this style <HelloWorld, 12, 24.7>  ");
  // Serial.println();
  SPI.begin();
  setupADS1256();

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /* Initialise the sensor */
  if (!accel.begin(0x1d))
  {
    Serial.println("Ooops, accel0 is not connected ... Check your wiring!");
    while (1)
      ;
  }
  if (!accel1.begin(0x53))
  {
    Serial.println("Ooops, accel1 is not connected ... Check your wiring!");
    while (1)
      ;
  }
  accel.setRange(ADXL345_RANGE_16_G);
  accel1.setRange(ADXL345_RANGE_16_G);
  accel.setDataRate(ADXL345_DATARATE_400_HZ);
  accel1.setDataRate(ADXL345_DATARATE_3200_HZ);
  Serial.print("SGR");
  Serial.print(",");
  Serial.print("load_cell");
  Serial.print(",");
  Serial.print("xxxx");
  Serial.print(",");
  Serial.print("SGL");
  Serial.print(",");
  Serial.print("b_lvdt");
  Serial.print(",");
  Serial.print("f_lvdt");
  Serial.println(",xb,yb,zb,xf,yf,zf,micros,");
}

void loop()
{
  // mainLoopSerialInput();

  // /* Get a new sensor event */
  int ab = analogRead(21);
  int aa = analogRead(22);

  double dist_back = (aa / 1023.00) * 100.00;
  // aa = map(aa,0,1023,0,3300);
  // Serial.print(aa);
  // Serial.print(",  ");

  // double dist_front = 252.00 - (ab / 1023.00) * 250.00;
  double dist_front = 250.00 - (ab / 1023.00) * 250.00;
  // ab = map(ab,0,1023,0,3300);
  // Serial.print(ab);
  // Serial.print(",  ");

  getDataFromADS1256();
  // Serial.print(",");
  Serial.print(dist_front,3);
  Serial.print(",");
  Serial.print(dist_back,3);
  Serial.print(",");
  sensors_event_t event;
  accel.getEvent(&event);
  //* Display the results (acceleration is measured in m/s^2) */
  // Serial.print("\t");
  // Serial.print("XB:,");
  Serial.print(event.acceleration.x);
  Serial.print(",");
  // Serial.print("YB:,");
  Serial.print(event.acceleration.y);
  Serial.print(",");
  // Serial.print("ZB:,");
  Serial.print(event.acceleration.z);
  Serial.print(",");

  // // /* Get a new sensor event */
  sensors_event_t event1;
  accel1.getEvent(&event1);
  // /* Display the results (acceleration is measured in m/s^2) */
  // Serial.print("XF:,");
  Serial.print(event1.acceleration.x);
  Serial.print(",");
  // Serial.print("YF:,");
  Serial.print(event1.acceleration.y);
  Serial.print(",");
  // Serial.print("ZF:,");
  Serial.print(event1.acceleration.z);
  Serial.print(",");
  // Serial.print("time:,"); 
  Serial.println(micros());
  // back, front, loadcell, strain gauge
  //  first voltage then converted data
}





// micros, bottom_load_cell, X,X, suspension_load_cell , X, Suspension_LVDT