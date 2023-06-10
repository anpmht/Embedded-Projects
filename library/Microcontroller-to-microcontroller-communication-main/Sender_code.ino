#include <SoftwareSerial.h>   // connect the 5 of esp to 5 of arduino
SoftwareSerial s(5, 6);              //connect the 6 of esp to 6 of arduino

void setup() {
  s.begin(9600);
}
void loop() {
  int data = 50;
  if (s.available() > 0)
  {
    s.write(data);
  }
}
