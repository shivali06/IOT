#include <Arduino.h>
#include "mbedtls/aes.h"
#include "mbedtls/pk.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"

// const char* public_pem_key = \

// "-----BEGIN PUBLIC KEY-----\n" \
// "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA1pj2Hfctwv32oo/SDsZe\n" \
// "gw/feTWnzPSY0Zx7aDhutqNt9fERpu8InLDczLx5a7FPDiQIBuhxLKlosdF7y4cW\n" \
// "jh4YK4eXSE42fTRmVzdOM8+facliQesgnLxBJVRCtMjQpQX5KMi7KjSuM7lxRw0u\n" \
// "zmTsl996nCdBhKIRcEVw8BphtN5tWMPd00dyFTU4VVAgggV4bxZ/+0M61TSvB9ua\n" \
// "qg5I09UVUCALRYkW38W5i7Yuvb++Ynb5KTJzRh//YFw0/V/ScUE6yXnu3VZeTazI\n" \
// "cUTmCgTbN//GRSBrZlwlKYE1jPjUunJMOULMRlm7JQBo03oGtizIuWpA5uxJWw0i\n" \
// "IQIDAQAB\n" \
// "-----END PUBLIC KEY-----\n";


// const char* private_pem_key = \

// "-----BEGIN RSA PRIVATE KEY-----\n" \
// "MIIEpAIBAAKCAQEA1pj2Hfctwv32oo/SDsZegw/feTWnzPSY0Zx7aDhutqNt9fER\n" \
// "pu8InLDczLx5a7FPDiQIBuhxLKlosdF7y4cWjh4YK4eXSE42fTRmVzdOM8+facli\n" \
// "QesgnLxBJVRCtMjQpQX5KMi7KjSuM7lxRw0uzmTsl996nCdBhKIRcEVw8BphtN5t\n" \
// "WMPd00dyFTU4VVAgggV4bxZ/+0M61TSvB9uaqg5I09UVUCALRYkW38W5i7Yuvb++\n" \
// "Ynb5KTJzRh//YFw0/V/ScUE6yXnu3VZeTazIcUTmCgTbN//GRSBrZlwlKYE1jPjU\n" \
// "unJMOULMRlm7JQBo03oGtizIuWpA5uxJWw0iIQIDAQABAoIBAQCVTGf2YxCAV6Xu\n" \
// "ZTbUNqhscG+eaXjru8RSGJc25AQKptsKv4PzatvH/7gt7HZhOUNx6Bl5jN+oHdiL\n" \
// "1d1VtQ85IrDYIAZdNKTuKLWGuiB7JViYBZaJm+bdhI+iQ+QA2LFoDibRfA3TCEGm\n" \
// "FL89mmtVBpwf8UfhOeJ4PzT4sicnszzf/TW4m9LhNRtAs9HBvxAZ2szYHGZTKiqx\n" \
// "VaM27rQDao584xf8cDTMYW3SymktRokTbb7w3I7vP7G6A6JJmfbyrVPoEplTb4+u\n" \
// "SClNJBfFQun+R+FKd5SFu/YFG/B4nKbRFL72OlT625ks2GUNa8ThyZCJkgj0JVel\n" \
// "bGsO/J8BAoGBAOv0aWnOwLsYKI5cdhma5AmYrftJGr6XLAWIkDCioxphvAEemNdL\n" \
// "AF8YTl5OYH7zAjlOoR7UDEckIIc1ASr+1VnwykvsyUpvq6JOmGqdXPwLXk3TPzKq\n" \
// "aNot2IBmr9t8S2CPVX7SC5J7ldaMhdbWy6kk1GnIY0G1AZTEbvu/tUtxAoGBAOjU\n" \
// "EY66MbH6M4sSsV9vExqPvPPxkyysLKVPEPRYQzbTSsM7v3Ol7o3kwRKjDswReFiT\n" \
// "6GaHz6dQUdQGzeX52GCXC6HJGtLBeyPKXWebatmuKb//ju9QBR78aYAlWmf1fKtt\n" \
// "UN4KcPW7fm/KoRAeHhj5eIrZUv/K/1ZMnDb3XQmxAoGAYYaUS3d5kdtaZ/0vk0XC\n" \
// "G3VP9B0zQLObLm5TqsaOQ8lBu/S5l8MYVDXnrnMWz2IB+U3Wqrdam80C5N+zq+HI\n" \
// "rXQpKVilmm6tslhqBycj18mbIO+/n2FwFT39zpSi+0VUooScAd8viUwrkpJif3oo\n" \
// "k7D1WwPIgtAd5y+J7+Q9OwECgYAybCL1RwuWILpi5wR0x4ham0BvkCSwawUsM8hX\n" \
// "Mnoz+GRbjafzsr7MCftqPtfoh+wJOtb8xXLKKFHgWizjHk2m9W82dWgStoYlhSDf\n" \
// "XEAyrk4qX30kBK9nO4DZmL1QgFCki4MU4yv9e8JxEW/6HpukCvr5DwGzldHgbazz\n" \
// "FnUWsQKBgQCy+Yu/OT9EpDuVm3L5sHnz4fdWjqs3WO90qsQr40hAzYTxz0LC6Wn/\n" \
// "3APN1Ec0UjSH6HeqfENCox1WULVMKm4WlfBWB3ARtF+PPy645D6eVa3bFab//jpv\n" \
// "ckjBw6dpQT4GTiZwy656/9zuHQsEXkDXG54zTi7MxqWlJgQNnlG1fg==\n" \
// "-----END RSA PRIVATE KEY-----\n";

