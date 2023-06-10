#include <math.h>
#include <Wire.h>

int PWM_FREQUENCY = 1000; // this variable is used to define the time period 
int PWM_CHANNEL = 0; // this variable is used to select the channel number
int PWM_RESOUTION = 10; // this will define the resolution of the signal which is 8 in this case
int GPIOPIN = 15 ; // GPIO to which we want to attach this channel signal
int dutyCycle = 127; // it will define the width of signal or also the one time

float v1 = 0;
float v2 = 0;
int ref_voltage = 24;
float power = 0;
float resistance  = 0;
int pwm_out = 0;

void setup() {                 
  pinMode(35, INPUT);
  pinMode(34, INPUT);
  pinMode(25, OUTPUT);
  Serial.begin(9600);
  ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOUTION);
  ledcAttachPin(GPIOPIN, PWM_CHANNEL);
}

void loop() {                  // main loop function
  v1 = analogRead(34);
  v2  = analogRead(35) * 9.894833;
  resistance = (2*(v2 - v1) / v1);
  resistance  = resistance/0.86;
  power  = ref_voltage * ref_voltage / resistance;
  pwm_out = equation(resistance);
  ledcWrite(channel, pwm_out);
  delay(10);
}

float equation(float resistance) {  // equation to calculate the PWM from fitted curve
  double a = 39680;
  double b = -0.1918;
  double c = 128.1;
  double d = - 0.006742;
  double e = 0;
  double f = 0;
  int pwm = a * exp(b * resistance)+ c* exp(d  * resistance);
  //int pwm = a*po(5,power)+b*po(4,power)+c*po(3,power)+d*po(2,power)+e*power+f;
  return pwm;
}

float po(int deg ,float  base)
{ float x = 1;
  for (int i = 0 ; i < deg ; i ++ )
  {
    x  = x  * base;
    }
    return x;
 }
