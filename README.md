# 📸 ESP32 Camera MJPEG Stream Server

This project transforms an ESP32 (with a connected camera module, e.g., OV2640) into a stable MJPEG video stream server accessible over a local network. The code is structured using the PlatformIO framework for clean separation of concerns and robust performance.

## ✨ Features

* **Dynamic Configuration:** Adjust video **resolution**, **quality**, **brightness**, and **contrast** on the fly using simple URL query parameters, even while streaming.
* **Unique Device Identity:** Uses PlatformIO environments to automatically assign unique **Hostnames** and **IP addresses** to multiple devices during a single upload.
* **Robust Connectivity:** Uses static IP addressing and WiFi event handlers for reliable, automatic network reconnection.
* **Clean Design:** Modular C++ structure using separate files for WiFi, Camera Initialization, and Server logic.

## 🛠️ Project Setup

### 1. Requirements

* **PlatformIO IDE:** VS Code with the PlatformIO extension installed.
* **Board:** ESP32 Dev Module (WROOM-32) or any PSRAM-enabled ESP32 board wired to an OV2640/OV3660 camera.
* **Wiring:** Ensure your camera module's pins match the definitions in `include/camera_pins.h`.

### 2. PlatformIO Configuration (`platformio.ini`)

This project uses the `huge_app.csv` partition scheme for maximum sketch size and specific build flags to enable PSRAM.

To upload to both cameras with unique IPs, use the configured environments:

```ini
[platformio]
default_envs = cam1, cam2
```

When you click the Upload button, it will compile and upload both configurations.

### 3. Initial Configuration (include/config.h)

Before compiling, you must set your networks defaults in `include/config.h`:

```C++

#define WIFI_SSID       "YOUR_WIFI_SSID"
#define WIFI_PASS       "YOUR_WIFI_PASSWORD"
```

## 🚀 Usage (Accessing the Stream)

The stream is always available at the `/stream` endpoint. You can optionally append query parameters to modify the stream settings instantly.

### 1. Default Access

Access the stream using the configured hostname or static IP address:

* Hostname (mDNS): `http://[HOSTNAME].local/stream` (e.g., `http://camera-left.local/stream`)
* Static IP: `http://[IP]/stream`

### 2. Dynamic Settings Control

| Parameter | Function | Value Range (Type) | Example Value |
| :--- | :--- | :--- | :--- |
| `framesize` | **Resolution** | Integer (0-13, see table below) | `10` (XGA) |
| `quality` | **JPEG Quality** | Integer (10 is best, 63 is worst) | `12` |
| `brightness` | **Brightness** | Integer (-2 to 2) | `1` |
| `contrast` | **Contrast** | Integer (-2 to 2) | `-1` |

**Example URLs:**

| Goal | URL Example |
| :--- | :--- |
| High Quality Stream (FHD) | `http://[IP]/stream?framesize=11&quality=10` |
| Low Bandwidth Stream (Fast) | `http://[IP]/stream?framesize=5&quality=40` |
| Darken and Fast Stream | `http://[IP]/stream?brightness=-2&quality=30` |

**Resolution Values (val) Reference:**

| val | Name | Resolution |
| :--- | :--- | :--- |
| **13** | **UXGA** | 1600x1200 |
| **11** | **HD** | 1280x720 |
| **10** | **XGA** | 1024x768 |
| **8** | **VGA** | 640x480 |
| **5** | **QVGA** | 320x240 |
