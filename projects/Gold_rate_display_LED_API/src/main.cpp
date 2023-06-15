#include <SoftwareSerial.h>
#include "LedControl.h"
#include <Wire.h>
#include <DS1307RTC.h>

SoftwareSerial myGsm(3, 2);
LedControl lc = LedControl(12, 11, 10, 6);
LedControl lc1 = LedControl(7, 6, 5, 6);

long price[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
bool gettime = false;
bool start = false;
bool getapi = true;
int delaytime = 1;
int time = 1111;
int ptime = 1111;
long timeout = 0;
bool isdata = false;
String json = "";

void printSerialData()
{
  for (int i = 0; i < 10; i++)
  {
    while (myGsm.available() != 0)
      Serial.write(myGsm.read());
  }
  delay(1);
}

void getAPI()
{
  myGsm.println("AT + CGATT = 1");
  delay(200);

  myGsm.println("AT + SAPBR = 3, 1,\"CONTYPE\",\"GPRS\""); // setting the SAPBR,connection type is GPRS
  delay(1000);

  myGsm.println("AT + SAPBR = 3, 1,\"APN\",\"\""); // setting the APN,2nd parameter empty works for all networks
  delay(5000);

  myGsm.println("AT + SAPBR = 1, 1");
  delay(10000);

  myGsm.println("AT + HTTPINIT");
  delay(2000);
  printSerialData();

  myGsm.println("AT+HTTPPARA=\"CID\",1");
  delay(2000);
  printSerialData();

  myGsm.println("AT+HTTPPARA=\"URL\",\"http://golddata.tiurl.link/api/data\"");
  delay(8000);
  printSerialData();

  myGsm.println("AT+HTTPACTION=0");
  delay(8000);
  printSerialData();

  myGsm.println("AT+HTTPREAD");
}

void setupLed()
{
  int devices = lc.getDeviceCount();
  for (int address = 0; address < devices; address++)
  {
    lc.shutdown(address, false);
    lc.setIntensity(address, 8);
    lc.clearDisplay(address);
  }
}

void setupLed1()
{
  int devices = lc1.getDeviceCount();
  for (int address = 0; address < devices; address++)
  {
    lc1.shutdown(address, false);
    lc1.setIntensity(address, 8);
    lc1.clearDisplay(address);
  }
}

int countDigit(long data)
{
  if (data < 10)
  {
    return 1;
  }
  else if (data < 100)
  {
    return 2;
  }
  else if (data < 1000)
  {
    return 3;
  }
  else if (data < 10000)
  {
    return 4;
  }
  else if (data < 100000)
  {
    return 5;
  }
  else if (data < 1000000)
  {
    return 6;
  }
  else if (data < 10000000)
  {
    return 7;
  }
  else if (data < 100000000)
  {
    return 8;
  }
  else
  {
    return 0;
  }
}

void print(long data, int Row)
{
  int devices = lc.getDeviceCount();
  int Col = countDigit(data);
  int digit[Col];

  for (int i = 0; i < Col; i++)
  {
    digit[i] = data % 10;
    data = data / 10;
  }

  int col_order[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  if (Row == 1 || Row == 2 || Row == 3)
  {
    col_order[3] = 4;
    col_order[4] = 3;
  }

  int row = Row;
  for (int col = 0; col < Col; col++)
  {
    for (int address = 0; address < devices; address++)
    {
      delay(delaytime);
      if ((row == 0) & (col == 0 || col == 1))
      {
        if (digit[col] == 1)
        {
          for (int d = 0; d < 8; d++)
          {
            lc.setLed(0, col, d, false);
          }
          lc.setLed(0, col, 5, true);
          lc.setLed(0, col, 6, true);
        }
        else if (digit[col] == 3)
        {
          lc.setChar(row, col_order[col], 'E', false);
        }
        else if (digit[col] == 4)
        {
          for (int d = 0; d < 8; d++)
          {
            lc.setLed(0, col, d, false);
          }
          lc.setLed(0, col, 5, true);
          lc.setLed(0, col, 6, true);
          lc.setLed(0, col, 3, true);
          lc.setLed(0, col, 7, true);
        }
        else if (digit[col] == 6)
        {
          lc.setChar(row, col_order[col], 9, false);
        }
        else if (digit[col] == 7)
        {
          lc.setChar(row, col_order[col], 'L', false);
        }
        else if (digit[col] == 9)
        {
          lc.setChar(row, col_order[col], 6, false);
        }
        else
        {
          lc.setChar(row, col_order[col], digit[col], false);
        }
      }
      else
      {
        lc.setChar(row, col_order[col], digit[col], false);
      }
      lc.setLed(0, 1, 0, true);
      lc.setLed(0, 2, 0, true);
    }
  }
}

void print1(long data, int Row)
{
  int devices = lc1.getDeviceCount();
  int Col = countDigit(data);
  int digit[Col];

  // defining a loop for splitting the number
  for (int i = 0; i < Col; i++)
  {
    digit[i] = data % 10;
    data = data / 10;
  }

  /////////////////////////////////
  int col_order[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  if (Row == 1 || Row == 2 || Row == 3)
  {
    col_order[3] = 4;
    col_order[4] = 3;
  }

  //////////////////////////////////
  int row = Row;
  for (int col = 0; col < Col; col++)
  {
    for (int address = 0; address < devices; address++)
    {
      delay(delaytime);
      lc1.setChar(row, col_order[col], digit[col], false);
    }
  }
  lc1.setLed(0, 4, 0, true);
  lc1.setLed(0, 2, 0, true);
  lc1.setLed(0, 0, 0, true);
  for (int d = 0; d < 8; d++)
  {
    lc1.setLed(0, 5, d, true);
  }
}

void one(int row, int col)
{
  lc.setLed(1, row, col, true);
}

void three(int row, int col)
{
  lc.setLed(1, row, col, true);
}

void getTime()
{
  tmElements_t tm;
  if (RTC.read(tm))
  {
    int h = tm.Hour * 100;
    time = h + tm.Minute;
    Serial.println(time);
  }
}

void setup()
{
  setupLed();
  setupLed1();
  delay(30000);

  myGsm.begin(9600);
  Serial.begin(9600);
  delay(500);
}

void loop()
{
  if (getapi == true)
  {
    getAPI();
    getapi = false;
    timeout = millis();
  }
  if (myGsm.available())
  {
    char data = myGsm.read();
    if (data == '[')
    {
      start = true;
      json = "";
    }
    if ((start == true) && (data != '[') && (data != ']'))
    {
      json += data;
    }
    else if (data == ']')
    {
      json += ",";
      start = false;
      Serial.println(json);
      int b = json.length();
      String tempdata = "";
      int d = 0;
      for (int c = 0; c < b; c++)
      {
        if (json[c] != ',')
        {
          tempdata += json[c];
        }
        else
        {
          price[d] = tempdata.toInt();
          tempdata = "";
          d++;
        }
      }
      ptime = price[1];
      for (int i = 0; i < 10; i++)
      {
        Serial.println(price[i]);
      }
      print1(price[0], 0);
      for (int i = 0; i < 5; i++)
      {
        print(price[i+1], i + 1);
      }
      for (int i = 5; i < 10; i++)
      {
        print1(price[i+1], i - 4);
      }
      if (price[9] > 10)
      {
        getapi = false;
        gettime = true;
        isdata = true;
        Serial.println("right data setting getapi false and time true");
      }
      else{
        isdata = false;
        Serial.println("wrong data");
      }
      // myGsm.println("AT+HTTPTERM");
    }
    if (((millis() - timeout) > 60000) && (isdata == false))
    {
      gettime = true;
      getapi = true;
      Serial.println("get time and get API true because of timeout or wrong data");
    }
  }
  if (gettime == true)
  {
    getTime();
    print(time, 0);
    if (time - ptime > 30)
    {
      ptime = time;
      getapi = true;
      gettime = false;
      Serial.println("getting data cause its 30 minutes");
    }
    if (getapi == true ){
      gettime = false;
      Serial.println("setting gettime false caude getapi is true");
    }
    delay(10000);
  }
}