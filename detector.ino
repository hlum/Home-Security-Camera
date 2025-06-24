#include<WiFi.h>
#include<HTTPClient.h>
#include<secrets.h>
#include<connectToWifi.h>
#include<getDistance.h>


const String endPoint = "https://24cm0138.main.jp/esp32/detector.php";
HTTPClient http;

const int sigPin = 14;

void setup() {
  Serial.begin(115200);
  connectToWifi(ssidAtHome, passwordAtHome);
}

void loop() {
  float distance = getDistance(sigPin);
  Serial.println(distance);
  delay(500); 
}

void uploadData() {
  http.begin(endPoint);
  http.addHeader("Authorization", API_KEY);

  int httpCode = http.GET();
  
  String payload = http.getString();
  Serial.println(payload);

  http.end();
}
