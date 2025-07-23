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


#define LED_PIN 2

#define BOOT_BUTTON_PIN 0


#define PIR_PIN 32      // PIR sensor connected to GPIO 32

unsigned long motionTimeout = 5000;   // Time to keep output on (ms)
unsigned long motionStartTime = 0;    // When motion was last detected

bool detectionEnabled = true;


bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;


void setup() {
  Serial.begin(115200);
  connectToWifi(ssidSchool, passwordSchool);

  // Add server connectivity test
  testServerConnection();
  
  pinMode(PIR_PIN, INPUT);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP); // BOOT is active LOW

  // Initialize the camera pin
  if (!initCamera()) {
    Serial.println("❌ Failed to initialize camera. Halting.");
    while (true); // Halt
  }
  Serial.println("Camera initialized.");

  delay(1000); // Wait for sensor to stabilize
}



void loop() {
  // // Read the button
  int buttonState = digitalRead(BOOT_BUTTON_PIN);

  // Detect falling edge with debounce
  if (buttonState == LOW && lastButtonState == HIGH && (millis() - lastDebounceTime > debounceDelay)) {
    detectionEnabled = !detectionEnabled;
    lastDebounceTime = millis();

    Serial.print("Detection toggled: ");
    Serial.println(detectionEnabled ? "ON" : "OFF");
  }

  lastButtonState = buttonState;

  if (detectionEnabled) {
    if (motionDetected()) {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("🚶 Person detected!");
      captureAndUpload();
    }
  }

  delay(100); // keep loop responsive
  digitalWrite(LED_PIN, LOW);
}


bool motionDetected() {
  int motionDetected = digitalRead(PIR_PIN);

  if (motionDetected == HIGH) {
    if (millis() - motionStartTime >= motionTimeout) {
      motionStartTime = millis();  // Record the time motion started
      return true;
    }
  }

  return false;
}


void captureAndUpload() {
  size_t imageLength = 0;
  uint8_t* imageData = captureImage(&imageLength);

  if (imageData != nullptr) {
    Serial.println("Image captured successfully.");
    Serial.printf("📦 Image size: %d bytes\n", imageLength);
    postImageToServerWithRetry(uploadEndPoint, imageData, imageLength, 5);
    free(imageData);
  } else {
    Serial.println("❌ Image capture failed");
  }
}

void testServerConnection() {
  Serial.println("Testing server connectivity...");
  
  // Test basic HTTP connection
  WiFiClient testClient;
  if (testClient.connect("24cm0138.main.jp", 80)) {
    Serial.println("✅ HTTP (port 80) connection successful");
    testClient.stop();
  } else {
    Serial.println("❌ HTTP (port 80) connection failed");
  }
  
  // Test HTTPS connection
  WiFiClientSecure httpsClient;
  httpsClient.setInsecure();
  if (httpsClient.connect("24cm0138.main.jp", 443)) {
    Serial.println("✅ HTTPS (port 443) connection successful");
    httpsClient.stop();
  } else {
    Serial.println("❌ HTTPS (port 443) connection failed");
  }
}



