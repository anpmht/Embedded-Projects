SoftwareSerial s(D6, D5);
float data;
void setup() {
  Serial.begin(9600);
}

void loop() {
  if (s.available() > 0)
  {
    data = s.read();
    Serial.println(data);
  }
}
