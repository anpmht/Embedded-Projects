//connect signal pin to A1 pin of arduino
//connect vcc of sensor to 5v pin of arduino 
//connect ground pin of sensor to ground pin of arduino
int ut =0;    // upper threshold value of the signal from sensor analog pin
int lt =0;    // lower threshold value
int sensorValue = 0;         // the data from the sensor
int sensorMin = 1023;        // minimum data coming from the sensor
int sensorMax = 0;           //max data coming from sensor
long measurementStartTime =0;
int beats = 0;
byte sensorPin1 = A1;
int currentSensorValue;
boolean counted = false;

void setup() 
{
  Serial.begin(9600);
  pinMode(sensorPin1, INPUT);
  while (millis() < 5000) {
    sensorValue = analogRead(sensorPin1);

    // record the maximum sensor value
    if (sensorValue > sensorMax) {
      sensorMax = sensorValue;
    }
    // auto callibrating the threshold for the user
    // record the minimum sensor value
    if (sensorValue < sensorMin) {
      sensorMin = sensorValue;
    }
  }
  ut = sensorMax - sensorMax * 0.20;
  lt = sensorMin + sensorMin * 0.25;
  Serial.println(ut);
  Serial.println(lt);
  measurementStartTime = 5000;
}

void loop()   // in this loop the sensor will count the pulse for six seconds and multiply it by 10 and shows as bpm
{ 
  int  p = millis() - measurementStartTime;
  if (p > 6000)  //time is up
  {
    Serial.print("Heart rate =  ");
    beats = beats * 10;
    Serial.println(beats);
    measurementStartTime = millis();
    beats = 0;
  }
  Serial.println(beats);
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
  //Serial.println(currentSensorValue);
}
