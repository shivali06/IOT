#include <Arduino.h>

#define sensor_in 0 // Needs to be an interrupt pin

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}
float round_to_dp( float in_value, int decimal_place )
{
  float multiplier = powf( 10.0f, decimal_place );
  in_value = roundf( in_value * multiplier ) / multiplier;
  return in_value;
}
// the loop routine runs over and over again forever:
void loop() {
   float volt = 0;
    for(int i=0; i<800; i++)
    {
        volt += ((float)analogRead(sensor_in)/4096)*5;
    }
    volt = volt/800;
    volt = round_to_dp(volt,1);
    float ntu = 0.0;
   
    ntu = -1120.4*volt*volt+5742.3*volt-4353.8; 
    
  // read the input on analog pin 0:
  //int sensorValue = analogRead(sensor_in);

  // print out the value you read:
  //float y = -1120.4*3.3*3.3 + 5742.3*3.3 - 4352.9;

  //float voltage = sensorValue * (5.0 / 1024.0); // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  //float ntu = (-781.25*voltage) + 3000;
  //float y = ((-1120.4*voltage*voltage)+ (5742.3*voltage) - 4352.9);


    Serial.println(volt);
  //Serial.println("Sensor value");
   // Serial.println(ntu);

  delay(4100);        // delay in between reads for stability
}