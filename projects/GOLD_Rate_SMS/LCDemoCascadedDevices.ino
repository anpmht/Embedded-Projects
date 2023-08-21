#include <SoftwareSerial.h>
#include "LedControl.h"
#include <Wire.h>
#include <RTClib.h>
#include <string.h>
#include <EEPROM.h>
SoftwareSerial sim800lSerial(2, 3);         // RX, TX - Replace with your desired pins
RTC_DS1307 rtc;                             // create a clock object
LedControl lc = LedControl(12, 11, 10, 6);  // create a ledcontrol object
LedControl lc1 = LedControl(7, 6, 5, 6);    // create a ledcontrol object

String Response = "";
uint32_t priceData[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
uint32_t dateData[2] = { 0, 0 };
int delaytime = 5;
int time = 1111;

void setup() {
  Serial.begin(9600);
  sim800lSerial.begin(9600);
  Serial.println("program started");
  setupLed();
  setupLed1();
  setupTimer();
  Response = readStringFromEEPROM(0);
  seperateLine(Response);

  sendCommand("AT");
  delay(1);
  readData();
  delay(1000);

  // Set the module in text mode
  sendCommand("AT+CMGF=1");
  delay(1);
  readData();
  delay(1000);

  // Set the preferred message storage
  sendCommand("AT+CPMS=\"SM\",\"SM\",\"SM\"");
  delay(1);
  readData();
  delay(1000);

  // sendCommand("AT+CMGD=1,4");
  // delay(1);
  // readData();
  // delay(1000);
}
void loop() {
  getTime();
  getGSM();
  print(dateData[0], 0);
  print1(dateData[1], 0);
  for (int i = 0; i < 5; i++) {
    print(priceData[i], i + 1);
  }
  for (int i = 5; i < 10; i++) {
    print1(priceData[i], i - 4);
  }
  delay(1000);
}

void sendCommand(String command) {
  sim800lSerial.println(command);
}

void getGSM() {

  // sendCommand("AT+CMGL=\"ALL\"");
  sendCommand("AT+CMGL=\"REC UNREAD\"");
  delay(1);
  readData();
  delay(500);
}

void readData() {
  // Check for incoming data from SIM800L
  while (sim800lSerial.available()) {
    Response = sim800lSerial.readString();
    Serial.println(Response);
  }
  char* token;
  char* remainder = const_cast<char*>(Response.c_str());  // Convert String to char*
  // Loop through the multiline string and extract individual strings
  while ((token = strtok_r(remainder, "\n", &remainder)) != NULL) {
    String line = String(token);
    line.replace("\t", "");
    // Serial.println(line);
    if (line.length() > 40) {
      if (line[0] == '1') {
        Serial.println("message found");
        Serial.println(line);

        for (int i = 0; i < 200; i++) {
          EEPROM.write(i, 0);
        }
        writeStringToEEPROM(0, line);

        seperateLine(line);

        sendCommand("AT+CMGD=1,4");
        delay(1);
        readData();
        delay(1000);
      }
    }
  }
}

void seperateLine(String line) {
  char* token1;
  char* remainder1 = const_cast<char*>(line.c_str());  // Convert String to char*
  // Loop through the multiline string and extract individual strings
  int a = 0;
  while ((token1 = strtok_r(remainder1, ",", &remainder1)) != NULL) {
    String value = String(token1);
    priceData[a] = value.toInt();
    Serial.println(value);
    a++;
  }
}
































void setupTimer() {
  if (!rtc.begin()) {
    while (1) delay(10);
  }

  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void setupLed() {
  int devices = lc.getDeviceCount();
  for (int address = 0; address < devices; address++) {
    lc.shutdown(address, false);
    lc.setIntensity(address, 8);
    lc.clearDisplay(address);
  }
}
void setupLed1() {
  int devices = lc1.getDeviceCount();
  for (int address = 0; address < devices; address++) {
    lc1.shutdown(address, false);
    lc1.setIntensity(address, 8);
    lc1.clearDisplay(address);
  }
}

int countDigit(long data) {
  if (data < 10) {
    return 1;
  } else if (data < 100) {
    return 2;
  } else if (data < 1000) {
    return 3;
  } else if (data < 10000) {
    return 4;
  } else if (data < 100000) {
    return 5;
  } else if (data < 1000000) {
    return 6;
  } else if (data < 10000000) {
    return 7;
  } else if (data < 100000000) {
    return 8;
  }
}

int SplitNo(int data, int size) {
  int digit[size];
  //defining a loop for splitting the number
  for (int i = 0; i < size; i++) {
    digit[i] = data % 10;
    data = data / 10;
  }
  return digit;
}

void print(long data, int Row) {
  int devices = lc.getDeviceCount();
  // Serial.println(devices);
  int Col = countDigit(data);
  // Serial.println(Col);
  int digit[Col];

  //defining a loop for splitting the number
  for (int i = 0; i < Col; i++) {
    digit[i] = data % 10;
    // Serial.println(digit[i]);
    data = data / 10;
  }
  // Serial.println("...");

  /////////////////////////////////
  int col_order[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
  if (Row == 1 || Row == 2 || Row == 3) {
    col_order[3] = 4;
    col_order[4] = 3;
  }

  //////////////////////////////////
  int row = Row;
  for (int col = 0; col < Col; col++) {
    for (int address = 0; address < devices; address++) {
      delay(delaytime);
      if (row == 0 & (col == 0 || col == 1)) {
        // Serial.println("problem row");
        if (digit[col] == 1) {
          lc.setLed(0, col, 5, true);
          lc.setLed(0, col, 6, true);
        } else if (digit[col] == 3) {
          lc.setChar(row, col_order[col], 'E', false);
        } else if (digit[col] == 4) {
          lc.setLed(0, col, 5, true);
          lc.setLed(0, col, 6, true);
          lc.setLed(0, col, 3, true);
          lc.setLed(0, col, 7, true);
        } else if (digit[col] == 6) {
          lc.setChar(row, col_order[col], 9, false);
        } else if (digit[col] == 7) {
          lc.setChar(row, col_order[col], 'L', false);
        } else if (digit[col] == 9) {
          lc.setChar(row, col_order[col], 6, false);
        } else {
          lc.setChar(row, col_order[col], digit[col], false);
        }
      } else {
        lc.setChar(row, col_order[col], digit[col], false);
      }
      lc.setLed(0, 1, 0, true);
      lc.setLed(0, 2, 0, true);
    }
  }
}
void print1(long data, int Row) {
  int devices = lc1.getDeviceCount();
  // Serial.println(devices);
  int Col = countDigit(data);
  // Serial.println(Col);
  int digit[Col];

  //defining a loop for splitting the number
  for (int i = 0; i < Col; i++) {
    digit[i] = data % 10;
    // Serial.println(digit[i]);
    data = data / 10;
  }
  // Serial.println("...");

  /////////////////////////////////
  int col_order[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
  if (Row == 1 || Row == 2 || Row == 3) {
    col_order[3] = 4;
    col_order[4] = 3;
  }

  //////////////////////////////////
  int row = Row;
  for (int col = 0; col < Col; col++) {
    for (int address = 0; address < devices; address++) {
      delay(delaytime);
      // if (row == 0 & (col == 0 || col == 1)) {
      //   // Serial.println("problem row");
      //   if (digit[col] == 1) {
      //     lc1.setLed(0, col, 5, true);
      //     lc1.setLed(0, col, 6, true);
      //   } else if (digit[col] == 3) {
      //     lc1.setChar(row, col_order[col], 'E', false);
      //   } else if (digit[col] == 4) {
      //     lc1.setLed(0, col, 5, true);
      //     lc1.setLed(0, col, 6, true);
      //     lc1.setLed(0, col, 3, true);
      //     lc1.setLed(0, col, 7, true);
      //   } else if (digit[col] == 6) {
      //     lc1.setChar(row, col_order[col], 9, false);
      //   } else if (digit[col] == 7) {
      //     lc1.setChar(row, col_order[col], 'L', false);
      //   } else if (digit[col] == 9) {
      //     lc1.setChar(row, col_order[col], 6, false);
      //   } else {
      //     lc1.setChar(row, col_order[col], digit[col], false);
      //   }
      // } else {
      lc1.setChar(row, col_order[col], digit[col], false);
    }
  }
  lc1.setLed(0, 4, 0, true);
  lc1.setLed(0, 2, 0, true);
  lc1.setLed(0, 0, 0, true);
}

// print one
void one(int row, int col) {
  lc.setLed(1, row, col, true);
}
// print three
void three(int row, int col) {
  lc.setLed(1, row, col, true);
}

void getTime() {
  DateTime now = rtc.now();
  char buf1[] = "hhmm";
  String time = now.toString(buf1);
  char buf2[] = "YYMMDD";
  String date = now.toString(buf2);
  dateData[0] = time.toInt();
  if (date.toInt() > 230000) { dateData[1] = date.toInt(); }
}

void writeStringToEEPROM(int addrOffset, const String& strToWrite) {
  byte len = strToWrite.length();
  EEPROM.write(addrOffset, len);

  for (int i = 0; i < len; i++) {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
}

String readStringFromEEPROM(int addrOffset) {
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];

  for (int i = 0; i < newStrLen; i++) {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\ 0';  // !!! NOTE !!! Remove the space between the slash "/" and "0" (I've added a space because otherwise there is a display bug)

  return String(data);
}