#include <SoftwareSerial.h>
#include <ArduinoJson.h>
SoftwareSerial myGsm(3, 2);

long price[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
bool get = false;
bool start = false;
StaticJsonDocument<400> doc;

String json = "";

void setup() {
  myGsm.begin(9600);
  Serial.begin(9600);
  delay(500);

  myGsm.println("AT + CGATT = 1");
  delay(200);
  // printSerialData();

  myGsm.println("AT + SAPBR = 3, 1,\"CONTYPE\",\"GPRS\"");  //setting the SAPBR,connection type is GPRS
  delay(1000);
  // printSerialData();

  myGsm.println("AT + SAPBR = 3, 1,\"APN\",\"\"");  //setting the APN,2nd parameter empty works for all networks
  delay(5000);
  // printSerialData();

  myGsm.println("AT + SAPBR = 1, 1");
  delay(10000);
  // printSerialData();

  // printSerialData();

  // delay(1000);
  // myGsm.println("AT+HTTPTERM");  // terminate HTTP service
  // printSerialData();
}

void loop() {
  if (get == false ) {
    getAPI();
    get = true;
  }
  if (myGsm.available()) {
    char data = myGsm.read();
    // Serial.write(data);
    if (data == '{') {
      start = true;
    }
    if (start == true) {
      json += data;
    }
    if (data == '}') {
      start = false;
      Serial.println("");
      Serial.println("");
      Serial.println("");
      Serial.println("");
      Serial.println("");
      Serial.println("");
      Serial.println(json);

      DeserializationError error = deserializeJson(doc, json);
      const char* date = doc["nepali_date"];
      Serial.println(date);
      price[0] = doc["gold_10g_sell"];
      price[1] = doc["gold_10g_buy"];
      price[2] = doc["gold_1t_sell"];
      price[3] = doc["gold_1t_buy"];
      price[4] = doc["tejawigold_1t_sell"];
      price[5] = doc["tejawigold_1t_buy"];
      price[6] = doc["aslisilver10gsell"];
      price[7] = doc["aslisilver10gbuy"];
      price[8] = doc["aslisilver1tsell"];
      price[9] = doc["aslisilver1tbuy"];

      for (int i = 0; i < 10; i++) {
        Serial.println(price[i]);
      }
    }
  }
}

void printSerialData() {
  for (int i = 0; i < 10; i++) {
    while (myGsm.available() != 0)
      Serial.write(myGsm.read());
  }
  delay(1);
}

void getAPI() {
  myGsm.println("AT + HTTPINIT");  //init the HTTP request
  delay(2000);
  printSerialData();

  myGsm.println("AT+HTTPPARA=\"CID\",1");  //init the HTTP request
  delay(2000);
  printSerialData();

  myGsm.println("AT+HTTPPARA=\"URL\",\"http://golddata.tiurl.link/api/data\"");  // setting the httppara,
  //the second parameter is the website from where you want to access data
  delay(8000);
  printSerialData();

  myGsm.println("AT+HTTPACTION=0");  //submit the GET request
  delay(8000);                       //the delay is important if the return datas are very large, the time required longer.
  printSerialData();

  myGsm.println("AT+HTTPREAD");  // read the data from the website you access
}