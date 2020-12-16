#include <Arduino.h>
#include <painlessMesh.h>
#include <Wire.h>
#include <SPI.h>

#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
 
const int AirValue = 600;   //you need to replace this value with Value_1
const int WaterValue = 350;  //you need to replace this value with Value_2
int soilMoistureValue = 0;
int soilmoisturepercent=0;

#define   MESH_SSID       "Shivali"
#define   MESH_PASSWORD   "testpassword"
#define   MESH_PORT       5555

void sendMessage(); 
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback(); 
void nodeTimeAdjustedCallback(int32_t offset); 
void delayReceivedCallback(uint32_t from, int32_t delay);

Scheduler     userScheduler; // to control your personal task
painlessMesh  mesh;

bool calc_delay = false;
SimpleList<uint32_t> nodes;

void sendMessage() ; // Prototype
Task taskSendMessage( TASK_SECOND * 1, TASK_FOREVER, &sendMessage ); // start with a one second interval

void setup() {
  Serial.begin(9600); // open serial port, set the baud rate to 9600 bps
  lcd.begin(16, 2);
   mesh.setDebugMsgTypes(CONNECTION | SYNC);
  mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onNodeDelayReceived(&delayReceivedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();


  randomSeed(analogRead(A0));

}
void loop() {

userScheduler.execute(); // it will run mesh scheduler as well
  mesh.update();

/*soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
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
}  */
}
void sendMessage() {
  soilMoistureValue = analogRead(A0);  //put Sensor insert into soil
  Serial.println(soilMoistureValue);
  String moistmsg="Hello World";
  moistmsg ="";
  soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
  if(soilmoisturepercent >= 100)
  {
    Serial.println("100 %");
    lcd.setCursor(0, 0);
    lcd.print("Soil Moisture");
    lcd.setCursor(0, 1);
    lcd.print("100 %");
    moistmsg ="Moisture =100";
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
    moistmsg ="Moisture =0";

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
    moistmsg =("Moisture ="+ soilmoisturepercent);

    delay(250);
    lcd.clear();
  }
  
 
  
  mesh.sendBroadcast(moistmsg);
    
  Serial.println("Sending message:"+ moistmsg);
  
  taskSendMessage.setInterval( random(TASK_SECOND * 1, TASK_SECOND * 5));  // between 1 and 5 seconds
}


void receivedCallback(uint32_t from, String & msg) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());

/*bool motion = digitalRead(PIR); // Read the PIR
if (motion)
digitalWrite(LED,HIGH); // Turns on the LED
else
digitalWrite(LED,LOW);
*/

}

void newConnectionCallback(uint32_t nodeId) {
  // Reset blink task
  
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections %s\n", mesh.subConnectionJson().c_str());
  // Reset blink task
 
  nodes = mesh.getNodeList();

  Serial.printf("Num nodes: %d\n", nodes.size());
  Serial.printf("Connection list:");

  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println();
  calc_delay = true;
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}