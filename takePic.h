#ifndef TAKE_PIC
#define TAKE_PIC_H

#include<esp_camera.h>

// === CAMERA PIN CONFIGURATION ===
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     21
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       19
#define Y4_GPIO_NUM       18
#define Y3_GPIO_NUM       5
#define Y2_GPIO_NUM       4
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// === CAMERA INIT ===
bool initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; // Use JPEG format

  // Frame size options: FRAMESIZE_QVGA, FRAMESIZE_VGA, etc.
  config.frame_size = FRAMESIZE_VGA;
  config.jpeg_quality = 10;  // Lower is better quality
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return false;
  }
  return true;
}

// === CAPTURE IMAGE METHOD ===
uint8_t* captureImage(size_t* lengthOut) {
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return nullptr;
  }

  // Set output length
  if (lengthOut != nullptr) {
    *lengthOut = fb->len;
  }

  uint8_t* copy = (uint8_t*)malloc(fb->len);
  if (!copy) {
    Serial.println("Failed to allocate memory for image copy");
    esp_camera_fb_return(fb);
    return nullptr;
  }

  memcpy(copy, fb->buf, fb->len);
  esp_camera_fb_return(fb);  // Free original frame buffer

  return copy;  // You must free this in the calling code
}

#endif
