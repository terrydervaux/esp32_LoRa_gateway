# ESP32 LoRa GATEWAY

## Hardware

The project has been developed on **heltec wifi lora 32 V2** board. The board
includes the following components:

- LoRa Modem SemTech SX1276
- OLED display SSD1306 0.96-inch 128x64

The components are connected according to the following
[pinout diagram](https://resource.heltec.cn/download/WiFi_LoRa_32/WIFI_LoRa_32_V2.pdf).

## Setup

- Download VScode
- Install platformIO extension on VScode
- Create a file ``src/wifi_secrets.h`` containing

```cpp
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"
```

- Create a file ``src/HA_secrets.h`` containing

```cpp
#define HA_ENDPOINT "your_ha_endpoint"
#define HA_API_PORT "your_ha_api_port"
#define HA_BEARER_TOKEN "your_ha_bearer_token"
```

- Compile and upload
