#ifndef webserver_h
#define webserver_h

/**
 * This class serves the Angular configuration website, sends & handels Requests to & from ng application
 * Author: Mohimen Al Mahaini
 * */

#include <Arduino.h>
#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#include <IRReceiver.h>
#include <EnOcean.h>
#include <FilesAndData.h>

#define PORT 80

const char *ENTRY_ID = "entryId";
const char *ENTRY_NAME = "name";
const char *ENTRY_IR = "ir";
const char *ENTRY_ENOCEAN = "enocean";

AsyncWebServer webServer(PORT);

/*This function parse the IR Array to Json and return a String*/
String getIRJsonString()
{
    String jsonString = "{ \"signals\": [";
    uint tracker = indexPtr;
    boolean first = true;
    for (int i = 0; i < sizeOfArray; i++)
    {
        if (recivedHexStringBuffer[sizeOfArray - 1] != 0) // Array is full
        {
            if (tracker + i > sizeOfArray - 1)
            {
                tracker = 0;
            }
            if (!first)
            {
                jsonString += ", ";
            }
            first = false;
            jsonString += "\"";
            jsonString += recivedHexStringBuffer[tracker + i];
            jsonString += "\"";
        }
        else // array is not full
        {
            if (recivedHexStringBuffer[i] != 0) // if it's not empty
            {
                if (!first)
                {
                    jsonString += ",";
                }
                first = false;
                jsonString += "\"";
                jsonString += recivedHexStringBuffer[i];
                jsonString += "\"";
            }
        }
    }

    jsonString += "]}";
    return jsonString;
}

/*This function parse the Enocean Array to Json and returns it as a String*/
String getEnoceanJson()
{
    String oceanString = "{ \"telegrams\" :[ ";
    String tmp;
    uint tracker = indexPtr;
    boolean first = true;

    for (int i = 0; i < sizeOfEnoceanArray; i++)
    {

        if (enOceanArray[sizeOfEnoceanArray - 1] != 0) // Array is full
        {
            if (tracker + i > sizeOfArray - 1)
            {
                tracker = 0;
            }
            if (!first)
            {
                oceanString += ",";
            }
            first = false;
            oceanString += "{\"type\":\"";
            tmp = enOceanArray[tracker + i];
            uint ind = tmp.indexOf('#');
            uint ind0 = tmp.indexOf('*');
            oceanString += tmp.substring(0, ind);
            oceanString += "\",\"senderID\":\"";
            // TODO
            // oceanString += tmp.substring(ind, ind + 3);
            oceanString += tmp.substring(ind + 3, ind0);
            oceanString += "\",\"payload\":\"";
            oceanString += tmp.substring(ind0 + 1, tmp.length() - 1);
            oceanString += "\"}";
        }
        else // Array is not full
        {
            if (enOceanArray[i] != 0) // check that its not empty
            {
                if (!first)
                {
                    oceanString += ",";
                }
                first = false;
                tmp = enOceanArray[i];
                oceanString += "{\"type\":\"";
                uint ind = tmp.indexOf('#');
                uint ind0 = tmp.indexOf('*');
                oceanString += tmp.substring(0, ind);
                oceanString += "\",\"senderID\":\"";
                // TODO
                // oceanString += tmp.substring(ind, ind + 3);
                oceanString += tmp.substring(ind + 1, ind0);
                oceanString += "\",\"payload\":\"";
                oceanString += tmp.substring(ind0 + 1, tmp.length() - 1);
                oceanString += "\"}";
            }
        }
    }
    // String = { "telegram" : [ { "type:"ftke","senderID":"0xffffff"}, {"type":"rocker","senderID":"0xff0aac"}]}
    oceanString += "]}";
    Serial.println(oceanString);
    return oceanString;
}

/*After reboot check if entry.json file is not empty and send saved config to ng for displaying the data*/
void loadConfig()
{
}

