#include <Arduino.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "config.h"
#include "camera_init.h"
#include "wifi_manager.h"
#include "stream_server.h"

void setup() {
  // 1. Disable Brownout Detector 
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("\n--- ESP32 Stream Server Booting ---");

  // 2. Initialize WiFi FIRST
  // We do this before the camera interrupts start firing.
  setupWiFi();
  
  // 3. Initialize Camera SECOND
  // Now that WiFi is stable, we can start the heavy camera DMA/Interrupts
  setupCamera();

  // 4. Start Web Server
  startStreamServer();
}

void loop() {
  delay(10000); // Idle
}