#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

#define BME_CS 23
#define ESP32_SDI 21
#define ESP32_SCK 22
#define SEALEVELPRESSURE_HPA (1013.25)
int val;
Adafruit_BME680 bme;


// You will need an internet connection for this...
const char* ssid = "Taala";
const char* password = "chabi1137";

// The endpoint is broker address
const char* awsEndpoint = "a104xg45ewwqxy-ats.iot.us-east-2.amazonaws.com";

// This is the arn
const char* topic = "arn:aws:iot:us-east-2:680051663116:thing/BME680";

// Right now we are also subscribing to our own publication... 
// You can create another topic on AWS to subscribe to if you wish...
const char* subscribeTopic = topic;

// Update the two certificate strings below. Paste in the text of your AWS 
// device certificate and private key. Add a quote character at the start
// of each line and a backslash, n, quote, space, backslash at the end 
// of each line:

// xxxxxxxxxx-certificate.pem.crt
const char* certificate_pem_crt = \

"-----BEGIN CERTIFICATE-----\n" \
"MIIDWTCCAkGgAwIBAgIUGad/tWyy0k9oBgTmQpXwBUQ/EN0wDQYJKoZIhvcNAQEL\n" \
"BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n" \
"SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIwMTAxNTAzMzgz\n" \
"N1oXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n" \
"ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANaY9h33LcL99qKP0g7G\n" \
"XoMP33k1p8z0mNGce2g4brajbfXxEabvCJyw3My8eWuxTw4kCAbocSypaLHRe8uH\n" \
"Fo4eGCuHl0hONn00Zlc3TjPPn2nJYkHrIJy8QSVUQrTI0KUF+SjIuyo0rjO5cUcN\n" \
"Ls5k7JffepwnQYSiEXBFcPAaYbTebVjD3dNHchU1OFVQIIIFeG8Wf/tDOtU0rwfb\n" \
"mqoOSNPVFVAgC0WJFt/FuYu2Lr2/vmJ2+Skyc0Yf/2BcNP1f0nFBOsl57t1WXk2s\n" \
"yHFE5goE2zf/xkUga2ZcJSmBNYz41LpyTDlCzEZZuyUAaNN6BrYsyLlqQObsSVsN\n" \
"IiECAwEAAaNgMF4wHwYDVR0jBBgwFoAURVwYPNda1OiIXZv1nva2RiDgVzcwHQYD\n" \
"VR0OBBYEFGvVCNp6Qmw+EviG//LmdYgskY3ZMAwGA1UdEwEB/wQCMAAwDgYDVR0P\n" \
"AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQCfBMVtW0ccgQeOJjt+wnNrSW36\n" \
"gfWk/kiFSMbIT11yKLNC3diknBWjFXxnxi+1iBGQWib2gpO0imTK4MOuzYG/99S+\n" \
"zJPPwXllxKvyBaGQ4d0AqyS3ZgyXZ4eUrPdyS3g3j6V2MQ/WQ2FoPOEi7dGq87n3\n" \
"loaY++sDwuf9asL3n5fQkpWR+0Yf51FkQwfnVFDdw9HejTlPjr84bjn7kgbISAqc\n" \
"hQATzOPCKSd3wBBmelhpcGF6QA+bJnmU5PFTKvXm4GR2UyxoHw8LP44gEib/4Bnw\n" \
"3EVLCwecjr7EJAkKBK/e7uVsYjDxopRnRmRpKGuJJy+Dmqhrt+symbh8JFRp\n" \

"-----END CERTIFICATE-----\n";

// xxxxxxxxxx-private.pem.key
const char* private_pem_key = \

