#include <BluetoothSerial.h>
BluetoothSerial SerialBT;

int PWM_FREQUENCY = 490; // this variable is used to define the time period
int PWM_CHANNEL = 0; // this variable is used to select the channel number
int PWM_RESOUTION = 10; // this will define the resolution of the signal which is 8 in this case
int GPIOPIN = 25 ; // GPIO to which we want to attach this channel signal

void setup() {
  SerialBT.begin("ESP");
  ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOUTION);
}
void loop() {
  if (SerialBT.available()) {}
  pwm = SerialBT.parseInt();
  ledcWrite(PWM_CHANNEL, pwm);
}
