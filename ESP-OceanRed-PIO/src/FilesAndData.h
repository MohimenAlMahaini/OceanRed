#ifndef files_and_data_h
#define files_and_data_h
/**
 * This class handels files & files Data, it also implement some of the backend functions.
 * Author: Mohimen Al Mahaini
 * */

#include <ArduinoJson.h>
#include <SPIFFS.h>
#include "FS.h"

#define MAX_ALLOWED_ENTRIES 50

/* Files for Angular*/

/*Embedding files as binaries in ESP32 uses less flash space*/
extern const uint8_t index_html_start[] asm("_binary_www_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_www_index_html_end");
extern const uint8_t main_js[] asm("_binary_www_main_js_gz_start");
extern const uint8_t main_js_end[] asm("_binary_www_main_js_gz_end");
extern const uint8_t polyfills_js[] asm("_binary_www_polyfills_js_gz_start");
extern const uint8_t polyfills_js_end[] asm("_binary_www_polyfills_js_gz_end");
extern const uint8_t runtime_js[] asm("_binary_www_runtime_js_gz_start");
extern const uint8_t runtime_js_end[] asm("_binary_www_runtime_js_gz_end");
extern const uint8_t styles_css[] asm("_binary_www_styles_css_gz_start");
extern const uint8_t styles_css_end[] asm("_binary_www_styles_css_gz_end");
extern const uint8_t icon[] asm("_binary_www_favicon_ico_start");
extern const uint8_t icon_end[] asm("_binary_www_favicon_ico_end");

/* SPIFFS FILES*/

String readFile(fs::FS &fs, const char *path)
{
    File file = fs.open(path, "r");
    if (!file || file.isDirectory())
    {
        Serial.println("NOT A FILE OR IS A DIR");
        return String();
    }
    String fileContent;
    while (file.available())
    {
        fileContent += String((char)file.read());
    }
    return fileContent;
}

void writeFile(fs::FS &fs, const char *path, const char *message)
{
    Serial.printf("Writing file: %s\r\n", path);
    File file = fs.open(path, "w");
    if (!file)
    {
        Serial.println("- failed to open file for writing");
        return;
    }
    if (file.print(message))
    {
        Serial.println("- file written");
    }
    else
    {
        Serial.println("- write failed");
    }
}

void listAllFiles()
{
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    Serial.print("\n");
    while (file)
    {
        Serial.print("FILE: ");
        Serial.println(file.name());
        file = root.openNextFile();
    }
}

void removeFiles()
{
    SPIFFS.remove("/entry.json");
}

/* ### JSON, RAM, SPIFFS  ### */
/**
 * Data mapping can be done using 3 tables with refrences (Embedded Database).
 * learned Enocean telegrams, learned IR Signal, and functionName.
 * the reason why data mapping is not implemented this way is because 
 * implementing it this way reduces flexibility and increases complexity down the line which makes the code harder to maintain.
 *
 */

struct JsonEntry
{
    int entryId;
    String irRawData;
    String enoceanSignal;
    String functionName;
};

JsonEntry entry[MAX_ALLOWED_ENTRIES]; // Create JsonEntry array with the maxed_allowed_size

/*This function saves the entries from SPIFFS to RAM (faster than calling addEntry())*/
void saveEntriesToRAM(int entryId, String irRawData, String enoceanSignal, String functionName)
{
    entry[entryId - 1].entryId = entryId;
    entry[entryId - 1].irRawData = irRawData;
    entry[entryId - 1].enoceanSignal = enoceanSignal;
    entry[entryId - 1].functionName = functionName;
}

/*This function clears Entries from RAM (sets them to 0, entry is a global variable and it can not be freed)*/
void clearEntriesFromRAM()
{
    memset(entry, 0, sizeof(entry));
}

int capacity = 3000;
/*This function allocate DynamicJsonDocument and it checks if the memory size is enough if not it makes it bigger and it prints the entry.json to Serial*/
void readEntries()
{
    DynamicJsonDocument doc(capacity); // Capacity can change at run-time (HEAP ALLOCATION)
    String entryString = readFile(SPIFFS, "/entry.json");
    Serial.print("SAVED TO SPIFFS = ");
    Serial.println(entryString);
    Serial.println();
    DeserializationError error = deserializeJson(doc, entryString);

    // Test if parsing succeeded.
    if (error == DeserializationError::NoMemory)
    {
        capacity += 500;
        Serial.println("Json Document too small increasing the capacity ...");
        doc.clear();
        readEntries();
        return;
    }
    else if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }
    Serial.println("READING ENTRY.JSON");
    Serial.print("SPIFFS TOTAL Bytes = ");
    Serial.println(SPIFFS.totalBytes());
    Serial.print("SPIFFS TOTAL USED Bytes = ");
    Serial.println(SPIFFS.usedBytes());

    // Get a reference to the root array
    JsonArray arr = doc.as<JsonArray>();

    // Walk the JsonArray efficiently  See 3.7.4 deserialization_tutorial6
    for (JsonObject obj : arr)
    {
        const int entryid = obj["entryid"];
        const char *IR = obj["IR"];
        const char *enoceanID = obj["enoceanID"];
        const char *functionName = obj["functionName"];

        saveEntriesToRAM(entryid, IR, enoceanID, functionName);
    }
}