const char *public_pem_key =

    "-----BEGIN PUBLIC KEY-----\n"
    "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCoCgxx5ZtKmREfAGE+FoXjtRF9\n"
    "kRPL1maEjftgx0ln8PmJOKS7hHAWXVvPsmmaYv/sczjjMjJwE+RPlIiLNquQfpJu\n"
    "zo/1qU0/NvnaieLsfYw8hoiKl9mNad4nQTDbR6nnTS+K/jZIMiwIkI8clwmWaJt0\n"
    "fOfJ/91sGi1hFn5wUQIDAQAB\n"
    "-----END PUBLIC KEY-----";

const char *private_pem_key =

    "-----BEGIN RSA PRIVATE KEY-----\n"
    "MIICXgIBAAKBgQCoCgxx5ZtKmREfAGE+FoXjtRF9kRPL1maEjftgx0ln8PmJOKS7\n"
    "hHAWXVvPsmmaYv/sczjjMjJwE+RPlIiLNquQfpJuzo/1qU0/NvnaieLsfYw8hoiK\n"
    "l9mNad4nQTDbR6nnTS+K/jZIMiwIkI8clwmWaJt0fOfJ/91sGi1hFn5wUQIDAQAB\n"
    "AoGAHg4+D5kr33nlUBA3VG2e4Q2f5T+vtMPSbl8e2T6L6KV3XI3fGRMlITrtkwas\n"
    "UMroT5iEUk7GKrnEGl/M1Gmg4doQrW2xlpqn6/l+xAjLpXQ/XNQqtELKCqtuodTT\n"
    "2ucNeRzzbED9dPur49QG99Se/5MZOLsjeNZ7hN1l72+Q9wkCQQD2vMzD8Kf+zT6A\n"
    "RwAh91xIOgIvkFJ9y7oyAwo5k/CmgJuSrAGabg55SOYDTPEA/WBJzwsXJgGHD7Od\n"
    "qka7Z1+jAkEArljxF3EKkgdOH/KiVQ7Zol5kBPOUcwVGoN/YxZ8bOVRFmrRBcTKg\n"
    "hFKoQO9Ffxkt/nV+v5lstEk21BH1ELJfewJBANTt19QcYg4dN5viKfYmDyhxjyNW\n"
    "7CmNYBD62J8Z0asO/7MS54tLAQif5YfhpXZSMv9fQtXmQZk57qrYoJPnwPsCQQCK\n"
    "grq3JuWA8jDNDNSOwseASYEoteNAJfbgpDXxeuAEAcFykLZVGiWQOqLVQ8JwoS57\n"
    "NBlIWrmT2X6nqMSLSUq1AkEAv+SPI70QhzZIRKF0M9N1Gia1dRMJiV+LGnwleW6E\n"
    "gP/FbAF7lLDeZxKKTSMFA+bBPtNckxJ3QW+CTTtyX1xoUw==\n"
    "-----END RSA PRIVATE KEY-----\n";

void setup() {
  Serial.begin(9600);

  // The maximum length the library supports a single 16 byte block of data
  unsigned char input[6];
  char *msg = "Hello";
  memcpy(input, msg, strlen(msg));
  input[5] = '\0';

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
  Serial.printf("mbedtls_ctr_drbg_seed returned -0x%04x\n", -ret);  // Useful to debug ret messages

  size_t olen = 0;
  unsigned char cipherRSA[255];//[MBEDTLS_MPI_MAX_SIZE];
  mbedtls_pk_init( &pk );
  ret = mbedtls_pk_parse_public_key(&pk, (const unsigned char*) public_pem_key, strlen(public_pem_key)+1);
  ret = mbedtls_pk_encrypt(&pk, 
                            input, 
                            6,//strlen(msg),
                            cipherRSA, 
                            &olen, 
                            sizeof(cipherRSA),
                            mbedtls_ctr_drbg_random, 
                            &ctr_drbg
                          ); 
  mbedtls_ctr_drbg_free(&ctr_drbg);
  mbedtls_entropy_free(&entropy);
  mbedtls_pk_free(&pk);
  Serial.print("olen is ");
  Serial.println(olen);
  Serial.print("RSA Encrypted Data: ");
  for (int i = 0; i < olen; i++) {
    Serial.printf("%02x", (int) cipherRSA[i]);
  }

  if (ret == 0) {
    Serial.println();
    Serial.print("RSA Encrypted Data: ");
    for (int i = 0; i < olen; i++) {
      Serial.printf("%02x", (int) cipherRSA[i]);
    }
    Serial.println();
  } else {
    Serial.printf(" failed\n  ! mbedtls_pk_encrypt returned -0x%04x\n", -ret);
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
  Serial.print("RSA Decrypted Data: ");
  result[16] = '\0';
  Serial.printf("%s", result);
  if (ret == 0) {
    Serial.print("RSA Decrypted Data: ");
    result[16] = '\0';
    Serial.printf("%s", result);
  } else {
    Serial.printf(" failed\n  ! mbedtls_pk_decrypt returned -0x%04x\n", -ret);
  }
}

void loop() {}