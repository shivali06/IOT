#include <Arduino.h>
#define BME_SCK 13
#define BME_MISO 13
#define BME_MOSI 14
#define BME_CS 14
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>


Adafruit_BME680 bme;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial);
  //Serial.println(F("BME680 test"));

  if (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    while (1);
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
}

void loop() {
  // put your main code here, to run repeatedly:
  if (! bme.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }
 // Serial.print("Temperature = ");
  Serial.println(bme.temperature);
 // printf(" *C");

//   Serial.print("Pressure = ");
//   Serial.print(bme.pressure / 100.0);
//   Serial.println(" hPa");

//   Serial.print("Humidity = ");
//   Serial.print(bme.humidity);
//   Serial.println(" %");

//   Serial.print("Gas = ");
//   Serial.print(bme.gas_resistance / 1000.0);
//   Serial.println(" KOhms");

//  /* Serial.print("Approx. Altitude = ");
//   Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
//   Serial.println(" m");*/

//   Serial.println();
  delay(2000);
}