#include "ThingSpeak.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "EmonLib.h"
#define ADC_INPUT 34  // current measurement pin
#define ADC_INPUT1 32 // voltage measurement pin

// Replace with your network credentials
const char* ssid     = "Microgrid_Lab";
const char* password = "Microgrid123";

// REPLACE with your Domain name and URL path or IP address with path
const char* serverName = "http://example.com/post-esp-data.php";

float HOME_VOLTAGE =  220;
EnergyMonitor emon1;
LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiClient  client;
unsigned long lastMeasurement = 0;
unsigned long timeFinishedSetup = 0;
unsigned long uploadtime = 0;
long energyWM = 0.00;
long energyWH = 0.00;
long power = 0.00;
float Voltage = 0.00;
float amps = 0.00;
void setup()
{
  analogReadResolution(10);
  lcd.begin();
  lcd.backlight();
  emon1.current(ADC_INPUT, 31);
  emon1.voltage(ADC_INPUT1, 410, 1.7);  // Voltage: input pin, calibration, phase_shift
  Connect_WiFi();
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  timeFinishedSetup = millis();
  uploadtime = millis();
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastMeasurement > 10000) {
    amps = emon1.calcIrms(5588) - 0.08; // Calculate Irms only
    emon1.calcVI(100, 4000);                   // Calculate all. No.of half wavelengths (crossings), time-out
    Voltage   = emon1.Vrms;              //extract Vrms into Variable
    lastMeasurement = millis();
    power = Voltage * amps;
    energyWM = energyWM + power / 6;
    energyWH = energyWM / 60;
    writeEnergyToDisplay(power,  amps, Voltage,  energyWH);
  }

  if (millis() - uploadtime > 30000) {
    upload_thingspeak();
    uploadtime = millis();
  }

}

void writeEnergyToDisplay(double watts, double amps, float voltage, double energyWH)
{
  lcd.setCursor(0, 0); // Start from column 3, first two are broken :/
  lcd.print(voltage);
  lcd.print("V ");
  lcd.print(amps);
  lcd.print("A");

  lcd.setCursor(0, 1); // Start from column 3, first two are broken :/
  lcd.print(watts);
  lcd.print("W ");
  lcd.print(energyWH);
  lcd.print("WH");

}

void Connect_WiFi() {
  WiFi.begin(ssid, password);
  lcd.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.print(".");
  }
  lcd.clear();
  lcd.print("Connected");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  delay(2000);
}

void WiFi_Upload() {
  //  if (WiFi.status() == WL_CONNECTED) {
  //    WiFiClient client;
  //    HTTPClient http;
  //    http.begin(client, serverName);
  //    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  //    String httpRequestData = "api_key=" + apiKeyValue + "&sensor=" + sensorName
  //                             + "&location=" + sensorLocation + "&value1=" + String(bme.readTemperature())
  //                             + "&value2=" + String(bme.readHumidity()) + "&value3=" + String(bme.readPressure() / 100.0F) + "";
  //    Serial.print("httpRequestData: ");
  // Serial.println(httpRequestData);
  //    int httpResponseCode = http.POST(httpRequestData);
  //    if (httpResponseCode > 0) {
  //      Serial.print("HTTP Response code: ");
  //      Serial.println(httpResponseCode);
  //    }
  //    else {
  //      Serial.print("Error code: ");
  //      Serial.println(httpResponseCode);
  //    }
  //    http.end();
  //  }
  //  else {
  //    Serial.println("WiFi Disconnected");
  //  }
  //  delay(30000);
}

void upload_thingspeak() {
  ThingSpeak.setField(1, Voltage);
  ThingSpeak.setField(2, amps);
  ThingSpeak.setField(3, power);
  ThingSpeak.setField(4, energyWH);
  int x = ThingSpeak.writeFields(1647730, "OWEFXKR7X2OHP9Z0");
  if (x == 200) {
    lcd.clear();
    lcd.print("update success");
  }
  else {
    lcd.clear();
    lcd.print("Update failed");
  }
}
