#include <SPI.h>
#include <LoRa.h>
#include "mbedtls/aes.h"
#include "mbedtls/pk.h"

#define MBEDTLS_AES_ENCRYPT 1
#define mbedtls_aes_setkey_enc esp_aes_setkey
#define AES_BLOCK_SIZE  16

#ifdef ARDUINO_SAMD_MKRWAN1300
#error "This example is not compatible with the Arduino MKR WAN 1300 board!"
#endif

unsigned char cipherText[512];
mbedtls_aes_context aes;
char * key = "abcdefghijklmnop";

void onReceive(int packetSize) {
  // received a packet
  Serial.print("Received packet '");
  int i = 0;
  while(LoRa.available())
  {
      //byte a = LoRa.read();
      cipherText[i] = (char) LoRa.read();
      i++;
  }
  // read packet
  //for (int i = 0; i < packetSize; i++) {
    //Serial.print((char)LoRa.read());
  //  cipherText[i] = (unsigned char) LoRa.read();
  //}
  for (int i = 0; i < AES_BLOCK_SIZE; i++) {
    // Print each byte as a hexadecimal 
    // string with two characters padded
    // with a leading zero if needed
    Serial.printf("%02x", (int) cipherText[i]);
  }

  char decryptedText[17];
  esp_aes_init(&aes);
  mbedtls_aes_setkey_enc(&aes, (const unsigned char*) key, 128);
  mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, (const unsigned char*)cipherText, (unsigned char*)decryptedText);
  mbedtls_aes_free(&aes);
  Serial.print("Decoded Data: ");
  decryptedText[16]='\0';
  Serial.print(decryptedText);
  Serial.println();

  // print RSSI of packet
  Serial.print("' with RSSI ");
  Serial.println(LoRa.packetRssi());
}
void setup() {
  Serial.begin(9600);
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setSPI(SPI);
  LoRa.setPins(SS, LORA_DEFAULT_RESET_PIN, BLE_AUDIO0_IRQ_IDX); 
  while (!Serial);

  Serial.println("LoRa Receiver Callback");
  LoRa.begin(915E6);
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  // register the receive callback
  LoRa.onReceive(onReceive);

  // put the radio into receive mode
  LoRa.receive();
  
}

void loop() {
  Serial.print("Received packet '");
  int i = 0;
  while(LoRa.available())
  {
      //byte a = LoRa.read();
      cipherText[i] = (char) LoRa.read();
      i++;
  }
  // read packet
  //for (int i = 0; i < packetSize; i++) {
    //Serial.print((char)LoRa.read());
  //  cipherText[i] = (unsigned char) LoRa.read();
  //}
  /*for (int i = 0; i < AES_BLOCK_SIZE; i++) {
    // Print each byte as a hexadecimal 
    // string with two characters padded
    // with a leading zero if needed
    Serial.printf("%02x", (int) cipherText[i]);
  }*/

  char decryptedText[17];
  esp_aes_init(&aes);
  mbedtls_aes_setkey_enc(&aes, (const unsigned char*) key, 128);
  mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, (const unsigned char*)cipherText, (unsigned char*)decryptedText);
  mbedtls_aes_free(&aes);
  Serial.print("Decoded Data: ");
  decryptedText[16]='\0';
  Serial.print(decryptedText);
  Serial.println();

  // print RSSI of packet
  Serial.print("' with RSSI ");
  Serial.println(LoRa.packetRssi());
  // do nothing
}