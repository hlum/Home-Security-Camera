#include<WiFi.h>
#include<HTTPClient.h>
#include<secrets.h>
#include<connectToWifi.h>
#include<getDistance.h>
#include<uploadImage.h>
#include<takePic.h>

const int ULTRASONIC_PIN = 33;

const char* placeholderImageURL = "http://fastly.picsum.photos/id/851/200/200.jpg?hmac=JVRP-bj1-hofsGmrxkRZ4VaDr699PvCv6i8zcc6n-GQ";
const char* uploadEndPoint = "https://24cm0138.main.jp/esp32/upload.php";

const String endPoint = "https://24cm0138.main.jp/esp32/detector.php";
HTTPClient http;

#define BOOT_BUTTON_PIN 0

float initDistance = 0;
const float detectionThreshold = 15.0; // cm
bool detectionEnabled = true;
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;


void setup() {
  Serial.begin(115200);
  connectToWifi(ssidSchool, passwordSchool);

  pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP); // BOOT is active LOW

  // Initialize the camera pin
  if (!initCamera()) {
    Serial.println("❌ Failed to initialize camera. Halting.");
    while (true); // Halt
  }
  Serial.println("Camera initialized.");

  delay(1000); // Wait for sensor to stabilize

  initDistance = getDistance(ULTRASONIC_PIN);
  Serial.print("Starting distance = ");
  Serial.println(initDistance);
}



void loop() {
  // Read the button
  int buttonState = digitalRead(BOOT_BUTTON_PIN);

  // // Detect falling edge with debounce
  // if (buttonState == LOW && lastButtonState == HIGH && (millis() - lastDebounceTime > debounceDelay)) {
  //   detectionEnabled = !detectionEnabled;
  //   lastDebounceTime = millis();

  //   Serial.print("Detection toggled: ");
  //   Serial.println(detectionEnabled ? "ON" : "OFF");
  // }

  // lastButtonState = buttonState;

  // if (detectionEnabled) {
  //   float distance = getDistance(ULTRASONIC_PIN);
  //   if (distance > 2 && distance < initDistance - detectionThreshold) {
  //     Serial.println("🚶 Person detected!");
  //     captureAndUpload();
  //     delay(5000);
  //   }
  // }

  // delay(100); // keep loop responsive

  if(buttonState == LOW && (millis() - lastDebounceTime) > debounceDelay) {
    Serial.println("Btn pressed.");
    captureAndUpload();
    lastDebounceTime = millis();
    delay(5000);
  }

  delay(100);
}


void captureAndUpload() {
  size_t imageLength = 0;
  uint8_t* imageData = captureImage(&imageLength);

  if (imageData != nullptr) {
    Serial.println("Image captured successfully.");
    postImageToServer(uploadEndPoint, imageData, imageLength);
    free(imageData);
  } else {
    Serial.println("❌ Image capture failed");
  }
}



void uploadData() {
  http.begin(endPoint);
  http.addHeader("Authorization", API_KEY);

  int httpCode = http.GET();
  
  String payload = http.getString();
  Serial.println(payload);

  http.end();
}
