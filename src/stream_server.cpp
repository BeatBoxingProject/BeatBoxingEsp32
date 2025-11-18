#include <Arduino.h>
#include "esp_camera.h"
#include "esp_http_server.h"
#include "stream_server.h"

#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

/**
 * @brief Main Handler: Parses settings from URL -> Starts Stream
 * usage: http://[ip]/stream?framesize=10&quality=12
 */
static esp_err_t stream_handler(httpd_req_t *req) {
  camera_fb_t *fb = NULL;
  esp_err_t res = ESP_OK;
  char * part_buf[64];
  
  // ============================================================
  // --- 1. Parse URL Parameters (Configuration) ---
  // ============================================================
  // We check if the URL has a query string (e.g., ?framesize=10)
  // and apply settings BEFORE the stream loop starts.
  
  size_t buf_len = httpd_req_get_url_query_len(req) + 1;
  
  if (buf_len > 1) {
      char* buf = (char*)malloc(buf_len);
      if (buf) {
          if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
              char value[32] = {0,};
              sensor_t * s = esp_camera_sensor_get();

              // 1. Frame Size (Resolution)
              // e.g. ?framesize=13 (UXGA), ?framesize=5 (QVGA)
              if (httpd_query_key_value(buf, "framesize", value, sizeof(value)) == ESP_OK) {
                  int val = atoi(value);
                  // Ensure we don't switch to a resolution larger than allocated buffer
                  if (s->pixformat == PIXFORMAT_JPEG) {
                      s->set_framesize(s, (framesize_t)val);
                  }
              }

              // 2. Quality (10-63, lower is better)
              // e.g. ?quality=12
              if (httpd_query_key_value(buf, "quality", value, sizeof(value)) == ESP_OK) {
                  int val = atoi(value);
                  s->set_quality(s, val);
              }

              // 3. Brightness (-2 to 2)
              // e.g. ?brightness=1
              if (httpd_query_key_value(buf, "brightness", value, sizeof(value)) == ESP_OK) {
                  int val = atoi(value);
                  s->set_brightness(s, val);
              }

               // 4. Contrast (-2 to 2)
              if (httpd_query_key_value(buf, "contrast", value, sizeof(value)) == ESP_OK) {
                  int val = atoi(value);
                  s->set_contrast(s, val);
              }
          }
          free(buf); // Free memory immediately after parsing
      }
  }

  // ============================================================
  // --- 2. Start Stream ---
  // ============================================================

  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  if (res != ESP_OK) return res;

  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

  while (true) {
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("[Camera] Capture failed");
      res = ESP_FAIL;
    } else {
      if (res == ESP_OK) {
        size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, fb->len);
        res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        if (res == ESP_OK) {
          res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
        }
        if (res == ESP_OK) {
          res = httpd_resp_send_chunk(req, (const char *)fb->buf, fb->len);
        }
      }
      esp_camera_fb_return(fb);
    }
    
    if (res != ESP_OK) break;
  }
  return res;
}

void startStreamServer() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.stack_size = 8192; 

  httpd_handle_t server = NULL;

  if (httpd_start(&server, &config) == ESP_OK) {
    httpd_uri_t stream_uri = {
        .uri = "/stream",
        .method = HTTP_GET,
        .handler = stream_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &stream_uri);
  }
}