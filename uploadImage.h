#include "esp32-hal.h"
#ifndef UPLOAD_IMAGE_H
#define UPLOAD_IMAGE_H

#include <WiFiClientSecure.h>
#include<secrets.h>
#include<WiFi.h>


uint8_t* downloadImage(const char* url, int* outLength) {
  WiFiClient client;
  HTTPClient http;
  http.begin(client, url);
  http.addHeader("Authorization", API_KEY);

  int httpCode = http.GET();
  if (httpCode != 200) {
    Serial.printf("❌ Failed to GET image. HTTP code: %d\n", httpCode);
    http.end();
    return nullptr;
  }

  int length = http.getSize();
  WiFiClient* stream = http.getStreamPtr();
  uint8_t* buffer = (uint8_t*)malloc(length);

  if (!buffer) {
    Serial.println("❌ Memory allocation failed");
    http.end();
    return nullptr;
  }

  int bytesRead = 0;
  while (http.connected() && bytesRead < length) {
    if (stream->available()) {
      buffer[bytesRead++] = stream->read();
    }
  }

  http.end();
  *outLength = bytesRead;
  Serial.printf("✅ Image downloaded: %d bytes\n", bytesRead);
  return buffer;
}


void checkAndReconnectWifi() {
      // Check and reconnect WiFi if needed
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi disconnected, attempting reconnect...");
      WiFi.reconnect();
      delay(5000);
      if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi reconnect failed");
      }
    }
}



bool postImageToServer(const char* url, uint8_t* data, int length) {
  long timeoutDuration = 30000; // 30sec
  WiFiClientSecure client;
  client.setInsecure();  // WARNING: disables SSL verification – fine for dev/test
  client.setTimeout(timeoutDuration);

  HTTPClient http;
  http.setTimeout(timeoutDuration);
  http.setReuse(false);

  http.begin(client, url);  // Use secure client
  http.addHeader("Content-Type", "image/jpeg");
  http.addHeader("Content-Length", String(length));
  http.addHeader("Connection", "close");
  http.addHeader("User-Agent", "ESP32-Camera");

  int responseCode = http.POST(data, length);
  if (responseCode > 0) {
    Serial.printf("✅ Image sent! Server responded: %d\n", responseCode);
    // Serial.println(http.getString());
    http.end();
    return true;
  } else {

    Serial.printf("❌ POST failed: %i\n", responseCode);

    Serial.printf("❌ POST failed: %s\n", http.errorToString(responseCode).c_str());

    Serial.printf("❌ POST failed: %s (Free heap: %d)\n", 
                  http.errorToString(responseCode).c_str(), ESP.getFreeHeap());
    http.end();
    return false;
  }
}



bool postImageToServerWithRetry(const char* url, uint8_t* data, int length, int maxRetry = 3) {
  for(int attempt = 1; attempt <= maxRetry; attempt++) {
    Serial.printf("Upload attempt %d/%d (Free heap: %d)\n", attempt, maxRetry, ESP.getFreeHeap());

    checkAndReconnectWifi();

    if (postImageToServer(url, data, length)) {
      return true;  // Success!
    }

    if (attempt < maxRetry) {
      Serial.println("Retrying in 3 seconds...");
      delay(3000);
    }
  } // End of for loop

  Serial.println("❌ All upload attempts failed");

  return false;
}


#endif