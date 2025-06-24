#include<WiFi.h>
#include<HTTPClient.h>
#include<secrets.h>
#include<connectToWifi.h>

const String endPoint = "https://24cm0138.main.jp/esp32/detector.php";
HTTPClient http;

void setup() {
  Serial.begin(115200);
  connectToWifi(ssidAtHome, passwordAtHome);
}

void loop() {
  uploadData();
  delay(5000); 
}


void connectToWifi(const char* ssid, const char* password) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wifi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.print("Wifi Connected to : ");
  Serial.println(WiFi.localIP());
}

void uploadData() {
  http.begin(endPoint);
  http.addHeader("Authorization", API_KEY);

  int httpCode = http.GET();
  
  String payload = http.getString();
  Serial.println(payload);

  http.end();
}
