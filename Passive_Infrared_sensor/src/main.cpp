#include <Arduino.h>
#include <U8x8lib.h>
#define LED 25
#define PIR 2 // Add the pin you used to connect the PIR

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

void setup() {
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  pinMode(LED,OUTPUT); // Sets the LED as an output
  pinMode(PIR,INPUT); // Sets the PIR as an input
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
  bool motion = digitalRead(PIR); // Read the PIR
  if (motion)
  {
    digitalWrite(LED,HIGH); // Turns on the LED
    u8x8.println("yes");
    //delay(1000);
  }
  else
  {
    digitalWrite(LED,LOW);
    u8x8.println("No");
    //delay(1000);
  }
}
