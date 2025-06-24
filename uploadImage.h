#ifndef UPLOAD_IMAGE_H
#define UPLOAD_IMAGE_H

#include <WiFiClientSecure.h>


uint8_t* downloadImage(const char* url, int* outLength) {
  WiFiClient client;
  HTTPClient http;
  http.begin(client, url);

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

bool postImageToServer(const char* url, uint8_t* data, int length) {
  WiFiClientSecure client;
  client.setInsecure();  // WARNING: disables SSL verification – fine for dev/test

  HTTPClient http;
  http.begin(client, url);  // Use secure client
  http.addHeader("Content-Type", "image/jpeg");

  int responseCode = http.POST(data, length);
  if (responseCode > 0) {
    Serial.printf("✅ Image sent! Server responded: %d\n", responseCode);
    Serial.println(http.getString());
    http.end();
    return true;
  } else {
    Serial.printf("❌ POST failed: %s\n", http.errorToString(responseCode).c_str());
    http.end();
    return false;
  }
}


#endif