/*
*
*/
void requestHandel()
{
    /** ### Teachin ### **/
    webServer.on("/start-ir-teachin", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
                     startIRTeachin = true;
                     request->send(204);
                 });

    webServer.on("/stop-ir-teachin", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
                     startIRTeachin = false;
                     request->send(204);
                 });

    webServer.on("/start-enocean-teachin", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
                     startEnOceanTeachIn = true;
                     Serial.print("START ENOCEAN TEACHIN! .....");
                     request->send(204);
                 });

    webServer.on("/stop-enocean-teachin", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
                     startEnOceanTeachIn = false;
                     request->send(204);
                 });

    webServer.on("/get-ir", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
                     // ADD TIME STAMP for the recived Request

                     Serial.println("Recived a Get IR Request ...");
                     request->send(200, "application/json", getIRJsonString());
                     for (int x = 0; x < sizeof(recivedHexStringBuffer) / sizeof(recivedHexStringBuffer[0]); x++) // clear Array
                     {
                         recivedHexStringBuffer[x] = "";
                     }
                 });

    webServer.on("/get-eo", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
                     // ADD TIME STAMP for the recived Request
                     request->send(200, "application/json", getEnoceanJson());
                     for (int j = 0; j < sizeOfEnoceanArray; j++) // Empty Array
                     {
                         enOceanArray[j] = "";
                     }
                 });

    /* ### Entry manipulation ### */
    webServer.on("/removeEntry", HTTP_POST, [](AsyncWebServerRequest *request)
                 {
                     String entryToDelete;
                     if (request->hasParam(ENTRY_ID))
                     {
                         entryToDelete = request->getParam(ENTRY_ID)->value();

                         Serial.println("entryToDelete = " + entryToDelete);

                         if (!entryToDelete.toInt())
                         {
                             Serial.println("NOT VALID POST REQUEST FOR REMOVING ENTRY");
                             request->send(418); // I'm a teapot
                         }
                         else
                         {
                             removeEntry(entryToDelete.toInt());
                             request->send(200);
                         }
                     }
                     else
                     {
                         request->send(400);
                     }
                 });

    webServer.on("/addEntry", HTTP_POST, [](AsyncWebServerRequest *request)
                 {
                     String irRawData;
                     String entryEnOcean;
                     String functionName;

                     if (request->hasParam(ENTRY_NAME) && request->hasParam(ENTRY_IR))
                     {
                         functionName = request->getParam(ENTRY_NAME)->value();
                         irRawData = request->getParam(ENTRY_IR)->value();

                         if (request->hasParam(ENTRY_ENOCEAN))
                         {
                             entryEnOcean = request->getParam(ENTRY_ENOCEAN)->value();
                         }

                         if (!entryEnOcean.isEmpty())
                         {
                             addEntry(irRawData, entryEnOcean, functionName);
                             request->send(201);
                         }
                         else
                         {
                             addEntry(irRawData, functionName);
                             request->send(201);
                         }
                     }
                     else
                     {
                         request->send(400);
                     }
                 });

    webServer.on("/testIR", HTTP_POST, [](AsyncWebServerRequest *request)
                 {
                     if (request->hasParam(ENTRY_IR))
                     {
                         Serial.println("SENDING AN IR TEST SIGNAL ...");
                         transmitTX(request->getParam(ENTRY_IR)->value());
                         request->send(202);
                     }
                     else
                     {
                         request->send(400);
                     }
                 });

    webServer.on("/save", HTTP_POST, [](AsyncWebServerRequest *request)
                 {
                     saveToSPIFFS();
                     request->send(200);
                 });

    webServer.on("/editEntry", HTTP_POST, [](AsyncWebServerRequest *request)
                 {
                     String entryID;
                     String irRawData;
                     String entryEnOcean;
                     String functionName;
                     if (request->hasParam(ENTRY_ID) && request->hasParam(ENTRY_IR) && request->hasParam(ENTRY_NAME))
                     {
                         entryID = request->getParam(ENTRY_ID)->value();
                         irRawData = request->getParam(ENTRY_IR)->value();
                         functionName = request->getParam(ENTRY_NAME)->value();
                         if (request->hasParam(ENTRY_ENOCEAN))
                         {
                             entryEnOcean = request->getParam(ENTRY_ENOCEAN)->value();
                             editEntry(entryID.toInt(), irRawData, entryEnOcean, functionName);
                             request->send(200);
                         }
                         else
                         {
                             editEntry(entryID.toInt(), irRawData, functionName);
                             request->send(200);
                         }
                     }
                     else
                     {
                         request->send(400);
                     }
                 });

    webServer.on("/getEntries", HTTP_GET, [](AsyncWebServerRequest *request)
                 { request->send(SPIFFS, "/entry.json", "application/json"); });

    webServer.on("/restart", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
                     request->send(200);
                     ESP.restart();
                     delay(3);
                 });
}

void notFound(AsyncWebServerRequest *request)
{
    request->send(404, "application/json", "{\"message\":\"Not found\"}");
}

/*  This function serves the webpages & calls on the Request handlers.
*   This function gets called in the main.setup().
*/
void startWebServer()
{

    webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                 { request->send(200, "text/html", (const char *)index_html_start); });

    webServer.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request)
                 { request->send(200, "text/html", (const char *)index_html_start); });

    webServer.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
                     AsyncWebServerResponse *response = request->beginResponse_P(200, "image/ico", icon, icon_end - icon);

                     request->send(response);
                 });

    webServer.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
                     AsyncWebServerResponse *response = request->beginResponse_P(200, "text/css", styles_css, styles_css_end - styles_css);
                     response->addHeader("Content-Encoding", "gzip");
                     request->send(response);
                 });

    webServer.on("/runtime.js", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
                     AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", runtime_js, runtime_js_end - runtime_js);

                     response->addHeader("Content-Encoding", "gzip");
                     request->send(response);
                 });

    webServer.on("/polyfills.js", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
                     AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", polyfills_js, polyfills_js_end - polyfills_js);
                     response->addHeader("Content-Encoding", "gzip");
                     request->send(response);
                 });
    webServer.on("/main.js", HTTP_GET, [](AsyncWebServerRequest *request)
                 {
                     AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", main_js, main_js_end - main_js);
                     response->addHeader("Content-Encoding", "gzip");
                     request->send(response);
                 });

    webServer.onNotFound(notFound);
    requestHandel();
    webServer.begin();
}

#endif