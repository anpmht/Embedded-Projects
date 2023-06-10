const int trigpin = A2;
const int echopin  = A3;
long duration;
int distance;
void setup()
{
  pinMode(trigpin,OUTPUT);
  pinMode(echopin,INPUT);
  Serial.begin(9600);
  }
void loop()
{
  digitalWrite(trigpin,LOW);
  delayMicroseconds(2);
  digitalWrite(trigpin,HIGH);
  delayMicroseconds(30);
  digitalWrite(trigpin,LOW);
  duration = pulseIn(echopin,HIGH);
  distance  = duration*0.034/2;
  Serial.print("Distance  ");
  Serial.println(distance);
  
  }
