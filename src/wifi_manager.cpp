#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include "wifi_manager.h"
#include "config.h"

// Internal helper to print status
void printConnectionInfo(bool isReconnect) {
  Serial.println("\n---------------------------------");
  Serial.println(isReconnect ? "WiFi Reconnected!" : "WiFi Connected & Server Started!");
  Serial.println("---------------------------------");
  Serial.printf("  Stream URL: http://%s/stream?framesize=10&quality=12\n", WiFi.localIP().toString().c_str());
  #if USE_MDNS
  Serial.printf("  mDNS URL:   http://%s.local/stream?framesize=10&quality=12\n", HOSTNAME);
  #endif
  Serial.println("---------------------------------");
}

// Event: Disconnected
void WiFiEvent_OnDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("\n[WiFi] Disconnected! Auto-reconnecting...");
  #if USE_MDNS
  MDNS.end();
  #endif
}

// Event: Connected
void WiFiEvent_OnGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  #if USE_MDNS
  if (MDNS.begin(HOSTNAME)) {
    MDNS.addService("http", "tcp", 80);
  }
  #endif
  printConnectionInfo(true);
}

void setupWiFi() {
  // 1. Register Events
  WiFi.onEvent(WiFiEvent_OnGotIP, ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(WiFiEvent_OnDisconnected, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  WiFi.mode(WIFI_STA);
  WiFi.persistent(false);
  WiFi.setAutoReconnect(true);

  // 2. Configure Static IP (if enabled)
  #if USE_STATIC_IP
  if (!WiFi.config(IPAddress(STATIC_IP_ADDR), IPAddress(GATEWAY_ADDR), IPAddress(SUBNET_ADDR), IPAddress(DNS1_ADDR), IPAddress(DNS2_ADDR))) {
    Serial.println("[WiFi] Failed to configure Static IP!");
  }
  #endif

  // 3. Connect
  Serial.printf("[WiFi] Connecting to %s", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  // Wait for connection (optional, acts as a blocking start)
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}