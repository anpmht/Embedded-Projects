int decimalPrecision = 2;                   // decimal places for all values shown in LED Display & Serial Monitor

/* 1- AC Current Measurement */

int currentAnalogInputPin = A0;             // Which pin to measure Current Value (A0 is reserved for LCD Display Shield Button function)
int calibrationPin = A1;                    // Which pin to calibrate offset middle value
float manualOffset = 0.00;                  // Key in value to manually offset the initial value
float mVperAmpValue = 12.5;                 // If using "Hall-Effect" Current Transformer, key in value using this formula: mVperAmp = maximum voltage range (in milli volt) / current rating of CT
// For example, a 20A Hall-Effect Current Transformer rated at 20A, 2.5V +/- 0.625V, mVperAmp will be 625 mV / 20A = 31.25mV/A
// For example, a 50A Hall-Effect Current Transformer rated at 50A, 2.5V +/- 0.625V, mVperAmp will be 625 mV / 50A = 12.5 mV/A
float supplyVoltage = 5000;                 // Analog input pin maximum supply voltage, Arduino Uno or Mega is 5000mV while Arduino Nano or Node MCU is 3300mV
float offsetSampleRead = 0;                 /* to read the value of a sample for offset purpose later */
float currentSampleRead  = 0;               /* to read the value of a sample including currentOffset1 value*/
float currentLastSample  = 0;               /* to count time for each sample. Technically 1 milli second 1 sample is taken */
float currentSampleSum   = 0;               /* accumulation of sample readings */
float currentSampleCount = 0;               /* to count number of sample. */
float currentMean ;                         /* to calculate the average value from all samples, in analog values*/
float RMSCurrentMean ;                      /* square roof of currentMean, in analog values */
float FinalRMSCurrent ;                     /* the final RMS current reading*/

/* 2 - LCD Display  */

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
unsigned long startMicrosLCD;                   /* start counting time for LCD Display */
unsigned long currentMicrosLCD;                 /* current counting time for LCD Display */
const unsigned long periodLCD = 1000000;        // refresh every X seconds (in seconds) in LED Display. Default 1000000 = 1 second



void setup()                                              /*codes to run once */

{
  lcd.begin();
  lcd.backlight();                             /* Set LCD to start with upper left corner of display*/
  startMicrosLCD = micros();                        /* Start counting time for LCD display*/

}


void loop()                                                                                                   /*codes to run again and again */
{

  /* 1- AC & DC Current Measurement */

  if (micros() >= currentLastSample + 200)                                                              /* every 0.2 milli second taking 1 reading */
  {
    currentSampleRead = analogRead(currentAnalogInputPin) - analogRead(calibrationPin);                /* read the sample value including offset value*/
    currentSampleSum = currentSampleSum + sq(currentSampleRead) ;                                      /* accumulate total analog values for each sample readings*/
    currentSampleCount = currentSampleCount + 1;                                                       /* to count and move on to the next following count */
    currentLastSample = micros();                                                                      /* to reset the time again so that next cycle can start again*/
  }

  if (currentSampleCount == 4000)                                                                       /* after 4000 count or 800 milli seconds (0.8 second), do this following codes*/
  {
    currentMean = currentSampleSum / currentSampleCount;                                              /* average accumulated analog values*/
    RMSCurrentMean = sqrt(currentMean);                                                               /* square root of the average value*/
    FinalRMSCurrent = (((RMSCurrentMean / 1023) * supplyVoltage) / mVperAmpValue) - manualOffset;     /* calculate the final RMS current*/
    if (FinalRMSCurrent <= (625 / mVperAmpValue / 100))                                               /* if the current detected is less than or up to 1%, set current value to 0A*/
    {
      FinalRMSCurrent = 0;
    }
//    Serial.print(" The Current RMS value is: ");
//    Serial.print(FinalRMSCurrent, decimalPrecision);
//    Serial.println(" A ");
    currentSampleSum = 0;                                                                             /* to reset accumulate sample values for the next cycle */
    currentSampleCount = 0;                                                                           /* to reset number of sample for the next cycle */
  }

  /* 2 - LCD Display  */

  currentMicrosLCD = micros();                                                                          /* Set counting time for LCD Display*/
  if (currentMicrosLCD - startMicrosLCD >= periodLCD)                                                   /* for every x seconds, run the codes below*/
  {
    lcd.setCursor(0, 0);                                                                              /* Set cursor to first colum 0 and second row 1  */
    lcd.print("I=");
    lcd.print(FinalRMSCurrent, decimalPrecision);                                                     /* display current value in LCD in first row  */
    lcd.print("A                  ");
    lcd.setCursor(0, 1);
    lcd.print("                   ");                                                                 /* display nothing in LCD in second row */
    startMicrosLCD = currentMicrosLCD ;                                                               /* Set the starting point again for next counting time */
  }




}
