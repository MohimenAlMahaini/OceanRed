#ifndef webserver_h
#define webserver_h

#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include "FS.h"
#include "ESPAsyncWebServer.h"

#define PORT 80

AsyncWebServer webServer(PORT);

/*In project documentation explain why the embdeed files method have been choosen over Reading from SPIFFS*/
extern const uint8_t index_html_start[] asm("_binary_data_www_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_data_www_index_html_end");
extern const uint8_t main_js[] asm("_binary_data_www_main_js_gz_start");
extern const uint8_t main_js_end[] asm("_binary_data_www_main_js_gz_end");
extern const uint8_t polyfills_js[] asm("_binary_data_www_polyfills_js_gz_start");
extern const uint8_t polyfills_js_end[] asm("_binary_data_www_polyfills_js_gz_end");
extern const uint8_t runtime_js[] asm("_binary_data_www_runtime_js_gz_start");
extern const uint8_t runtime_js_end[] asm("_binary_data_www_runtime_js_gz_end");
extern const uint8_t styles_css[] asm("_binary_data_www_styles_css_gz_start");
extern const uint8_t styles_css_end[] asm("_binary_data_www_styles_css_gz_end");
extern const uint8_t icon[] asm("_binary_data_www_favicon_ico_start");
extern const uint8_t icon_end[] asm("_binary_data_www_favicon_ico_end");

void requestHandel()
{

    webServer.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
                     // ADD TIME STAMP for the recived Request
                     Serial.println("Recived a Get Request ...");
                     String message;
                     if (request->hasParam("start"))
                     {
                         message = request->getParam("start")->value();
                         Serial.print("Get Request has Start Teach In Parameter! ... ");
                         Serial.println(message);
                     }
                     request->send(200, "application/json", "{\"data\": \"Hello, GET: " + message + "\"}");
                 });
}

AsyncWebHandler xwebServerHandler;

void startWebServer()
{

    webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                 { request->send(200, "text/html", (const char *)index_html_start); });

    webServer.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request)
                 //  { request->send(SPIFFS, "/www/index.html", "text/html"); });
                 { request->send(200, "text/html", (const char *)index_html_start); });

    webServer.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request)
                 //  { request->send(SPIFFS, "/www/favicon.ico", "text/html"); });
                 { request->send(200, "text/html", (const char *)icon); });

    webServer.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
                     //  AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/www/styles.css.gz", "text/css");
                     AsyncWebServerResponse *response = request->beginResponse_P(200, "text/css", styles_css, styles_css_end - styles_css);
                     response->addHeader("Content-Encoding", "gzip");
                     request->send(response);
                 });

    webServer.on("/runtime.js", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
                     //  AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/www/runtime.js.gz", "application/javascript");
                     AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", runtime_js, runtime_js_end - runtime_js);

                     response->addHeader("Content-Encoding", "gzip");
                     request->send(response);
                 });

    webServer.on("/polyfills.js", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
                     //   AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/www/polyfills.js.gz", "application/javascript");
                     AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", polyfills_js, polyfills_js_end - polyfills_js);
                     response->addHeader("Content-Encoding", "gzip");
                     request->send(response);
                 });
    webServer.on("/main.js", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
                     //  AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/www/main.js.gz", "application/javascript");
                     AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", main_js, main_js_end - main_js);
                     response->addHeader("Content-Encoding", "gzip");
                     request->send(response);
                 });

    requestHandel();
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