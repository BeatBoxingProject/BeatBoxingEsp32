#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// --- WiFi Credentials (Same for both) ---
#define WIFI_SSID  "SSID"
#define WIFI_PASS  "PASS"

// ============================================================
// --- Dynamic Configuration (Injected by platformio.ini) ---
// ============================================================

#define USE_MDNS true
// If platformio.ini doesn't provide a name, use this default
#ifndef HOSTNAME
  #define HOSTNAME "esp32-cam-default"
#endif

// We will inject just the LAST number of the IP (The "Octet")
// e.g., 192.168.137.XXX
#ifndef IP_LAST_OCTET
  #define IP_LAST_OCTET 100 // Default backup
#endif

#define USE_STATIC_IP true
// We construct the full IP using the injected octet
#define STATIC_IP_ADDR  192, 168, 137, IP_LAST_OCTET
#define GATEWAY_ADDR    192, 168, 137, 1
#define SUBNET_ADDR     255, 255, 255, 0
#define DNS1_ADDR       8, 8, 8, 8
#define DNS2_ADDR       1, 1, 1, 1

// --- Camera Settings ---
#define JPEG_QUALITY    12 
#define LED_GPIO_NUM    2 

#endif