#include <WiFi.h>
#include <esp_wifi.h>
#include <Preferences.h>

// Function declaration
void wifiInit(const  char*, const  char*, Preferences); 
bool checkPrefsStore(Preferences);
void initSmartConfig(Preferences);
String getSsidPass(String);


// This function initializes the wifi with prestored config (preferences)
void wifiInit(const  char* rssiSSID, const  char* password, Preferences preferences)
{
  String PrefSSID, PrefPassword;                                                      
  WiFi.mode(WIFI_AP_STA);                                                             // required to read NVR before WiFi.begin()
  wifi_config_t conf;
  esp_wifi_get_config(WIFI_IF_STA, &conf);                                            // load wifi settings to struct comf
  rssiSSID = reinterpret_cast<const char*>(conf.sta.ssid);
  password = reinterpret_cast<const char*>(conf.sta.password);


  preferences.begin("wifi", false);
  PrefSSID =  preferences.getString("ssid", "none");                                  //NVS key ssid
  PrefPassword  =  preferences.getString("password", "none");                         //NVS key password
  preferences.end();

  if (!checkPrefsStore(preferences))                                                  // see if NV and Prefs are the same
  {                                                                                   /not the sa/ me, setup with SmartConfig
    if ( PrefSSID == "none" ) // New...setup wifi
    {
      initSmartConfig(preferences);                                                   // initSmartConfig will stablish connection with new wifi
      wait(3000);
      ESP.restart();                                                                  // reboot with wifi configured
    }
  }

  WiFi.begin( PrefSSID.c_str() , PrefPassword.c_str() );                              // Wifi connects
}

// Check if NVstored config is equal to preferences stored
bool checkPrefsStore(Preferences preferences)
{
  bool val = false;

  // NV
  String NVssid, NVpass, prefssid, prefpass;

  NVssid = getSsidPass( "ssid" );
  NVpass = getSsidPass( "pass" );

  // Preferences
  preferences.begin("wifi", false);
  prefssid  =  preferences.getString("ssid", "none");                                  //NVS key ssid
  prefpass  =  preferences.getString("password", "none");                              //NVS key password
  preferences.end();

  // Comparation
  if ( NVssid.equals(prefssid) && NVpass.equals(prefpass) )
  {
    val = true;
  }

  return val;
}

void initSmartConfig(Preferences preferences)
{
  // LED status will blink
  static const int ledPin = 25;
  bool ledStatus = LOW;
  


  WiFi.mode( WIFI_AP_STA );                                                           // Init WiFi, start SmartConfig
  WiFi.beginSmartConfig();                                                            // Entering SmartConfig

  while (!WiFi.smartConfigDone())                                                     // Will enter in a loop while smartConfig connects wifi
  {
                                                                                      
    digitalWrite(ledPin, ledStatus);                                                  // flash led to indicate not configured
    ledStatus = !ledStatus;
    wait(600);

  }

  digitalWrite(ledPin, LOW);
  // LED status stop blink here

  // SmartConfig received. Waiting for WiFi
  wait(2000);

  while ( WiFi.status() != WL_CONNECTED )                                             // check till connected
  {
    wait(500);
  }

  digitalWrite(ledPin, HIGH);                                                         // LED status will stay on while connected, if disconnected will blink
  preferences.begin("wifi", false);                                                   // put it in preference storage
  preferences.putString("ssid", WiFi.SSID());
  preferences.putString("password", WiFi.psk());
  preferences.end();

  delay(300);
}  // END SmartConfig()

// This function will get WiFi Configuration in ESP32 Wifi module.
String getSsidPass( String s )
{
  String val = "NONE";                                                                // return "NONE" if wrong key sent
  s.toUpperCase();
  if ( s.compareTo("SSID") == 0 )                                                     // Get SSID
  {
    wifi_config_t conf;
    esp_wifi_get_config( WIFI_IF_STA, &conf );
    val = String( reinterpret_cast<const char*>(conf.sta.ssid) );
  }
  if ( s.compareTo("PASS") == 0 )                                                     // Get Password
  {
    wifi_config_t conf;
    esp_wifi_get_config( WIFI_IF_STA, &conf );
    val = String( reinterpret_cast<const char*>(conf.sta.password) );
  }
  return val;                                                                         // Return value
}
