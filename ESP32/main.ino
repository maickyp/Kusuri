#include <WiFi.h>               // WiFi library
#include <NTPClient.h>          // NTP Client for NTP Servers and hours
#include <esp_wifi.h>           // WiFi library for ESP32 connection
#include <WiFiUdp.h>            // WiFi UDP library 
#include <HTTPClient.h>         // HTTP Client
#include "functions.h"          // Functions file
#include "wifiConnections.h"    // Wifi Connections
#include <Preferences.h>        // Preference storage
#include <ArduinoJson.h>        // JSON converter
#include <Servo.h>              // Servos drivers
#include "servosFuntions.h";    // Servos functions

/* PINS */
static const int wifiButton = 17;
static const int medButton = 3;
static const int piezo = 36;
static const int buzzer = 14;
static const int statusLed = 25;

/* Instances*/
// SSID Storage
Preferences preferences;  // declare class object

//NTP Client
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

//Servo
Servo servo1;

/* Variable Declarations */

// Wifi Credentials
const  char* rssiSSID;
const  char* password;

// Time String
String timing;

int reg = 0;
bool medCollected = false;
bool newWifiConn = false;
bool medFallen = false;


int analog_value = 0;

/* Function Declarations */
// Interruption function checking if med collected
void IRAM_ATTR collect() {
  medCollected = true;
}

// Interruption function checking if new wifi button was pressed.
void IRAM_ATTR newWifi() {
  newWifiConn = true;
}

void setup() {
  Serial.begin(115200); 
  wifiInit(rssiSSID, password, preferences); // Wifi connection routine. 
  //initSmartConfig(preferences);
  timeClient.begin(); // NTP "time client" initiated.
  timeClient.setTimeOffset(-18000); // GMT adjust, Mexico Summer time GMT-5

  // Pin configuration

  // Interruption + internal pull up configuration
  pinMode(medButton, INPUT_PULLUP);
  attachInterrupt(medButton, collect, FALLING);

  pinMode(wifiButton, INPUT_PULLUP);
  attachInterrupt(wifiButton, newWifi, FALLING);

  // Inputs + Outputs
  pinMode(piezo, INPUT);
  pinMode(buzzer, OUTPUT);
  
  pinMode(statusLed, OUTPUT);
  digitalWrite(statusLed, LOW);

  // Servo Pin config
  servo1.attach(servo1Pin);
  servo1.write(74); // Set up position
}

void loop() {
  // When new wifi button is pressed
  if (newWifiConn) {
    preferences.begin("wifi", false);      // Clear current config
    preferences.putString("ssid", "none");
    preferences.putString("password", "none");
    preferences.end();
    wifiInit(rssiSSID, password, preferences); // Set new config
    newWifiConn = false;
  }
  if (WiFi.status() != WL_CONNECTED) {            // If not connected to WiFi, reconnect.
    digitalWrite(statusLed, LOW);                 // Turn off status Led
    if(WiFi.getAutoReconnect()){                  // Autoreconnect method
      digitalWrite(statusLed, HIGH);              // Turn on status Led
    }
  }
  else {

    timing = timeStamp(timeClient);                       // Get time on a string with format "HH:MM"
    readMed(preferences);                                 // Read medicine info in DB and put it on preferences
    reg = function2(timing, reg, servo1, preferences);    // Compare hours, if it's time, sounds alarm

    if (medCollected && reg > 0) {                        // Press collect button while it's time, drop pill
      buzzer(2, 100);
      dropPill(reg, 0);
      if (medFallen) {                                    // If piezo detects fallen med register it
        reg = collected(reg, preferences);
        medCollected = false;
        medFallen = false;
      }
    }

    if (reg > 0) {                                        // When some time is passed, reg = 0.
      if (checkMillis(preferences))
        reg = 0;
    }

    if (reg == 0) {
      medCollected = false;
    }

    wait(1000);                                           // Every loop takes 1 second to repeat
  }
}



void dropPill(int reg, int i) {                           // 'int a' it's a counter, max 3.
  if (reg == 1)                                           // Drop pill in container 1
    pill(1, servo1);                                      

  else if (reg == 2)                                      // Drop pill in container 2
    pill(2, servo1);                                      

  else if (reg == 3){                                     // Drop pill in both containers
    pill(1, servo1);
    pill(2, servo1);
  }

  wait(50);
  if (Fall() || i >= 3){                                  // Clear medCollected button
    medCollected = false;
    return;
  }

  else {
    i++;                                                  // Increment 'a' counter
    dropPill(reg, i);                                     // Recurrence
  }
}

bool Fall() {                                             // Function that detects if pill was droped.
  unsigned long time_now = millis();                      // Take current time in milis
  int adc_value = 0;                                      // Clear adc value
  bool fallen = LOW;                                      // Clear control fallen value

  while (millis() < time_now + 1000) {                    // Within the next second
    adc_value = analogRead(piezo);                        // Read adc value

    if (adc_value > 50) {                                 // If adc value is above 50 (ref 4095 is the max value)
      fallen = HIGH;                                      // set Flags high
      medFallen = true;                                     
    }
  }
  return fallen;
}
