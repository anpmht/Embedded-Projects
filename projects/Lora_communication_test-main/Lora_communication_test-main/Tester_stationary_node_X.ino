#include <LoRa.h>
#include <Wire.h>
#define ss 5
#define rst 13
#define dio0 26
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
int SF = 7;
int sf = 0;
int n = 0;
String packet = "";
long rsssi = 0;
String Message = "";
String FM = "";
String SM = "";
bool Transmit = false;
long t = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  LoRa.setPins(ss, rst, dio0);
  while (!Serial);
  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setSpreadingFactor(SF);
  lcd.begin();
  lcd.setCursor(0, 0);
  lcd.print("initiating.....");
  delay(1000);
  lcd.clear();
}

void loop() {
  readData();
  if (Transmit == true)
  {
    transmit();
    Transmit = false;
  }
}

void transmit() {
  int gh = FM.toInt();
  LoRa.setSpreadingFactor(gh);
  for (n = 0; n < SM.toInt(); n++)
  { t= millis();
    packet = String(gh) + " " + String(n) + " " + String(22);
    LoRa.beginPacket();
    LoRa.print(packet);
    LoRa.endPacket();
    Serial.println(packet);
    delay(20);
    lcd.setCursor(4, 0);
    lcd.print("SF-");
    lcd.setCursor(9, 0);
    lcd.print(gh);
    lcd.setCursor(0, 1);
    lcd.print("Packet-");
    lcd.setCursor(8, 1);
    lcd.print(packet);
    Serial.print("transmitted ");
    Serial.print(n);
    Serial.print(" data in ");
    Serial.print(millis()- t-20);
    Serial.print(" millis");
  }
  delay(1000);
  lcd.clear();
  for (n = 0; n < 5; n++)
  {
    packet = String(gh) + " " + String(19) + " " + String(33);
    LoRa.beginPacket();
    LoRa.print(packet);
    LoRa.endPacket();
    Serial.println(packet);
    delay(20);
  }
  Transmit = false;
  LoRa.setSpreadingFactor(12);
}

void readData()
{ LoRa.setSpreadingFactor(12);
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
    if ((FM.toInt() > 6) && (FM.toInt() < 13))
    {
      Transmit = true;
    }
  }
}
