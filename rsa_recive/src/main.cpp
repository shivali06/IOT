#include <Arduino.h>
#include "mbedtls/aes.h"
#include "mbedtls/pk.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include <LoRa.h>

const char* public_pem_key = \

"-----BEGIN PUBLIC KEY-----\n" \
"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDYJ0552zCPyzI3kV3tlUsgXxUI\n" \
"5XxuRvMNSKs6OSgVoAId12HXBIAxMNvggsYjjajQUBcWUSM7KnT1aiDlE84C7VgW\n" \
"GhItMHxNIIwYNW3/7FOGD1Lfd1wSoEyh31q/qki5qEh0JHGagc6l31o22Du+PN3/\n" \
"1KPVmjhEaj8/GVex8wIDAQAB\n" \
"-----END PUBLIC KEY-----";

const char* private_pem_key = \

"-----BEGIN RSA PRIVATE KEY-----\n" \
"MIICXQIBAAKBgQDYJ0552zCPyzI3kV3tlUsgXxUI5XxuRvMNSKs6OSgVoAId12HX\n" \
"BIAxMNvggsYjjajQUBcWUSM7KnT1aiDlE84C7VgWGhItMHxNIIwYNW3/7FOGD1Lf\n" \
"d1wSoEyh31q/qki5qEh0JHGagc6l31o22Du+PN3/1KPVmjhEaj8/GVex8wIDAQAB\n" \
"AoGBAJ00eS7G65qo/ezMHLQeUHVKsdG2lbjegQa4Q1AlIbIYnXF2pAHtlA4yqreu\n" \
"ZMyWyqA2n5PrjuQJDOcwOJxiuwCv0lSv6cjwWz5En7lsVeBUAXe59IMD5f7XtsoB\n" \
"6+7tlIw4TI3hx9nPETRr78ia3zyS8w8UhNQGDG3UdYcO7juBAkEA61b9Z/s4GyO2\n" \
"f5xTeg16XFTjIt49dBNzxTPqr0CyX0gCOa1amcNdyX1Zs7dhFPXPRgj8d9/5S30t\n" \
"O9VJ3RmajQJBAOshID7x9TNmmmkW9xxmuhn/FsnYE2bVc/yUtdoijg7qaCGPoyHl\n" \
"LFru4iRlHAASrSBXMY38Gh9K1rmyQNWFnn8CQBGXkBvMwLXupn7Fjcvq5AGw6sW5\n" \
"fTiHF7lfFyUAnhTXSitLlSE3ZHRiZWk0xRIPCe5/T3gaiwUkl1QIrpvThSUCQQDB\n" \
"oGDelQAYWQkVO405fTsKB2/rvh2UFHlUad4riUM080HXSWLMmd3h8FCYKg9Z3Adn\n" \
"14X+Z7nXRRj4iNstOsDfAkA9sEGS5Umwwe2FvTLqVcICF+rWXAaYWsemtRbeDHCK\n" \
"H7fjvDOdqmaGXYu9z+2rKjQak0sYqlIzHOCc/5DCHyj/\n" \
"-----END RSA PRIVATE KEY-----\n";


void setup() {
  Serial.begin(115200);

  // The maximum length the library supports a single 16 byte block of data
  unsigned char input[1024];
  char *msg = "Hello Lora World";
  memcpy(input, msg, strlen(msg));
  input[16] = '\0';

  Serial.printf("\n%s\n",input);

  // Encrypting using RSA public key
  mbedtls_pk_context pk;              // stores a public or private key
  mbedtls_ctr_drbg_context ctr_drbg;  // random number generator context
  mbedtls_entropy_context entropy;
  mbedtls_entropy_init(&entropy);
  const char *pers = "mbedtls_pk_decrypt";
  int ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func,
                         &entropy, (const unsigned char *) pers,
                         strlen( pers )
                       );
  //Serial.printf("mbedtls_ctr_drbg_seed returned -0x%04x\n", -ret);  // Useful to debug ret messages

  size_t olen = 0;
  unsigned char cipherRSA[MBEDTLS_MPI_MAX_SIZE];
  mbedtls_pk_init( &pk );
  ret = mbedtls_pk_parse_public_key(&pk, (const unsigned char*)public_pem_key, strlen(public_pem_key)+1);
  ret = mbedtls_pk_encrypt(&pk, 
                            input, 
                            strlen(msg),
                            cipherRSA, 
                            &olen, 
                            sizeof(cipherRSA),
                            mbedtls_ctr_drbg_random, 
                            &ctr_drbg
                          ); 
  mbedtls_ctr_drbg_free(&ctr_drbg);
  mbedtls_entropy_free(&entropy);
  mbedtls_pk_free(&pk);


  if (ret == 0) {
    Serial.println();
    //Serial.print("RSA Encrypted Data: ");
    for (int i = 0; i < olen; i++) {
      //Serial.printf("%02x", (int) cipherRSA[i]);
    }
    //Serial.println();
  } else {
    //Serial.printf(" failed\n  ! mbedtls_pk_encrypt returned -0x%04x\n", -ret);
  }

  // Decrypt using RSA
  unsigned char result[1024];
  mbedtls_pk_init( &pk );
  mbedtls_entropy_init(&entropy);
  ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func,
                         &entropy, (const unsigned char *) pers,
                         strlen( pers )
                       );
  ret = mbedtls_pk_parse_key( &pk, (unsigned char*)private_pem_key, strlen(private_pem_key)+1, NULL, 0);  // No password for key
  
  int lenCipher = olen;
  ret = mbedtls_pk_decrypt(&pk,
                           cipherRSA,  
                           lenCipher, 
                           result, 
                           &olen, 
                           sizeof(result),
                           mbedtls_ctr_drbg_random,
                           &ctr_drbg
                          );

  if (ret == 0) {
    Serial.print("RSA Decrypted Data: ");
    result[16] = '\0';
    Serial.printf("%s", result);
  } else {
    Serial.printf(" failed\n  ! mbedtls_pk_decrypt returned -0x%04x\n", -ret);
  }
}

void loop() {}