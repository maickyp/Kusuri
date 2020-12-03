#include <ArduinoJson.h>        // JSON converter
#include <Preferences.h>        // Preference storage
#include <Servo.h>              // Servos drivers

// Methods
String timeStamp(NTPClient timeClient);                 // Return actual time in HH:MM format
String post(String message, int i);                     // Connection via POST with some php files on web server
void newDose(int i, Preferences preferences);           // Modify the next dose time; use ONLY if dose was confirmed
void readMed(Preferences preferences);                  // Reads med info from server and stores it on preferences
void writeLog(int med, Preferences preferences);        // Writes on DB the last med supplied
int getHour(String timeStamp);                          // Receives a time string with format "HH:MM", gives "HH" as integer
int getMinute(String timeStamp);                        // Receives an time string with format "HH:MM", gives "MM" as integer
String addHour(int i, Preferences preferences);         // Adds next dose time to a med.
bool checkMillis(Preferences preferences);              // Checks time since dose time, if applies sounds a buzzer
void milis(Preferences preferences);                    // Storage time right now, for checkmilis() method
int quantity(int i, Preferences preferences);           // Checks quantity of med in storage and decrement it
void checkQuantity(int reg, Preferences preferences);   // Decrement quantity of med in DB
void pill(int i, Servo servo1);                         // Drops the pill
bool isMedLeft(int i, Preferences preferences);         // Check if there is at least 1 pill of any med
void buzzer(int numTimes, int delay_ms);                // Activates buzzer for alarms
void wait(int ms);                                      // Wait


// Pins
static const int buzzerPin = 14;                        // Buzzer Pin
bool buzzerStatus = LOW;                                // Initial state of buzzer
unsigned long waitTime = 3 * 60000;                     // Wait time for sound alarms
int buzzerCounter = 0;                                  // Buzzer counter for checkmillis() method



String timeStamp(NTPClient timeClient) {

  String formattedDate;
  String dayStamp;
  String timeStamp;
  String timing;

  while (!timeClient.update()) {                 // Update actual hour
    timeClient.forceUpdate();
  }

  formattedDate = timeClient.getFormattedDate(); //Get hour with date

  // Extract date
  int splitT = formattedDate.indexOf("T");

  // Extract time
  timeStamp = formattedDate.substring(splitT + 1, formattedDate.length() - 1);

  // HH:MM convertion
  timing = timeStamp.substring(0, 2);
  timing += ":";
  timing += timeStamp.substring(3, 5);
  Serial.println(timing);
  wait(1000);

  return timing;
}

