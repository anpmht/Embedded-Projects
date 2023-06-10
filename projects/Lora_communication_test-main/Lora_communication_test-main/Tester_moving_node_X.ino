#include <LoRa.h>
//#include <string.h>
#include <Wire.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
int SF = 7;
int x = 0;
int n = 0;
int m = 0;
int loss = 0;
int pq = 0;
int k = 0;
int  s = 0;
String keyData;
String Message = "";
char splitStrings[10][10];
String FM = "";
String SM = "";
String TM = "";
long rsssi = 0;
long snnr = 0;
bool Listen = false;
const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', ' '}
};
byte rowPins[ROWS] = {A3, A2, A1, A0}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4, 3, 1, 0}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup() {
  // put your setup code here, to run once:
  // put your setup code here, to run once:
  //Serial.begin(9600);
  // while (!Serial);
  if (!LoRa.begin(868E6)) {

    while (1);
  }
  LoRa.setSpreadingFactor(SF);
  lcd.begin();
  lcd.setCursor(0, 0);
  lcd.print("initiating.....");
  lcd.clear();
}




void loop() {

  readKeypad();
  if(Listen == true)
  {
    readData();
    }
}




void readKeypad()
{
  char key = keypad.getKey();
  if (key) {
    if (key != '*') {
      keyData += key;
    }
    lcd.setCursor(0, 0);
    lcd.print("Pkt ");
    lcd.setCursor(4, 0);
    lcd.print(keyData);
  }


  if (key == '*') {
    lcd.setCursor(0, 1);
    lcd.print("request sent");

    int n = keyData.length();
    char char_array[n + 1];
    strcpy(char_array, keyData.c_str());
    int i = 0;
    char *p = strtok (char_array, " ");
    char *array[20];
    while (p != NULL)
    {
      array[i++] = p;
      p = strtok (NULL, " ");
    }
    FM = String(array[0]);
    SM = String(array[1]);
    x = SM.toInt();
    LoRa.setSpreadingFactor(12);
    LoRa.beginPacket();
    LoRa.print(keyData);
    LoRa.endPacket();
    LoRa.setSpreadingFactor(FM.toInt());
    Listen = true;    
    keyData = "";
    lcd.clear();
  }

  if (key == '#') {
    lcd.setCursor(0, 1);
    lcd.clear();
    lcd.print("Data cleared");
    delay(500);
    lcd.clear();
    keyData = "";
  }
}





void readData()
{
  // put your main code here, to run repeatedly:
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // read packet
    while (LoRa.available()) {
      Message = LoRa.readString() ;
    }
    int n = Message.length();                 //Read who sent the data
    char char_array[n + 1];
    strcpy(char_array, Message.c_str());
    int i = 0;
    char *p = strtok (char_array, " ");
    char *array[20];
    while (p != NULL)
    {
      array[i++] = p;
      p = strtok (NULL, " ");
    }
    FM = String(array[0]);
    SM = String(array[1]);
    TM = String(array[2]);
    k = TM.toInt();
    pq = 1;
    //Serial.println(Message);
    lcd.setCursor(13, 0);
    lcd.print(SM);
    if (k == 22) {
      m = m + 1;
      rsssi = rsssi + LoRa.packetRssi();
      snnr = snnr + LoRa.packetSnr();
    lcd.setCursor(4, 0);
    lcd.print("SF-");
    lcd.setCursor(9, 0);
    lcd.print(FM.toInt());
    lcd.setCursor(0, 1);
    lcd.print("Packet-");
    lcd.setCursor(8, 1);
    lcd.print(Message);
    }
    if (k == 33) {
      Listen = false;
      //Serial.print("spreading factor changed to ");
      //Serial.println(SF);
      loss = x - m;
      int r = rsssi / m;
      int n = snnr / m;
      m = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Loss-");
      lcd.setCursor(5, 0);
      lcd.print(loss);
      lcd.setCursor(8, 0);
      lcd.print("SF");
      lcd.setCursor(10, 0);
      lcd.print(FM.toInt());
      lcd.setCursor(0, 1);
      lcd.print("Rssi-");
      lcd.setCursor(5, 1);
      lcd.print(r);
      lcd.setCursor(9, 1);
      lcd.print("Snr-");
      lcd.setCursor(14, 1);
      lcd.print(n);

      //Serial.print("loss is ");
      //Serial.println(loss);
      //Serial.print("RSSI is ");
      //Serial.println(r);
      //Serial.print("SNR is ");
      //Serial.println(n);

      rsssi = 0;
      snnr = 0;
     
    }
  }
}
