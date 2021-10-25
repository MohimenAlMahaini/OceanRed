/*
 * This class handels Valid rxEnOcean -> IRtx.
 * Author: Mohimen Al Mahaini
 */

#ifndef enocean_ir_h
#define enocean_ir_h

#include <Arduino.h>
#include <TransmitIR.h>
#include <FilesAndData.h>

/* This Function sends IR signal when entry is saved without an EnOcean Signal. */
void sendIR(int entryId)
{
  for (int i = 0; i < MAX_ALLOWED_ENTRIES; i++)
  {
    if (entryId == entry[i].entryId)
    {
      // call IR Transmit Function.
      transmitTX(entry[i].irRawData);
    }
  }
}

/*  This function take a byte Array (size of 4) & databyte, and search for the same enocean signal in the JsonArray in RAM
*   If a match is found the corresponding IR signal will be sent.
*/
void compareRxWithRAMEntries(byte senderID[], byte dataByte)
{
  // Prepare String for comparison
  String compString;
  compString += "0x";
  for (int jj = 0; jj < 4; jj++)
  {
    compString += String(senderID[jj], HEX);
    if (senderID[jj] == 0)
    {
      compString += "0";
    }
  }
  compString += String(dataByte, HEX); // compString = "0xfefa27c5f0"

  Serial.println("Comparative String = " + compString); // FOR DEBUG
  // Iterate throw all entries
  for (int j = 0; j < MAX_ALLOWED_ENTRIES; j++)
  {
    // if entry[j].irRawData  is emtpy or if an Entry does not contain an Enocean signal no need to do anything just skip.
    if (entry[j].irRawData.equals("") || entry[j].enoceanSignal.equals("") || entry[j].entryId < 0)
    {
      continue;
    }
    char *pch;
    String enoceanSignal = entry[j].enoceanSignal;                           // enoceanSignal = 0xfefa27c5f#
    Serial.println("EnOcean Signal @ " + String(j) + " = " + enoceanSignal); // FOR DEBUG
    // Prepare enoceanSignal for comparison
    // Prepare the character array (the buffer) , Length (with one extra character for the null terminator)
    char str[enoceanSignal.length() + 1];
    // Copy it over
    enoceanSignal.toCharArray(str, enoceanSignal.length() + 1);
    pch = strtok(str, "#");
    while (pch != NULL)
    {
      //   // Serial.printf("pch = %s\n", pch); // FOR DEBUG
      if (compString.equals(pch))
      {
        /*Get the irRawData and send it*/
        Serial.print("a Match for this signal has been found Sending entryID = ");
        Serial.println(entry[j].entryId);
        transmitTX(entry[j].irRawData);
      }
      pch = strtok(NULL, "#");
    }
    // Serial.printf("pch = %s\n", pch); // FOR DEBUG
  }
}

#endif