String post(String message, int i) {
  String host;
  String response;

  if (i == 0)
    host = "https://kusuriproyect.000webhostapp.com/conection.php";       // Write on DB
  else if (i == 1)
    host = "https://kusuriproyect.000webhostapp.com/readMed.php";         // Read from DB
  else if (i == 2)
    host = "https://kusuriproyect.000webhostapp.com/modHour.php";         // Modify next doseTime
  else if (i == 3)
    host = "https://kusuriproyect.000webhostapp.com/modCollected.php";    // Modify Collected cell
  else if (i == 4)
    host = "https://kusuriproyect.000webhostapp.com/modLevel.php";        // Modify Collected cell

  if (WiFi.status() == WL_CONNECTED) {                                    // Check WiFi connection status
    HTTPClient http;                                                      // HTTPClient Declaration
    http.begin(host);                                                     // Specify destination for HTTP request
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");  // Specify content-type header
    int httpResponseCode = http.POST(message);                            // Send the actual POST request

    if (httpResponseCode > 0) {
      response = http.getString();                                        // Get the response to the request
      Serial.println(httpResponseCode);                                   // Print return code
      Serial.println(response);                                           // Print request answer
    } else {
      Serial.print(" Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    http.end();                                                           // Free resources
  } else {
    Serial.println(" Error in WiFi connection ");
  }

  return response;                                                        // return server answer
}

void readMed(Preferences preferences) {
  //Variable declaration
  StaticJsonDocument<400> doc;                                 // Limit memory on JSON Doc
  String query;
  String message;
  String temp;
  query = "key=Hola1234$$";                                    // Key for DB
  message = post(query, 1);                                    // POST on web server, receives info

  DeserializationError error = deserializeJson(doc, message);  // Decode JSON String

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  int medNumber = doc["MedNumber"]; //Get
  // Variable Declaration
  const char* nameMed1;
  const char* tubeNum1;
  const char* doseTime1;
  const char* nextDose1;
  int level1;

  const char* nameMed2;
  const char* doseTime2;
  const char* tubeNum2;
  const char* nextDose2;
  int level2;

  //Put on respective string
  nameMed1 = doc["nameMed1"];
  tubeNum1 = doc["tubeNum1"];
  doseTime1 = doc["doseTime1"];
  nextDose1 = doc["nextDose1"];
  level1 = doc["level1"];

  nameMed2 = doc["nameMed2"];
  tubeNum2 = doc["tubeNum2"];
  doseTime2 = doc["doseTime2"];
  nextDose2 = doc["nextDose2"];
  level2 = doc["level2"];

  // Save data on preferences
  preferences.begin("medicines", false);
  preferences.putInt("medNumber", medNumber);
  preferences.putString("nameMed1", nameMed1);
  preferences.putString("tubeNum1", tubeNum1);
  preferences.putString("doseTime1", doseTime1);
  preferences.putString("nextDose1", nextDose1);
  preferences.putInt("level1", level1);
  preferences.putString("nameMed2", nameMed2);
  preferences.putString("tubeNum2", tubeNum2);
  preferences.putString("doseTime2", doseTime2);
  preferences.putString("nextDose2", nextDose2);
  preferences.putInt("level2", level2);
  preferences.end();
}

void writeLog(int med, Preferences preferences) {
  String medicine, tubeNum, query;
  query = "key=Hola1234$$";
  if (med == 1) {
    preferences.begin("medicines", false);
    medicine = preferences.getString("nameMed1", "none");
    tubeNum = preferences.getString("tubeNum1", "none");
    preferences.end();

    query += "&med=" + medicine + "&tube=" + tubeNum;
    post(query, 0);
  }
  else if (med == 2) {
    preferences.begin("medicines", false);
    medicine = preferences.getString("nameMed2", "none");
    tubeNum = preferences.getString("tubeNum2", "none");
    preferences.end();

    query += "&med=" + medicine + "&tube=" + tubeNum;
    post(query, 0);

  }
}

int getHour(String timeStamp) { //Receives an time string with format "HH:MM", gives "HH" as integer.
  int hour;
  String temp;
  temp = timeStamp.substring(0, 2);
  hour = temp.toInt();
  return hour;
}

int getMinute(String timeStamp) { //Receives an time string with format "HH:MM", gives "MM" as integer.
  int minute;
  String temp;
  temp = timeStamp.substring(3, 5);
  minute = temp.toInt();
  return minute;
}

String addHour(int i, Preferences preferences) {//Función que devuelve el string de la siguiente hora de dosis de un medicamento
  String nextDose, doseTime;
  int doseHour, doseMin, nextHour, nextMin, newHour, newMin;

  String newDose = "";
  if (i == 1) {
    preferences.begin("medicines", false);
    doseTime = preferences.getString("doseTime1", "none");
    nextDose = preferences.getString("nextDose1", "none");
    preferences.end();
  }
  else if (i == 2) {
    preferences.begin("medicines", false);
    doseTime = preferences.getString("doseTime2", "none");
    nextDose = preferences.getString("nextDose2", "none");
    preferences.end();
  }

  doseHour = getHour(doseTime);
  doseMin = getMinute(doseTime);

  nextHour = getHour(nextDose);
  nextMin = getMinute(nextDose);

  newHour = doseHour + nextHour;

  newMin = doseMin + nextMin;
  if (newMin > 59) {
    newMin -= 60;
    newHour += 1;
  }

  if (newHour > 23)
    newHour -= 24;

  if (newHour < 10) {
    newDose = "0";
    newDose += newHour;
  }
  else
    newDose = newHour;

  newDose += ":";

  if (newMin < 10) {
    newDose += "0";
    newDose += newMin;
  }
  else
    newDose += newMin;

  return newDose;
}

int function2(String timeStamp, int reg, Servo servo1, Preferences preferences) { //Receives an time string with format "HH:MM"
  String nextDose1, nextDose2;
  preferences.begin("medicines", false);
  nextDose1 = preferences.getString("nextDose1", "none");
  nextDose2 = preferences.getString("nextDose2", "none");
  preferences.end();

  Serial.println("NextDose1: " + nextDose1);
  Serial.println("NextDose2: " + nextDose2);

  if (timeStamp == nextDose1) {
    if (isMedLeft(1,  preferences)) {

      Serial.println("Es hora de la medicina 1");
      writeLog(1, preferences); //Rutina de registro
      newDose(1, preferences);
      milis(preferences);
      Serial.println("Buzzer");
      buzzer(5, 1000);
      reg += 1; // Se surtió medicina 1
    }
  }

  if (timeStamp == nextDose2) {
    if (isMedLeft(2,  preferences)) {
      Serial.println("Es hora de la medicina 2");
      writeLog(2, preferences); //Rutina de registro
      newDose(2, preferences);
      milis(preferences);
      buzzer(5, 1000);
      reg += 2; // Se surtió medicina 2
    }
  }

  return reg;

}

void newDose(int i, Preferences preferences) {

  String nextDose;
  String query = "key=Hola1234$$";
  nextDose = addHour(i, preferences);
  query += "&id=";
  query += i;
  query += "&nextDose=";
  query += nextDose;
  //Serial.println(query);
  post(query, 2);

}

int collected (int reg, Preferences preferences) {
  String query = "key=Hola1234$$";
  String nameMed;
  buzzerCounter = 0;


  if (reg == 1) {
    preferences.begin("medicines", false);
    nameMed = preferences.getString("nameMed1", "none");
    preferences.end();
    query += "&reg=1&nameMed1=";
    query += nameMed;
    Serial.println(query);
  }

  else if (reg == 2) {
    preferences.begin("medicines", false);
    nameMed = preferences.getString("nameMed2", "none");
    preferences.end();
    query += "&reg=2&nameMed2=";
    query += nameMed;
    Serial.println(query);
  }

  else if (reg == 3) {
    preferences.begin("medicines", false);
    nameMed = preferences.getString("nameMed1", "none");
    query += "&reg=3&nameMed1=";
    query += nameMed;
    nameMed = preferences.getString("nameMed2", "none");
    query += "&nameMed2=";
    query += nameMed;
    preferences.end();
    Serial.println(query);
  }


  if (post(query, 3) == "OK") {
    checkQuantity(reg, preferences);
    return 0;
  }
  else
    return reg;
}

void milis(Preferences preferences) {
  preferences.begin("millis", false);
  preferences.putULong("time", millis());
  preferences.end();
}

bool checkMillis(Preferences preferences) {
  unsigned long time_now = millis();
  unsigned long time_med;

  preferences.begin("millis", false);
  time_med = preferences.getULong("time", millis());
  preferences.end();

  if ((time_now > time_med + waitTime / 3) && buzzerCounter == 0) {   // 3 minutos de espera
    buzzer(5, 1000);
    buzzerCounter++;
  }

  if ((time_now > time_med + waitTime * 0.6) && buzzerCounter == 1) { // 3 minutos de espera
    buzzer(5, 1000);
    buzzerCounter++;
  }
  if (time_now > time_med + waitTime) {                               // 3 minutos de espera
    Serial.println("No se recogió la medicina");
    buzzerCounter = 0;
    return true;
  }
  else
    return false;

}

int quantity(int i, Preferences preferences) {

  int quantity;
  if (i == 1) {
    preferences.begin("medicines", false);
    quantity = preferences.getInt("level1", 0);
    preferences.end();
  }
  else if (i == 2) {
    preferences.begin("medicines", false);
    quantity = preferences.getInt("level2", 0);
    preferences.end();
  }

  quantity--;
  if (quantity < 0)
    quantity = 0;

  return quantity;
}

void checkQuantity(int reg, Preferences preferences) {
  String query = "key=Hola1234$$";

  if (reg == 1) {
    int level = quantity(1, preferences);
    query += "&reg=";
    query += "1";
    query += "&level1=";
    query += level;
  }
  else if (reg == 2) {
    int level = quantity(2, preferences);
    query += "&reg=";
    query += "2";
    query += "&level2=";
    query += level;
  }
  else if (reg == 3) {
    int level = quantity(1, preferences);
    query += "&reg=";
    query += "3";
    query += "&level1=";
    query += level;
    level = quantity(2, preferences);
    query += "&level2=";
    query += level;
  }
  Serial.println(reg);
  if (post(query, 4) == "OK")
    Serial.println("Se restó la cantidad de medicamento");

  return;
}

bool isMedLeft(int i, Preferences preferences) {

  int quantity;
  if (i == 1) {
    preferences.begin("medicines", false);
    quantity = preferences.getInt("level1", 0);
    preferences.end();
  }
  else if (i == 2) {
    preferences.begin("medicines", false);
    quantity = preferences.getInt("level2", 0);
    preferences.end();
  }

  if (quantity <= 0)
    return false;

  else
    return true;
}

void wait(int ms) {
  unsigned long actual_time = millis();

  while (millis() < actual_time + ms) {}

}

void buzzer(int numTimes, int delay_ms) {

  for (int i = 0; i < numTimes; i++) {
    buzzerStatus = HIGH;
    //Serial.println(buzzerStatus);
    digitalWrite(buzzerPin, buzzerStatus);
    delay(delay_ms);
    buzzerStatus = LOW;
    //Serial.println(buzzerStatus);
    digitalWrite(buzzerPin, buzzerStatus);
    delay(delay_ms);
    Serial.println(i);
  }
}
