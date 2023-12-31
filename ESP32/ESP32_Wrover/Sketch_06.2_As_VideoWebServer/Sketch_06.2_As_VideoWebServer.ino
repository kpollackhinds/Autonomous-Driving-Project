#include "esp_camera.h"
#include "secrets.h"
#include <WiFi.h>
//
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//

// Select camera model
#define CAMERA_MODEL_WROVER_KIT // Has PSRAM
#include "camera_pins.h"

//BELOW IS ONLY FOR REFERENCE//
// typedef enum {
//     FRAMESIZE_96X96,    // 96x96
//     FRAMESIZE_QQVGA,    // 160x120
//     FRAMESIZE_QCIF,     // 176x144
//     FRAMESIZE_HQVGA,    // 240x176
//     FRAMESIZE_240X240,  // 240x240
//     FRAMESIZE_QVGA,     // 320x240
//     FRAMESIZE_CIF,      // 400x296
//     FRAMESIZE_HVGA,     // 480x320
//     FRAMESIZE_VGA,      // 640x480
//     FRAMESIZE_SVGA,     // 800x600
//     FRAMESIZE_XGA,      // 1024x768
//     FRAMESIZE_HD,       // 1280x720
//     FRAMESIZE_SXGA,     // 1280x1024
//     FRAMESIZE_UXGA,     // 1600x1200
//     // 3MP Sensors
//     FRAMESIZE_FHD,      // 1920x1080
//     FRAMESIZE_P_HD,     //  720x1280
//     FRAMESIZE_P_3MP,    //  864x1536
//     FRAMESIZE_QXGA,     // 2048x1536
//     // 5MP Sensors
//     FRAMESIZE_QHD,      // 2560x1440
//     FRAMESIZE_WQXGA,    // 2560x1600
//     FRAMESIZE_P_FHD,    // 1080x1920
//     FRAMESIZE_QSXGA,    // 2560x1920
//     FRAMESIZE_INVALID
// } framesize_t;

// defined in secrets.h file
// const char* ssid     = "********";   //input your wifi name
// const char* password = "********";   //input your wifi passwords

void startCameraServer();

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if(psramFound()){
    // config.frame_size = FRAMESIZE_UXGA;
    // config.frame_size = FRAMESIZE_HVGA;
    // config.frame_size = FRAMESIZE_QVGA;
    config.frame_size = FRAMESIZE_240X240;

;
    config.jpeg_quality = 10;
    // config.jpeg_quality = 15;

    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 30;
    config.fb_count = 1;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  // drop down frame size for higher initial frame rate
  // s->set_framesize(s, FRAMESIZE_VGA);
    s->set_framesize(s, FRAMESIZE_240X240);


  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10000);
}