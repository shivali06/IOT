#include <SPI.h>
#include <LoRa.h>
#include <Arduino.h>
//#include <Adafruit_GPS.h>
//static const uint8_t RX   = 16;
///static const uint8_t TX   = 17;
#define mbedtls_aes.h esp_aes.h
//#define GPSSerial Serial2
#define MBEDTLS_AES_ENCRYPT 1
#define mbedtls_aes_setkey_enc esp_aes_setkey
//Adafruit_GPS GPS(&GPSSerial);

#include "mbedtls/aes.h"
#include "mbedtls/pk.h"

#define AES_BLOCK_SIZE  16
// For 128 bit key, the key must be exactly 16 bytes
//int sensorValue = analogRead(4);
char * key = "abcdefghijklmnop";
char *input = "Hello Lora World";
int counter = 0;

void setup() {
  Serial.begin(9600);
 // GPS.begin(9600);
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setSPI(SPI);
  LoRa.setPins(SS, LORA_DEFAULT_RESET_PIN, BLE_AUDIO0_IRQ_IDX); 

 // GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);

 // GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  while (!Serial);

  Serial.println("LoRa Sender");

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

}

void loop() {
  unsigned char cipherText[AES_BLOCK_SIZE];
  char input[50];
  sprintf(input,"%d",123);

   mbedtls_aes_context aes;

  // Encrypting Data using AES
  esp_aes_init(&aes);
  mbedtls_aes_setkey_enc(&aes, (const unsigned char*) key, strlen(key) * 8 );
  esp_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, (const unsigned char*)input, cipherText);
  esp_aes_free(&aes);

  Serial.println();
  Serial.print("Encrypted Data: ");

  LoRa.beginPacket();
  LoRa.write(cipherText, 16);
  //LoRa.print("Hello!");
  LoRa.endPacket();

  for (int i = 0; i < AES_BLOCK_SIZE; i++) {
    // Print each byte as a hexadecimal 
    // string with two characters padded
    // with a leading zero if needed
    Serial.printf("%02x", (int) cipherText[i]);

  }
 
   delay(1000); 
}