#include <SPI.h>
#include <LoRa.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GrayOLED.h>
#include <SPI.h>
#include <LoRa.h>
#include <U8x8lib.h>

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);


void setup() {
  Serial.begin(9600);
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  while (!Serial);

  Serial.println("LoRa Receiver");
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setSPI(SPI);
  LoRa.setPins(SS, LORA_DEFAULT_RESET_PIN, BLE_AUDIO0_IRQ_IDX);

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.receive();
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");
    //u8x8.println("Received packet ");

    // read packet
    while (LoRa.available()) {
      Serial.print(String((char)LoRa.read()));
      u8x8.println(String((char)LoRa.read()));
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(String(LoRa.packetRssi()));
    //u8x8.println(LoRa.packetRssi());
  }
}


