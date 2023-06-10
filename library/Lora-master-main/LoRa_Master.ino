#include <SPI.h>
#include <LoRa.h>
#include <string.h>

long int TIME = 0;
long int TIME1 = 0;
int voltage = 221;
int current  = 11;
int power = 111;
int energy = 1;
char splitStrings[10][10];
bool received = false;
bool sub = true;
int ID = 1;
String gateId = "0";
String sender_ID = "0";
String receiver_ID = "2";
String repeater_ID = "0";
String datatype = "0";
String packet = "";
String myId = "1";
String Message = "";
String query = "0";
int n = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  TIME = millis()-10000;
}



void loop() {
  if (ID > 5) {
        ID = 0;
      }
  packet = query + " " + myId + " " + ID + " " + repeater_ID ;//+ " " + voltage + " " + current + " " + power + " " + energy;
  TIME1 = millis();
  if (millis() - TIME > 5000)
  { Serial.print("request sent waiting for responce ");
    Serial.println(ID);
    sub = true;
    Serial.println(packet);
    LoRa.beginPacket();
    LoRa.print(packet);
    LoRa.endPacket();
    TIME = millis();
  }
  while (sub == true)
  {
    Listen();
    if ((millis() - TIME1) > 2000) {
      sub = false;
      n++;
      if (n >= 3)
      {
        ID++;
        n = 0;
        Serial.println("five data retreival attemt failed, moving to another node");
      }
    }
    if (received == true)    // if data is received than break the loop and reser the indicator
    {
      sub = false;
      received = false;
      ID++;
      n = 0;
    }
  }
}

void Listen() {
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
    datatype = String(array[0]);
    sender_ID = String(array[1]);       //Process and print the data
    receiver_ID = String(array[2]);       //Process and print the data
    repeater_ID = String(array[3]);
    if (datatype = "1")
    {
      received = true;
      datatype = "0";
      Serial.print("data received from ");
      Serial.println(sender_ID);
      Serial.println(Message);
      Message = "";
    }
  }
}
