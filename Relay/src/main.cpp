#include <Arduino.h>
#include <U8x8lib.h>
#define LED 25
#define PIR 2// Add the pin you used to connect the PIR
#define RELAY 13// Add the pin you used to connect the RELAY
bool on = false;
//U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);
void setup() {
  pinMode(LED,OUTPUT); // Sets the LED as an output
  pinMode(RELAY,OUTPUT); // Sets RELAY as an output
}
void loop() {
bool motion = digitalRead(PIR);
 if (motion)
  {
    digitalWrite(RELAY,HIGH);
    
  }
  else
  {
    
    digitalWrite(RELAY,LOW);
    
  }

}