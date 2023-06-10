#include <VirtualWire.h>
const int ledPin = 12;
void setup()
{
    vw_set_ptt_inverted(true);
    vw_set_rx_pin(A0);
    vw_setup(4000);
    pinMode(ledPin, OUTPUT);
    vw_rx_start();
}
    void loop()
{
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN; 
 
    if (vw_get_message(buf, &buflen))
    {
      if(buf[0]=='2')
      {  
       digitalWrite(ledPin,HIGH);
       delay(500);
        digitalWrite(ledPin,LOW);
         delay(500);
         digitalWrite(ledPin,HIGH);
       delay(500);
        digitalWrite(ledPin,LOW);
         delay(500);
         
      }  
      if(buf[0]=='9')
      {
        digitalWrite(ledPin,HIGH);
       delay(2000);
      }
    }
}
