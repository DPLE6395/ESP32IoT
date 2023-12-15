#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32httpUpdate.h>

#define FIRMWARE_VERSION 1.3
#define UPDATE_URL "https://dple6395.github.io/firmware.json"

// #define relayPin01 32
// #define ledRedPin 25
#define ledRedPin 26

String wifiSSID = "DPleMi";
String wifiPassword = "1234567890";

void connectWifi();
void checkUpdate();
void ledBlink(void *pvParameters);
// t_httpUpdate_return updateFirmware(String url_update);
t_httpUpdate_return updateOverHttp(String url_update);

TaskHandle_t task0;

void setup() {
  Serial.begin(115200);
  pinMode(ledRedPin, OUTPUT);
  digitalWrite(ledRedPin, HIGH);

  connectWifi();
  xTaskCreatePinnedToCore(
    ledBlink,
    "checkUpdate",
    10000,
    NULL,
    1,&task0,
    0
  );
}

void loop() {
  checkUpdate();
  delay(5000);
}

void ledBlink(void *pvParameters) {
  while (1) {
    digitalWrite(ledRedPin, LOW);
    delay(500);
    digitalWrite(ledRedPin, HIGH);
    delay(500);
  }
}


void checkUpdate() {
  Serial.println("Checking Update");
  HTTPClient http;
  String response;
  String url = UPDATE_URL;
  http.begin(url);
  http.GET();

  response =http.getString();
  Serial.println(response);

  StaticJsonDocument<1024> doc;
  deserializeJson(doc, response);
  JsonObject obj = doc.as<JsonObject>();

  String version = obj[String("version")];
  String url_update = obj[String("url")];

  Serial.println("=> Version: " + version + "\n=> url_update:" + url_update);

  if(version.toDouble()> FIRMWARE_VERSION) {
    Serial.println("=> Update Available \n");
    if (updateOverHttp(url_update) == HTTP_UPDATE_OK) {
    // if(updateFirmware(url_update) == HTTP_UPDATE_OK) {
      Serial.println("Update Success");
    }
    else {
      Serial.println("Update Failed");
    }
  }
  else {
    Serial.println("=> No Update Available \n");
  }
}

// t_httpUpdate_return updateFirmware(String url_update) {
t_httpUpdate_return updateOverHttp(String url_update) {
  t_httpUpdate_return ret;
  if(WiFi.status() == WL_CONNECTED) {
    ret = ESPhttpUpdate.update(url_update);
    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.println("[update] Update failed.");
        return ret;
        break;
      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("[update] Update no Update.");
        return ret;
        break;
      case HTTP_UPDATE_OK:
        Serial.println("[update] Update ok.");
        return ret;
        break;
      default:
        break;
    }
  }
}

void connectWifi() {
  Serial.println("Connecting To Wifi");
  WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
  }
  Serial.println("Wifi Connected");
  Serial.println(WiFi.SSID());
  Serial.println(WiFi.RSSI());
  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.gatewayIP());
  Serial.println(WiFi.dnsIP());
}