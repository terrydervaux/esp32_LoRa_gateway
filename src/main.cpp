#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>
#include <WiFiClientSecure.h>
#include <WiFiMulti.h>
#include "wifi_secrets.h"
#include "HAClient.h"
#include "HA_secrets.h"

const char *rootCACertificate =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\n"
    "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n"
    "DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow\n"
    "PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\n"
    "Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n"
    "AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\n"
    "rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\n"
    "OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\n"
    "xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\n"
    "7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\n"
    "aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\n"
    "HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\n"
    "SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\n"
    "ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\n"
    "AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\n"
    "R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\n"
    "JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo\n"
    "Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\n"
    "-----END CERTIFICATE-----\n";

// LoRa Module (SX1276)
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 866E6

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

// Declaration for WiFi
WiFiClientSecure client;

// Incoming messages
String LoRaData;

// Declaration for Home Assistant 
HAClient ha(&client, HA_ENDPOINT,HA_API_PORT,HA_BEARER_TOKEN);

void setupOLEDDisplay()
{
  //Reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  //Initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS, false, false))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("OLED Display...OK!");
  display.display();
  Serial.println("OLED Display...OK!");
  delay(500);
}

void displayLoRaPacket(String LoRaDevice, int rssi, boolean HA_transmitted)
{
  // Display informations
  display.clearDisplay();

  display.setCursor(0, 0);
  display.print("LORA GATEWAY");

  display.setCursor(0, 10);
  display.print("packet received from:");
  display.print(LoRaDevice);

  display.setCursor(0, 40);
  display.print("RSSI:");
  display.print(rssi);
  display.print("dBm");

  display.setCursor(0, 50);
  display.print("transmitted to HA:");  
  if(HA_transmitted) display.print("yes");
  else display.print("no");
  
  display.display();
}

void setupLoRa()
{
  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);

  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(BAND))
  {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }
  Serial.println("LoRa Initializing OK!");

  delay(500);
}

void setupWifi()
{
  // We start by connecting to a WiFi network
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Waiting for WiFi... ");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("MAC address: ");
  Serial.println(WiFi.macAddress());
  Serial.println("Add root CA Certificate");
  client.setCACert(rootCACertificate);

  delay(500);
}

void setupNTP(){
  // Not sure if WiFiClientSecure checks the validity date of the certificate. 
// Setting clock just to be sure...
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}

String decodeLoRaDevice(String LoRaData){
  return LoRaData.substring(0,LoRaData.indexOf("|"));
}

String decodeLoRaPayload(String LoRaData){
  return LoRaData.substring(LoRaData.indexOf("|")+1,LoRaData.length());
}

void setup()
{
  //Initial Serial
  Serial.begin(115200);

  while (!Serial)
    ;

  setupOLEDDisplay();

  display.setCursor(0, 10);
  display.print("WiFi...");
  display.display();
  setupWifi();
  display.print("OK!");
  display.display();

  display.setCursor(0, 20);
  display.print("NTP...");
  display.display();
  setupNTP();
  display.print("OK!");
  display.display();

  display.setCursor(0, 30);
  display.print("LoRa...");
  display.display();
  setupLoRa();
  display.print("OK!");
  display.display();
  delay(500);

  // Uncomment the next line to disable the default AGC and set LNA gain, values between 1 - 6 are supported
  // LoRa.setGain(6);

  // put the radio into receive mode
  LoRa.receive();
}

void loop()
{
  //try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    //received a packet
    Serial.print("Received packet ");

    //read packet
    while (LoRa.available())
    {
      //TODO: optimize String usage
      LoRaData = LoRa.readString();
      
      // decode packet
      int rssi = LoRa.packetRssi();
      String device = decodeLoRaDevice(LoRaData);
      String payload = decodeLoRaPayload(LoRaData);

      // print decoded packet
      Serial.print(" with RSSI ");
      Serial.println(rssi);
      Serial.print("Lora Data device: ");
      Serial.println(device);
      Serial.print("Lora Data payload: ");
      Serial.println(payload);
      
      // send to HA
      boolean HA_transmitted = ha.updateState(device,payload);
      Serial.print("Transmitted to Home Assistant: ");
      Serial.println(HA_transmitted);

      // display on screen.
      displayLoRaPacket(device, rssi, HA_transmitted);
    }
  }
}
