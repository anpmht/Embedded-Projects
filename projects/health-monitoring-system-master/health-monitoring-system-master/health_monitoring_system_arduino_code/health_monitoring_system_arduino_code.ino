const int groundpin = A4;             // analog input pin 4 -- ground
const int powerpin = A5;              // analog input pin 5 -- voltage
const int xpin = A2;                  // x-axis of the accelerometer
const int ypin = A3;                  // y-axis
long p = 5000;
long n = 0;
int v = 1500;

int ut =0;
int lt =0;
int sensorValue = 0;         // the sensor value
int sensorMin = 1023;        // minimum sensor value
int sensorMax = 0;  
long measurementStartTime = 0;
int beats = 0;
byte sensorPin1 = A1;
int currentSensorValue;
boolean counted = false;


void setup() {
  // initialize the serial communications:
  Serial.begin(115200);

  // Provide ground and power by using the analog inputs as normal digital pins.
  // This makes it possible to directly connect the breakout board to the
  // Arduino. If you use the normal 5V and GND pins on the Arduino,
  // you can remove these lines.
pinMode(groundpin, OUTPUT);
  pinMode(powerpin, OUTPUT);
  pinMode(A1,INPUT);
    
     pinMode(A3,INPUT);
     pinMode(A2,INPUT);
     pinMode(A0,INPUT);
  digitalWrite(groundpin, LOW);
  digitalWrite(powerpin, HIGH);


pinMode(sensorPin1, INPUT);
  while (millis() < 5000) {
    sensorValue = analogRead(sensorPin1);

    // record the maximum sensor value
    if (sensorValue > sensorMax) {
      sensorMax = sensorValue;
    }

    // record the minimum sensor value
    if (sensorValue < sensorMin) {
      sensorMin = sensorValue;
    }
  }
  ut = sensorMax - sensorMax * 0.1;
  lt = sensorMax - sensorMax * 0.2;
measurementStartTime = 5000;
}
#include <math.h>
 
int sensorPin = A0; // select the input pin for the potentiometer
 
double Thermistor(int RawADC) {
  double Temp;
  Temp = log(10000.0*((1024.0/RawADC-1))); 
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
  Temp = Temp - 273.15;            // Convert Kelvin to Celcius
   Temp = (Temp * 9.0)/ 5.0 + 32.0 ; // Convert Celcius to Fahrenheit
   return Temp;
}

void loop() {
  float temp;
  int i;
  i = temp;
int a= analogRead(xpin);
int b= analogRead(ypin);





if(a>=450 || b>=450)
{
  v= 500;
  }
  

int readVal=analogRead(sensorPin);
 temp =  Thermistor(readVal);
 
 // display tempature
 float k = (temp/10)+0.1;


 if ((millis() - measurementStartTime > 30000) && (beats > 0))  //time is up
  {
    
    beats = beats * 2   ;
    Serial.println(beats);
    measurementStartTime = millis();
    beats = 0;
  }
  currentSensorValue = analogRead(sensorPin1);
  if (currentSensorValue > ut && counted == false)
  {
    beats++;
    counted = true;  
  }
  else if (currentSensorValue < lt)
  {
    counted = false; 
  }

  n = millis();
if( n == p)
{
  p = p + 5000;
  Serial.println(v);
  Serial.println(k);
  v = 1500;
  }

}