"-----BEGIN RSA PRIVATE KEY-----\n" \
"MIIEpAIBAAKCAQEA1pj2Hfctwv32oo/SDsZegw/feTWnzPSY0Zx7aDhutqNt9fER\n" \
"pu8InLDczLx5a7FPDiQIBuhxLKlosdF7y4cWjh4YK4eXSE42fTRmVzdOM8+facli\n" \
"QesgnLxBJVRCtMjQpQX5KMi7KjSuM7lxRw0uzmTsl996nCdBhKIRcEVw8BphtN5t\n" \
"WMPd00dyFTU4VVAgggV4bxZ/+0M61TSvB9uaqg5I09UVUCALRYkW38W5i7Yuvb++\n" \
"Ynb5KTJzRh//YFw0/V/ScUE6yXnu3VZeTazIcUTmCgTbN//GRSBrZlwlKYE1jPjU\n" \
"unJMOULMRlm7JQBo03oGtizIuWpA5uxJWw0iIQIDAQABAoIBAQCVTGf2YxCAV6Xu\n" \
"ZTbUNqhscG+eaXjru8RSGJc25AQKptsKv4PzatvH/7gt7HZhOUNx6Bl5jN+oHdiL\n" \
"1d1VtQ85IrDYIAZdNKTuKLWGuiB7JViYBZaJm+bdhI+iQ+QA2LFoDibRfA3TCEGm\n" \
"FL89mmtVBpwf8UfhOeJ4PzT4sicnszzf/TW4m9LhNRtAs9HBvxAZ2szYHGZTKiqx\n" \
"VaM27rQDao584xf8cDTMYW3SymktRokTbb7w3I7vP7G6A6JJmfbyrVPoEplTb4+u\n" \
"SClNJBfFQun+R+FKd5SFu/YFG/B4nKbRFL72OlT625ks2GUNa8ThyZCJkgj0JVel\n" \
"bGsO/J8BAoGBAOv0aWnOwLsYKI5cdhma5AmYrftJGr6XLAWIkDCioxphvAEemNdL\n" \
"AF8YTl5OYH7zAjlOoR7UDEckIIc1ASr+1VnwykvsyUpvq6JOmGqdXPwLXk3TPzKq\n" \
"aNot2IBmr9t8S2CPVX7SC5J7ldaMhdbWy6kk1GnIY0G1AZTEbvu/tUtxAoGBAOjU\n" \
"EY66MbH6M4sSsV9vExqPvPPxkyysLKVPEPRYQzbTSsM7v3Ol7o3kwRKjDswReFiT\n" \
"6GaHz6dQUdQGzeX52GCXC6HJGtLBeyPKXWebatmuKb//ju9QBR78aYAlWmf1fKtt\n" \
"UN4KcPW7fm/KoRAeHhj5eIrZUv/K/1ZMnDb3XQmxAoGAYYaUS3d5kdtaZ/0vk0XC\n" \
"G3VP9B0zQLObLm5TqsaOQ8lBu/S5l8MYVDXnrnMWz2IB+U3Wqrdam80C5N+zq+HI\n" \
"rXQpKVilmm6tslhqBycj18mbIO+/n2FwFT39zpSi+0VUooScAd8viUwrkpJif3oo\n" \
"k7D1WwPIgtAd5y+J7+Q9OwECgYAybCL1RwuWILpi5wR0x4ham0BvkCSwawUsM8hX\n" \
"Mnoz+GRbjafzsr7MCftqPtfoh+wJOtb8xXLKKFHgWizjHk2m9W82dWgStoYlhSDf\n" \
"XEAyrk4qX30kBK9nO4DZmL1QgFCki4MU4yv9e8JxEW/6HpukCvr5DwGzldHgbazz\n" \
"FnUWsQKBgQCy+Yu/OT9EpDuVm3L5sHnz4fdWjqs3WO90qsQr40hAzYTxz0LC6Wn/\n" \
"3APN1Ec0UjSH6HeqfENCox1WULVMKm4WlfBWB3ARtF+PPy645D6eVa3bFab//jpv\n" \
"ckjBw6dpQT4GTiZwy656/9zuHQsEXkDXG54zTi7MxqWlJgQNnlG1fg==\n" \
"-----END RSA PRIVATE KEY-----\n";

// This key should be fine as is. It is just the root certificate.
const char* rootCA = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n" \
"ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n" \
"b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n" \
"MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n" \
"b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n" \
"ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n" \
"9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n" \
"IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n" \
"VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n" \
"93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n" \
"jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n" \
"AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n" \
"A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n" \
"U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n" \
"N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n" \
"o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n" \
"5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n" \
"rqXRfboQnoZsG4q5WTP468SQvvG5\n" \
"-----END CERTIFICATE-----\n";

WiFiClientSecure wiFiClient;
void msgReceived(char* topic, byte* payload, unsigned int len);
PubSubClient pubSubClient(awsEndpoint, 8883, msgReceived, wiFiClient); 
void pubSubCheckConnect();

void setup() {
  Wire.begin(ESP32_SDI, ESP32_SCK);
  Serial.begin(9600); 
  delay(50); 
  Serial.println();
  Serial.println("ESP32 AWS IoT Example");
  Serial.printf("SDK version: %s\n", ESP.getSdkVersion());

  while (!Serial);
  Serial.println(F("BME680 async test"));

  bme.begin();
  /*if (!bme.begin()) {
    Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
    while (1);
  }*/
  bme.setTemperatureOversampling(BME680_OS_8X); 
  bme.readTemperature();
  Serial.print(bme.temperature);

  Serial.print("Connecting to "); 
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  WiFi.waitForConnectResult();
  Serial.print(", WiFi connected, IP address: "); 
  Serial.println(WiFi.localIP());

  wiFiClient.setCACert(rootCA);
  wiFiClient.setCertificate(certificate_pem_crt);
  wiFiClient.setPrivateKey(private_pem_key);
  
}

unsigned long lastPublish;
int msgCount;

void loop() {

  //pubSubCheckConnect();
  pubSubCheckConnect();
  
  if (millis() - lastPublish > 10000) {
    String msg = String("Hello from ESP32:Temperature ") + bme.temperature;
    boolean rc = pubSubClient.publish(topic, msg.c_str());
    Serial.print("Published, rc="); 
    Serial.print( (rc ? "OK: " : "FAILED: ") );
    Serial.println(msg);
    lastPublish = millis();
  }
}

void msgReceived(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on "); 
  Serial.print(topic); 
  Serial.print(": ");
  for (int i = 0; i < length; i++) {
    Serial.print((char) payload[i]);
    char p[15];
    strcpy(p,(char*) payload); 
    sscanf(p, "Interval: &d", &val);
  }
  Serial.println();
}

void pubSubCheckConnect() {
  if (!pubSubClient.connected()) {
    Serial.print("PubSubClient connecting to: "); 
    Serial.print(awsEndpoint);
    while (!pubSubClient.connected()) {
      Serial.print(".");
      pubSubClient.connect("ESPthingXXXX");
      delay(1000);
    }
    Serial.println(" connected");
    pubSubClient.subscribe(subscribeTopic);
  }
  pubSubClient.loop();
}