// #include delay
#define SECONDS_DS(seconds) ((seconds)*1000000UL)

#define Y_LED 14
#define R_LED 13
#define G_LED 12
// #define SW1 5
#define Motor 5
#define WaterSensor A0
#define Buzzer 4


void Line_Notify(String message);

#define LINE_TOKEN "Rz6mjV6NgVby5f8X3jOyX66x2jxPooimPbuA5tGjcNH"  // line TOKEN
#define BLYNK_TEMPLATE_ID "TMPL60gq2PKfY"
#define BLYNK_TEMPLATE_NAME "project"
#define BLYNK_AUTH_TOKEN "6Q-mELAV88m2elElQ6uE5juNqSU4rmBP"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <BlynkSimpleEsp8266.h>
// Your WiFi credentials
// Set password to "" for open networks.
char ssid[] = "Poomsaeroy";
char pass[] = "0123456789";
BlynkTimer timer;
int sensorValue;

WidgetLCD lcd(V5);

//  

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED() {
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V100, "offImageUrl", "https://staticimage.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V100, "onImageUrl", "https://staticimage.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V100, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstartdevice-was-made");
}
// This function sends Arduino's uptime every second to Virtual Pin 2.
// void ReadSW()
// {
//  uint8_t d = digitalRead(SW1);
//  Blynk.virtualWrite(V3,d);
// }

void Readsensor() {
  sensorValue = analogRead(WaterSensor);
  Serial.print("WaterSensor = ");
  Serial.println(sensorValue);
  if (sensorValue > 540) {  // red
    digitalWrite(R_LED, HIGH);
    Blynk.virtualWrite(V0, 1);
    digitalWrite(Y_LED, LOW);
    Blynk.virtualWrite(V3, 0);
    digitalWrite(G_LED, LOW);
    Blynk.virtualWrite(V1, 0);
    digitalWrite(Buzzer, LOW);
    digitalWrite(Motor, HIGH);
    Line_Notify("น้ำมาละจ้า");
    lcd.clear();
    lcd.print(3,0,"Water level");
    lcd.print(6,1,"Warning!!");
   
    
  } else if (sensorValue > 400 && sensorValue <= 530) {  // YELLOW
    digitalWrite(Y_LED, HIGH);
    Blynk.virtualWrite(V3, 1);
    digitalWrite(R_LED, LOW);
    Blynk.virtualWrite(V0, 0);
    digitalWrite(G_LED, LOW);
    Blynk.virtualWrite(V1, 0);
    digitalWrite(Buzzer, HIGH);
    digitalWrite(Motor, LOW);
    lcd.clear();
    lcd.print(3,0,"Water level");
    lcd.print(5,1,"Becareful");

  } else if (sensorValue > 0 && sensorValue <= 340) { //340
    digitalWrite(G_LED, HIGH);  // Green
    Blynk.virtualWrite(V1, 1);
    digitalWrite(R_LED, LOW);
    Blynk.virtualWrite(V0, 0);
    digitalWrite(Y_LED, LOW);
    Blynk.virtualWrite(V3, 0);
    digitalWrite(Buzzer, HIGH);
    digitalWrite(Motor, LOW);
    // Blynk.virtualWrite(V5,"123");
    lcd.clear();
    lcd.print(3,0,"Water level");
    lcd.print(6,1,"NORMAL");
  }
  delay(500);
}

int start_time = millis();

void setup() {
  pinMode(Y_LED, OUTPUT);
  pinMode(G_LED, OUTPUT);
  pinMode(R_LED, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(Motor, OUTPUT);
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  
}
void loop() {
  Blynk.run();
  Readsensor();
  timer.run();
  thingspeak_notify(sensorValue);
  
  // int end_time = millis();
  // if (end_time - start_time > 600000) {
  //   start_time = millis();
  //   Serial.println("Sleeping");
  //   ESP.deepSleep(SECONDS_DS(60));
  // }
}

void Line_Notify(String message) {
  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {

    WiFiClient client;
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    http.begin(client, "http://172.20.10.2:3000/line");  // HTTP
    http.addHeader("Content-Type", "application/json");

    Serial.print("[HTTP] POST...\n");
    // start connection and send HTTP header and body
    int httpCode = http.POST("{\"text\": \"" + String(message) + "\"}");
  
    Serial.println(httpCode);

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
}

void thingspeak_notify(int water) {
  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {

    WiFiClient client;
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    http.begin(client, "http://172.20.10.2:3000/water");  // HTTP
    http.addHeader("Content-Type", "application/json");

    Serial.print("[HTTP] POST...\n");
    // start connection and send HTTP header and body
    int httpCode = http.POST("{\"text\": \"" + String(water) + "\"}");
  
    Serial.println(httpCode);

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
}