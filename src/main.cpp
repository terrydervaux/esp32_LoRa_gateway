#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include "wifi_secrets.h"

// Pinout Diagram: https://resource.heltec.cn/download/WiFi_LoRa_32/WIFI_LoRa_32_V2.pdf

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
WiFiMulti WiFiMulti;

// Incoming messages
String LoRaData;

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
  display.print("OLED Display Initializing...OK!");
  display.display();
  Serial.println("OLED Display Initializing OK!");
  delay(500);
}

void displayLoRaMessage(String LoRaData, int rssi)
{
  // Display informations
  display.clearDisplay();

  display.setCursor(0, 0);
  display.print("LORA RECEIVER");
  
  display.setCursor(0, 10);
  display.print("MAC:");
  display.print(WiFi.macAddress());
  
  display.setCursor(0, 20);
  display.print("IP:");
  display.print(WiFi.localIP());

  display.setCursor(0, 30);
  display.print("Received packet:");

  display.setCursor(0, 40);
  display.print(LoRaData);

  display.setCursor(0, 50);
  display.print("RSSI:");

  display.setCursor(30, 50);
  display.print(rssi);
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
  WiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Waiting for WiFi... ");
  while (WiFiMulti.run() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("MAC address: ");
  Serial.println(WiFi.macAddress());
  
  delay(500);
}

void setup()
{
  //Initial Serial
  Serial.begin(115200);

  while (!Serial)
    ;

  setupOLEDDisplay();

  display.setCursor(0, 20);
  display.println("WiFi Initializing...");
  display.display();
  setupWifi();
  display.println("OK!");
  display.display();

  display.setCursor(0, 30);
  display.println("LoRa Initializing...");
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
      LoRaData = LoRa.readString();
      Serial.print(LoRaData);
    }

    //print RSSI of packet
    int rssi = LoRa.packetRssi();
    Serial.print(" with RSSI ");
    Serial.println(rssi);

    displayLoRaMessage(LoRaData, rssi);
  }
}
