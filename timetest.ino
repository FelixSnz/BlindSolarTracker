/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/epoch-unix-time-esp32-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

// #include <WiFi.h>
// #include "time.h"

// // Replace with your network credentials
// const char* ssid = "INFINITUM7EDE_2.4";
// const char* password =  "3KU99ZC64q";

// // NTP server to request epoch time
// const char* ntpServer = "pool.ntp.org";

// // Variable to save current epoch time
// unsigned long epochTime; 

// // Function that gets current epoch time
// time_t getTime() {
//   time_t now;
//   struct tm timeinfo;
//   if (!getLocalTime(&timeinfo)) {
//     //Serial.println("Failed to obtain time");
//     return(0);
//   }
//   time(&now);
//   return now;
// }

// // Initialize WiFi
// void initWiFi() {
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("");
//   Serial.println("WiFi connected.");
// }

// void wifioff()
// {
//     WiFi.disconnect(true);
//     WiFi.mode(WIFI_OFF);
// }

// void setup() {
//   Serial.begin(115200);
//   initWiFi();
//   configTime(0, 0, ntpServer);


// }

// void loop() {
//   epochTime = getTime();
//   Serial.print("Epoch Time: ");
//   Serial.println(epochTime);
//   delay(1000);
// }