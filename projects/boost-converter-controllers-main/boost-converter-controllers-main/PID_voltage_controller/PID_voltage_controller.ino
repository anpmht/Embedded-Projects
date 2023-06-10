#include <math.h>
#include <Wire.h>

int PWM_FREQUENCY = 490; // this variable is used to define the time period
int PWM_CHANNEL = 0; // this variable is used to select the channel number
int PWM_RESOUTION = 10; // this will define the resolution of the signal which is 8 in this case
int GPIOPIN = 25 ; // GPIO to which we want to attach this channel signal
int PID = 0; // it will define the width of signal or also the one time

long P = 0;
long I = 0;
long D = 0;
float Kp = 4;
float Ki = 0;
float Kd = 0;
long Time = 0;
long pTime = 0;
long dt = 0;

void setup() {
  pinMode(34, INPUT);
  pinMode(25, OUTPUT);
  ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOUTION);
  ledcAttachPin(GPIOPIN, PWM_CHANNEL);
}

void loop() {
  int Voltage = analogRead(34);
  
  int error  = setPoint - Voltage;
  P = error;
  I = I + P;
  pTime = Time;
  dt = Time - pTime;
  Time = millis();
  D = P / dt;
  PID  = P * Kp + I * Ki + D * Kd
  
  ledcWrite(PWM_CHANNEL, PID);
  delay(10);
}
