#include <math.h>
#include <Wire.h>

int PWM_FREQUENCY = 1000; // this variable is used to define the time period 
int PWM_CHANNEL = 0; // this variable is used to select the channel number
int PWM_RESOUTION = 10; // this will define the resolution of the signal which is 8 in this case
int GPIOPIN = 15 ; // GPIO to which we want to attach this channel signal

float setpoint = 24;
long  I = 0;
float Ki = 0.01;
float v1 = 0;
float v2 = 0;
float power = 0;
float resistance  = 0;
int pwm_1 = 0;
int pwm_2 = 0;
int PWM = 0;

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
  power  = setpoint * setpoint / resistance;
  pwm_1 = equation(resistance);

  error = setpoint - v2;
  I = I + error;
  pwm_2 = Ki * I;
  PWM = pwm_1 + pwm_2;
  
  ledcWrite(channel, PWM);
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
