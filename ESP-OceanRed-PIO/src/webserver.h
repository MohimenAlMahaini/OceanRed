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

#define PORT 81

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
      int trackerPlusI = tracker + i;
      if (trackerPlusI > sizeOfArray - 1)
      {
        trackerPlusI = 0;
      }
      if (!first)
      {
        jsonString += ", ";
      }
      first = false;
      jsonString += "\"";
      jsonString += recivedHexStringBuffer[trackerPlusI];
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
  // uint tracker = indexPtr;
  uint tracker = index_ptr_enocean;
  boolean first = true;

  for (int i = 0; i < sizeOfEnoceanArray; i++)
  {
    if (enOceanArray[sizeOfEnoceanArray - 1] != 0) // Array is full
    {
      int trackerPlusI = tracker + i;
      if (trackerPlusI > sizeOfArray - 1)
      {
        trackerPlusI = 0;
      }
      if (!first)
      {
        oceanString += ",";
      }
      first = false;
      oceanString += "{\"type\":\"";
      tmp = enOceanArray[trackerPlusI];
      uint ind = tmp.indexOf('#');
      uint ind0 = tmp.indexOf('*');
      oceanString += tmp.substring(0, ind);
      oceanString += "\",\"senderID\":\"";
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
  // Serial.println(oceanString); FOR DEBUG
  return oceanString;
}

/*After reboot check if entry.json file is not empty and send saved config to ng for displaying the data*/
void loadConfig()
{
}

/*This function handels incoming URL requests From the configuration webpage.*/
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
                 //  startEnOceanTeachIn = true;
                 setStartEnOceanTeachIn(true);
                 Serial.print("START ENOCEAN TEACHIN! .....");
                 request->send(204);
               });

  webServer.on("/stop-enocean-teachin", HTTP_GET, [](AsyncWebServerRequest *request)
               {
                 //  startEnOceanTeachIn = false;
                 setStartEnOceanTeachIn(false);
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
                 if (getStartEnOceanTeachIn())
                 {
                   request->send(200, "application/json", getEnoceanJson());
                   for (int j = 0; j < sizeOfEnoceanArray; j++) // Clear Array
                   {
                     enOceanArray[j] = "";
                   }
                 }
                 else
                 {
                   request->send(500, "text", "Internal Server Error!");
                 }
               });

  /* ### Entry manipulation ### */

  // return SerilizedEntries from RAM
  webServer.on("/getEntries", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(200, "application/json", serializedEntries()); });

  webServer.on(
      "/testIR", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
      {
        String postRequestBody;
        for (size_t i = 0; i < len; i++)
        {
          postRequestBody += ((char)data[i]);
        }
        Serial.println("RECIVED TESTIR REQUEST, POST REQUEST BODY = " + postRequestBody);
        StaticJsonDocument<1000> entry; // in Stack
        DeserializationError error = deserializeJson(entry, postRequestBody);
        if (error)
        {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
          request->send(400, "text", error.f_str());
          return;
        }
        const char *irRawData = entry["IRRawData"];
        Serial.print("Sending IR Signal = ");
        Serial.println(irRawData);
        transmitTX(irRawData);
        request->send(200);
      });

  webServer.on(
      "/addEntry", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
      {
        // get Post Request Body
        String postRequestBody;
        for (size_t i = 0; i < len; i++)
        {
          postRequestBody += ((char)data[i]);
        }
        Serial.println("RECIVED POST REQUEST, POST REQUEST BODY = " + postRequestBody);
        // get Info from body
        StaticJsonDocument<500> entry; // in Stack
        DeserializationError error = deserializeJson(entry, postRequestBody);
        if (error)
        {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
          request->send(400, "text", error.f_str());
          return;
        }
        const char *functionName = entry["functionName"];
        const char *irRawData = entry["irRawData"];
        if (functionName != NULL && irRawData != NULL)
        {
          // add Entry and return the EntryID
          int entryId = addEntry(functionName, irRawData);
          String jsonEntryId = "{ \"id\":" + String(entryId) + "}";
          request->send(201, "application/json", jsonEntryId);
        }
        else
        {
          request->send(400);
        }
        entry.clear();
      });

  webServer.on(
      "/addEnocean", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
      {
        Serial.print("ADD ENOCEAN REQUEST ....");
        // Read Post Request Body
        String postRequestBody;
        for (size_t i = 0; i < len; i++)
        {
          postRequestBody += ((char)data[i]);
        }
        Serial.println("Recived = " + postRequestBody);
        StaticJsonDocument<300> enEntry;
        DeserializationError error = deserializeJson(enEntry, postRequestBody);
        if (error)
        {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
          request->send(400, "text", error.f_str());
          return;
        }
        const int id = enEntry["id"];
        const char *enOcean = enEntry["enocean"];
        Serial.print("ADDING ENOCEAN $1 = ");
        // Serial.print(id);
        Serial.print(" $2 = ");
        Serial.println(enOcean);
        addEnocean(int(id), enOcean);

        request->send(200);
        enEntry.clear();
      });

  webServer.on("/removeEntry", HTTP_DELETE, [](AsyncWebServerRequest *request)
               {
                 String entryToDelete;
                 if (request->hasParam(ENTRY_ID))
                 {
                   entryToDelete = request->getParam(ENTRY_ID)->value();

                   Serial.println("entryToDelete = " + entryToDelete);

                   if (!entryToDelete.toInt())
                   {
                     Serial.println("NOT VALID DELETE REQUEST FOR REMOVING ENTRY");
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

  webServer.on("/removeEnocean", HTTP_DELETE, [](AsyncWebServerRequest *request)
               {
                 if (request->hasParam(ENTRY_ID) && request->hasParam(ENTRY_ENOCEAN))
                 {
                   Serial.println("REQUEST HAS BOTH REQUIERD PARAMETERS");
                   String enocean = request->getParam(ENTRY_ENOCEAN)->value();
                   String id = request->getParam(ENTRY_ID)->value();
                   Serial.println("removeEnocean recived, $1=");
                   Serial.println(enocean);
                   Serial.println(" $2=");
                   Serial.println(id);
                   if (enocean.isEmpty() || id.toInt() <= 0)
                   {
                     request->send(400);
                     return;
                   }
                   Serial.println("REMOVING AN ENOCEAN SIGNAL");
                   removeEnocean(id.toInt(), enocean);
                   request->send(204);
                   return;
                 }
                 request->send(406);
               });

  webServer.on("/save", HTTP_GET, [](AsyncWebServerRequest *request)
               {
                 saveToSPIFFS();
                 request->send(200);
               });

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

/* @ DEPRECATED
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
                 */