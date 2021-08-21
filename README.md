# ESP32 LoRa GATEWAY

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