/*This function save the entries from RAM to SPIFFS, When the user saves a new entry it should be saved so that we can serve it after a reboot*/
void saveToSPIFFS()
{
    /*ArduinoJson Memory leaks
    * Replacing and removing values produce a memory leak inside the
    * JsonDocument.
    * In practice, this problem only happens in programs that use a JsonDocument
    * to store the application’s state, which is not the purpose of ArduinoJson.
    * the sole purpose of ArduinoJson is to serialize and deserialize
    * JSON documents. Thats why I'm not using ArduinoJson for Serialization.
    */

    Serial.println("SAVING TO SPIFFS . . . ");

    boolean isFirst = true;
    String data = "[";
    for (int i = 0; i < MAX_ALLOWED_ENTRIES; i++)
    {
        // Check if entry is NOT empty
        if (entry[i].entryId != NULL)
        {
            if (!isFirst)
            {
                data += ",";
            }
            data += "{";
            data += "\"entryid\":";
            data += String(entry[i].entryId);
            data += ",\"IR\": \"";
            data += entry[i].irRawData;
            data += "\",\"enoceanID\": \"";
            data += entry[i].enoceanSignal;
            data += "\", \"functionName\":\"";
            data += entry[i].functionName;
            data += "\"}";
            isFirst = false;
        }
    }

    data += "]";
    Serial.print("DATA TO SAVE = ");
    Serial.println(data);
    writeFile(SPIFFS, "/entry.json", data.c_str());
}

/*This function return an availableID EntryID */
int getAValidEntryID()
{
    for (int i = 0; i < MAX_ALLOWED_ENTRIES; i++)
    {
        if (entry[i].irRawData.equals("") || entry[i].entryId == NULL)
        {
            return i + 1; // this entry has been deleted before or its empty, you can overwrite it.
        }
    }
    Serial.print("No More valid Entry IDs ...");
    return -1;
}

/*This function adds entry to RAM*/
void addEntry(String irRawData, String enoceanSignal, String functionName)
{
    Serial.println("ADDING NEW ENTRY");
    int availableID = getAValidEntryID();
    if (availableID != -1) // check if there is still availableIDs
    {
        entry[availableID - 1].entryId = availableID;
        entry[availableID - 1].irRawData = irRawData;
        entry[availableID - 1].enoceanSignal = enoceanSignal;
        entry[availableID - 1].functionName = functionName;
    }
}

/*This function adds entry (without EnOcean Signal) to RAM*/
void addEntry(String irRawData, String functionName)
{
    Serial.println("ADDING NEW ENTRY WITHOUT ENOCEAN");

    int availableID = getAValidEntryID();

    if (availableID != -1) // check if there is still availableIDs
    {
        entry[availableID - 1].entryId = availableID;
        entry[availableID - 1].irRawData = irRawData;
        entry[availableID - 1].functionName = functionName;
    }
}

/*This function is used to edit an Entry*/
void editEntry(int id, String ir, String enoceanData, String functionName)
{
    // check if id is valid, this check does not cost much computing, and id <= 0 should never return true.
    if (id <= 0)
    {
        Serial.print("UNVALID ID!");
        return;
    }
    // no need to check if id exists -> saves computing time, and under normal usage the user can edit only existing entries.
    if (entry[id - 1].entryId == id)
    {
        entry[id - 1].irRawData = ir;
        entry[id - 1].enoceanSignal = enoceanData;
        entry[id - 1].functionName = functionName;
    }
}

void editEntry(int id, String ir, String functionName)
{
    // check if id is valid, this check does not cost much computing, and id <= 0 should never return true.
    if (id <= 0)
    {
        Serial.print("UNVALID ID!");
        return;
    }
    // no need to check if id exists -> saves computing time, and under normal usage the user can edit only existing entries.
    if (entry[id - 1].entryId == id)
    {
        entry[id - 1].irRawData = ir;
        entry[id - 1].functionName = functionName;
    }
}

/*This function Removes entry from RAM*/
void removeEntry(int id)
{
    // no need to check if entry exist because under normal usage Angular can only delete existing Entries.

    // unvalid id
    if (id <= 0)
    {
        return;
    }
    // Make sure that it is so.
    if (entry[id - 1].entryId == id)
    {
        entry[id - 1].entryId = NULL;
        entry[id - 1].enoceanSignal = "";
        entry[id - 1].functionName = "";
        entry[id - 1].irRawData = "";
    }

    Serial.print("REMOVED ENTRY NR. " + String(id));
}

#endif /*files_h*/