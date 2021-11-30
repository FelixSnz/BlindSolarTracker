#include <WiFi.h>
#include "time.h"
#include <HTTPClient.h>
#include <SoftwareSerial.h>
#include <SolarPosition.h>


#define MYPORT_TX 17
#define MYPORT_RX 16

SoftwareSerial myPort;

SolarPosition Toronto(26.050425, -98.260485); //latitud y longitud


const char* ssid = "INFINITUM7EDE_2.4";
const char* password =  "3KU99ZC64q";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -21600;
const int   daylightOffset_sec = 3600;




short int httpResponseCode;
String url;
String httpRequestData, payload;
String ServerIP = "";

void setup() {

  Serial.begin(115200);

  pinMode(MYPORT_RX, INPUT);
  pinMode(MYPORT_TX, OUTPUT);

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
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  SolarPosition::setTimeProvider(getNtpTime);
  
}

void Connect(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("IP: " + (String)WiFi.localIP());  
  
}

void POST(String var_name, float value)
{
  HTTPClient http;
  http.addHeader("Content-Type", "text/plain");
  url = "http://"+(String)ServerIP+"/POST?"+(String)var_name+"="+String(value);
  http.begin(url.c_str());
  httpResponseCode = http.POST("");
  delay(300);
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}


String GET(String var_name){
  HTTPClient http;

  url = "http://"+(String)ServerIP+"/GET?"+(String)var_name;
  http.begin(url.c_str());
  httpResponseCode = http.GET();
  delay(300);
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    return http.getString();
  }
  return "Null";
  http.end();
}

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour,3, "%H", &timeinfo);
  Serial.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay,10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();
}

time_t getNtpTime()
{
  struct tm timeinfo;
  return mktime(&timeinfo);
}


void loop() {
go();
}

void go()
{

  delay(1000);
  printLocalTime();
      // myPort.write(0xC2); //Motor One Forward
      // myPort.write(127);
      // delay(100);
      // myPort.write(0xCA); //Motor two Forward    
      // myPort.write(127);
}