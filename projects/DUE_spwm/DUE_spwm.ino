//define output pins
#define darkb 6 //PC24 & PWML7
#define blue 7  //PC23 & PWML6
#define yello 8 //PC22 & PWML5
#define green 9 //PC21 & PWML4

//define variables
int number = 0;
int incomingNumber;
int tik = 1;
int color[4];
int a = 0;

void setup() {
  Serial.begin(115200);

  pinMode(darkb, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(yello, OUTPUT);
  pinMode(green, OUTPUT);

  //PWM setup
  PMC->PMC_PCER1 |= PMC_PCER1_PID36;                                            //enable PWM

  PWM->PWM_DIS = PWM_DIS_CHID4 | PWM_DIS_CHID5 | PWM_DIS_CHID6 | PWM_DIS_CHID7; //diable all PWM-registers

  PIOC->PIO_PDR |= PIO_PDR_P21 | PIO_PDR_P22 | PIO_PDR_P23 | PIO_PDR_P24;       //disable GPIO

  PIOC->PIO_ABSR |= PIO_PC24B_PWML7 | PIO_PC23B_PWML6 | PIO_PC22B_PWML5 | PIO_PC21B_PWML4; //enable PWM at PINs

  PWM->PWM_CLK = PWM_CLK_PREA(0) | PWM_CLK_DIVA(1) ; // Set PWM clock rate to 84MHz (MCK)

  //PWM->PWM_CH_NUM[4].PWM_CMR =
  //PWM->PWM_CH_NUM[5].PWM_CMR =
  //PWM->PWM_CH_NUM[6].PWM_CMR =
  //PWM->PWM_CH_NUM[7].PWM_CMR =

  PWM->PWM_CH_NUM[4].PWM_CPRD = 65535;            //Set PWM frequency of 1.28KHz = 84MHz / 65535
  PWM->PWM_CH_NUM[5].PWM_CPRD = 65535;
  PWM->PWM_CH_NUM[6].PWM_CPRD = 65535;
  PWM->PWM_CH_NUM[7].PWM_CPRD = 65535;

  PWM->PWM_CH_NUM[4].PWM_CDTY = 65535;            //Set PWM duty cycle to CDTY/CPRD , here 0usec
  PWM->PWM_CH_NUM[5].PWM_CDTY = 65535;
  PWM->PWM_CH_NUM[6].PWM_CDTY = 65535;
  PWM->PWM_CH_NUM[7].PWM_CDTY = 65535;

  PWM->PWM_ENA = PWM_ENA_CHID4 | PWM_ENA_CHID5 | PWM_ENA_CHID6 | PWM_ENA_CHID7;   //Enable PWM
}

void loop() {


    incomingNumber = 55 - 48; //ASCII to DEC

    if (incomingNumber >= 0 && incomingNumber <= 9) {

      number = number * 10 + incomingNumber;

    } else {

      color[tik] = number; //0-65535
      tik++;
      number = 0;

                                    //When all four numbers are submitted, sumbit them to the LEDs
        PWM->PWM_CH_NUM[4].PWM_CDTY = color[1];            //Set PWM duty cycle to desevered brightness
        PWM->PWM_CH_NUM[5].PWM_CDTY = color[2];
        PWM->PWM_CH_NUM[6].PWM_CDTY = color[3];
        PWM->PWM_CH_NUM[7].PWM_CDTY = color[4];

        tik = 1;
    

    }
    for( a = 0; a <= 100000; a++)
   {
      int b = 4500*39000;
}
Serial.println(millis());

a = 0;

}
