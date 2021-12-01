#include <WiFi.h>
#include "time.h"
#include <HTTPClient.h>
#include <SoftwareSerial.h>
#include <SolarPosition.h>


#define MYPORT_TX 17
#define MYPORT_RX 16

SoftwareSerial myPort;

unsigned long Epoch_Time;

SolarPosition ReynosaUT(26.050425, -98.260485); //latitud y longitud

const uint8_t digits = 3;

unsigned long degree_unit = 76.444444444444;

SolarPosition_t savedPosition;

const char* ssid = "INFINITUM7EDE_2.4";
const char* password =  "3KU99ZC64q";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

short int httpResponseCode;
String url;
String httpRequestData, payload;


void setup() {

  Serial.begin(115200);

  pinMode(MYPORT_RX, INPUT);
  pinMode(MYPORT_TX, OUTPUT);

  setTime(SECS_YR_2000);

  myPort.begin(19200, SWSERIAL_8N1, MYPORT_RX, MYPORT_TX, false);
  if (!myPort) { // If the object did not initialize, then its configuration is invalid
    Serial.println("Invalid SoftwareSerial pin configuration, check config"); 
    while (1) { // Don't continue with invalid configuration
      delay (1000);
    }
  }

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected."); 

  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  //disconnect WiFi as it's no longer needed


  SolarPosition::setTimeProvider(getTime);

  // myPort.write(0xC2); //Motor One Forward
  // myPort.write(127);
  // delay(180*degree_unit);
  // myPort.write(0xC0); //Motor One Forward
  // myPort.write(127);

  
}


void Connect(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("IP: " + (String)WiFi.localIP());  
  
}

void printLocalTime(){
  struct tm timeinfo;
  printSolarPosition(ReynosaUT.getSolarPosition(), digits);
}

void printSolarPosition(SolarPosition_t pos, int numDigits)
{
  Serial.print(F("el: "));
  Serial.print(pos.elevation, numDigits);
  Serial.print(F(" deg\t"));

  Serial.print(F("az: "));
  Serial.print(pos.azimuth, numDigits);
  Serial.println(F(" deg"));
}


void loop() {

}


time_t getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

void go()
{

  delay(1000);
  Epoch_Time = getTime();
  Serial.print("Epoch Time: ");
  Serial.println(Epoch_Time);
  printLocalTime();
  myPort.write(0xC2); //Motor One Forward
  myPort.write(127);
  delay(100);
  // myPort.write(0xCA); //Motor two Forward    
  // myPort.write(127);
}