#include <Arduino.h>
#include <connectToWiFi.h>
#include <webserver.h>

#define BAUD_RATE 115200

void setup()
{
  Serial.begin(BAUD_RATE);
  Serial.println("Starting ....");
  if (!SPIFFS.begin())
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  connectWiFi();
  startWebServer();
  listFiles();
}

void loop()
{
}