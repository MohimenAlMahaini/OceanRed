#ifndef webserver_h
#define webserver_h

#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include "FS.h"
#include "ESPAsyncWebServer.h"

#define PORT 80

AsyncWebServer webServer(PORT);

void startWebServer()
{

    webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                 { request->send(SPIFFS, "/www/index.html", "text/html"); });

    webServer.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request)
                 { request->send(SPIFFS, "/www/index.html", "text/html"); });

    webServer.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request)
                 { request->send(SPIFFS, "/www/favicon.ico", "text/html"); });

    webServer.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
                     AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/www/styles.css.gz", "text/css");
                     response->addHeader("Content-Encoding", "gzip");
                     request->send(response);
                 });

    webServer.on("/runtime.js", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
                     AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/www/runtime.js.gz", "application/javascript");
                     response->addHeader("Content-Encoding", "gzip");
                     request->send(response);
                 });

    webServer.on("/polyfills.js", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
                     AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/www/polyfills.js.gz", "application/javascript");
                     response->addHeader("Content-Encoding", "gzip");
                     request->send(response);
                 });
    webServer.on("/main.js", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
                     AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/www/main.js.gz", "application/javascript");
                     response->addHeader("Content-Encoding", "gzip");
                     request->send(response);
                 });

    webServer.begin();
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if (!root)
    {
        Serial.println("- failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if (levels)
            {
                listDir(fs, file.name(), levels - 1);
            }
        }
        else
        {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void listFiles()
{
    listDir(SPIFFS, "/", 0);
}

#endif