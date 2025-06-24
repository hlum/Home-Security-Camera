#include<WiFi.h>
#include<HTTPClient.h>
#include<secrets.h>
#include<connectToWifi.h>
#include<getDistance.h>
#include<uploadImage.h>

const char* placeholderImageURL = "http://fastly.picsum.photos/id/851/200/200.jpg?hmac=JVRP-bj1-hofsGmrxkRZ4VaDr699PvCv6i8zcc6n-GQ";
const char* uploadEndPoint = "https://24cm0138.main.jp/esp32/upload.php";

const String endPoint = "https://24cm0138.main.jp/esp32/detector.php";
HTTPClient http;

const int sigPin = 14;

void setup() {
  Serial.begin(115200);
  connectToWifi(ssidAtHome, passwordAtHome);

  int imageLength = 0;
  uint8_t* imageData = downloadImage(placeholderImageURL, &imageLength);

  if (imageData != nullptr) {
    postImageToServer(uploadEndPoint, imageData, imageLength);
    free(imageData);
  } else {
    Serial.println("❌ Image download failed");
  }

}

void loop() {
  // float distance = getDistance(sigPin);
  // Serial.println(distance);
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
