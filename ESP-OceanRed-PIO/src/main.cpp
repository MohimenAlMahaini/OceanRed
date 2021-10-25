#include <Arduino.h>
#include <connectToWiFi.h>
#include <webserver.h>
#include <TransmitIR.h>
#include <EnOceanIR.h>
#include <FilesAndData.h>

#define BAUD_RATE 115200

#define button 34
#define LED 5

void setup()
{
  Serial.begin(BAUD_RATE);
  Serial.println("Starting ....");
  if (!SPIFFS.begin())
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    delay(1000);
    return;
  }
  connectWiFi();

  setupUART();

  initIRReciver();

  // /*ENOCEAN TASK*/
  xTaskCreate(
      vEnOceanTask,  // Function Name
      "EnOceanTask", // Task Name
      4000,          // Stack Size
      NULL,          // Task Parameters,
      1,             // Task Priority
      NULL           // Task Handle
  );

  startWebServer();
  listAllFiles();

  pinMode(button, INPUT);
  pinMode(LED, OUTPUT);
  initTx();
  readEntries(); // loads saved config on boot
}

void loop()
{
  dump();
}