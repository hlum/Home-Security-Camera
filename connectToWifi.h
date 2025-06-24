#ifndef CONNECT_TO_WIFI_H
#define CONNECT_TO_WIFI_H



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

#endif