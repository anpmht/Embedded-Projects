#include <Arduino.h>
#include <SPI.h>
#include <encoder.h>

encoder Encoder;

void setup() { Serial.begin(115200); }

void loop() {
    Encoder.getRPM();  // Get data from encoder
}