int PWM_FREQUENCY = 5000; // this variable is used to define the time period
int PWM_CHANNEL = 0; // this variable is used to select the channel number
int PWM_CHANNEL_1 = 1; // this variable is used to select the channel number
int PWM_RESOUTION = 12; // this will define the resolution of the signal 
int GPIOPIN = 15 ; // GPIO to which we want to attach this channel signal
int GPIOPIN_1 = 4 ; // GPIO to which we want to attach this channel signal
int dutyCycle = 127; // it will define the width of signal or also the one time
int dutyCycle1 = 127; // it will define the width of signal or also the one time

void setup() {
  pinMode(13,INPUT);
  pinMode(14,OUTPUT);
  pinMode(27,OUTPUT);
  //pinMode(12,INPUT);
  ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOUTION);
  ledcAttachPin(GPIOPIN, PWM_CHANNEL);
  ledcSetup(PWM_CHANNEL_1, PWM_FREQUENCY, PWM_RESOUTION);
  ledcAttachPin(GPIOPIN_1, PWM_CHANNEL_1);
}

void loop() {
  dutyCycle = analogRead(13);
  dutyCycle1 = analogRead(12);
  ledcWrite(PWM_CHANNEL, dutyCycle);
  ledcWrite(PWM_CHANNEL_1, dutyCycle1);
  delay(1);
}
