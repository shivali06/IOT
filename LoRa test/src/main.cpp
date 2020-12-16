// //https://www.alictronix.com/archives/860
// #include <SPI.h>
// #include <LoRa.h>
// #include <Adafruit_SSD1306.h>
// #include <Arduino.h>
// #include <Adafruit_GrayOLED.h>
 
// //OLED pins to ESP32 GPIOs via this connecthin:
// //OLED_SDA -- GPIO4
// //OLED_SCL -- GPIO15
// //OLED_RST -- GPIO16
 
// Adafruit_SSD1306  display(-1);
 
// // WIFI_LoRa_32 ports
// // GPIO5  -- SX1278's SCK
// // GPIO19 -- SX1278's MISO
// // GPIO27 -- SX1278's MOSI
// // GPIO18 -- SX1278's CS
// // GPIO14 -- SX1278's RESET
// // GPIO26 -- SX1278's IRQ(Interrupt Request)
 
// #define SS      18
// #define RST     14
// #define DI0     26
// #define BAND    433E6  //915E6 
 
// int counter = 0;
 
// void setup() {
//   pinMode(25,OUTPUT); //Send success, LED will bright 1 second
//   pinMode(16,OUTPUT);
//   digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
//   delay(50); 
//   digitalWrite(16, HIGH);
   
//   Serial.begin(115200);
//   while (!Serial); //If just the the basic function, must connect to a computer
//   // Initialising the UI will init the display too.
//   display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
//   //display.invertDisplay(true);
//   display.clearDisplay();
// display.setTextSize(2);
// display.setTextColor(WHITE);
// display.setCursor(0,28);
// display.println("LoRa Sender");
// display.display();
// delay(2000);
   
//   SPI.begin(5,19,27,18);
//   LoRa.setPins(SS,RST,DI0);
//   Serial.println("LoRa Sender");
//   if (!LoRa.begin(BAND)) {
//     Serial.println("Starting LoRa failed!");
//     while (1);
//   }
//   Serial.println("LoRa Initial OK!");
//   display.println("LoRa Initializing OK!");
//   display.display();
//   delay(2000);
// }
// void loop() {
//   Serial.print("Sending packet: ");
//   Serial.println(counter);
//   display.clearDisplay();
//   display.setFont();
//   display.println("Sending packet ");
//   display.println(String(counter));
//   display.display();
//   // send packet
//   LoRa.beginPacket();
//   LoRa.print("Hello..");
//   LoRa.print(counter);
//   LoRa.endPacket();
//   counter++;
//   digitalWrite(25, HIGH);   // turn the LED on (HIGH is the voltage level)
//   delay(1000);                       // wait for a second
//   digitalWrite(25, LOW);    // turn the LED off by making the voltage LOW
//   delay(1000);                       // wait for a second
//   delay(3000);
// }
#include <SPI.h>
#include <LoRa.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <Adafruit_GrayOLED.h>
#include <U8x8lib.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
 
const int AirValue = 600;   //you need to replace this value with Value_1
const int WaterValue = 350;  //you need to replace this value with Value_2
int soilMoistureValue = 0;
int soilmoisturepercent=0;

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);


int counter = 0;

void setup() {
  Serial.begin(9600);
  u8x8.begin();
  lcd.begin(16, 2);

  u8x8.setFont(u8x8_font_chroma48medium8_r);
  while (!Serial);

  Serial.println("LoRa Sender");
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setSPI(SPI);
  LoRa.setPins(SS, LORA_DEFAULT_RESET_PIN, BLE_AUDIO0_IRQ_IDX); 
  randomSeed(analogRead(A0));


  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

String moistmsg="Hello World";
void loop() {

  moistmsg +="";
  Serial.print("Sending packet: ");
  u8x8.println("Sending ");

  Serial.println(counter);
  soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);


  if(soilmoisturepercent >= 100)
  {
    Serial.println("100 %");
    lcd.setCursor(0, 0);
    lcd.print("Soil Moisture");
    lcd.setCursor(0, 1);
    lcd.print("100 %");
    delay(250);
    lcd.clear();
  }
  else if(soilmoisturepercent <=0)
  {
    Serial.println("0 %");
    lcd.setCursor(0, 0);
    lcd.print("Soil Moisture");
    lcd.setCursor(0, 1);
    lcd.print("0 %");
    delay(250);
    lcd.clear();
  }
  else if(soilmoisturepercent >0 && soilmoisturepercent < 100)
  {
    Serial.print(soilmoisturepercent);
    Serial.println("%");
    lcd.setCursor(0, 0);
    lcd.print("Soil Moisture");
    lcd.setCursor(0, 1);
    lcd.print(soilmoisturepercent);
    lcd.print(" %");
    delay(250);
    lcd.clear();
  }

  moistmsg +=("Moisture ="+ soilmoisturepercent);
  Serial.println(soilMoistureValue);


  // send packet
  LoRa.beginPacket();
  //LoRa.write(0XFF);
  //LoRa.write(0XBB);
  LoRa.print(soilMoistureValue);
  
  u8x8.println(soilMoistureValue);
  //LoRa.print(counter);
  LoRa.endPacket();

  counter++;

  delay(5000);
}
