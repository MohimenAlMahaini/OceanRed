#ifndef recive_ir_h
#define recive_ir_h

/*TODO:
*  + Encapsulate this class in a task.
*/

#include <Arduino.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRac.h>
#include <IRtext.h>

uint16_t RECV_PIN = 13;

String recivedHexStringBuffer[20]; // Array to hold recived signals as a Hex String

const uint16_t kCaptureBufferSize = 1024;

const uint8_t kTimeout = 15; // this is good enough for AC need longer Timeouts.

/** Set higher if you get lots of random short UNKNOWN messages when nothing
* should be sending a message.
* Set lower if you are sure your setup is working, but it doesn't see messages
* from your device. */
const uint16_t kMinUnknownSize = 12;
const uint8_t kTolerancePercentage = kTolerance; // 25%

IRrecv irrecv(RECV_PIN, kCaptureBufferSize, kTimeout, true);

decode_results results;

void initIRReciver() //TODO: change function name
{
    irrecv.setUnknownThreshold(kMinUnknownSize);
    irrecv.setTolerance(kTolerancePercentage); // Override the default tolerance.
    irrecv.enableIRIn();
}

void addRecivedIRtoTransportArray(String rawData)
{
    //TODO: Parse incoming rawData before inserting it in Array
    // if array is not full
    uint16_t sizeOfArray = sizeof(recivedHexStringBuffer) / sizeof(recivedHexStringBuffer[0]);
    if (recivedHexStringBuffer[sizeOfArray - 1] == 0) // 0 or ""
    {
        for (int i = 0; i < sizeOfArray; i++)
        {
            if (recivedHexStringBuffer[i] == 0)
            {
                recivedHexStringBuffer[i] = rawData;
            }
        }
    }

    // If Array is full -> push the array 1 place overwritting the oldest entry
    if (recivedHexStringBuffer[sizeOfArray - 1] != 0)
    {
        for (int ii = 0; ii < sizeOfArray - 1; ii++)
        {
            recivedHexStringBuffer[ii] = recivedHexStringBuffer[ii + 1];
        }
        recivedHexStringBuffer[sizeOfArray - 1] == rawData;
    }
}

void dump()
{
    if (irrecv.decode(&results))
    {
        // Display a crude timestamp.
        uint32_t now = millis();
        Serial.printf(D_STR_TIMESTAMP " : %06u.%03u\n", now / 1000, now % 1000);
        // Check if we got an IR message that was to big for our capture buffer.
        if (results.overflow)
            Serial.printf(D_WARN_BUFFERFULL "\n", kCaptureBufferSize);
        // Display the library version the message was captured with.
        Serial.println(D_STR_LIBRARY "   : v" _IRREMOTEESP8266_VERSION_ "\n");
        // Display the tolerance percentage if it has been change from the default.
        if (kTolerancePercentage != kTolerance)
            Serial.printf(D_STR_TOLERANCE " : %d%%\n", kTolerancePercentage);
        // Display the basic output of what we found.
        Serial.print(resultToHumanReadableBasic(&results));
        // Display any extra A/C info if we have it.
        String description = IRAcUtils::resultAcToString(&results);
        if (description.length())
            Serial.println(D_STR_MESGDESC ": " + description);
        yield(); // Feed the WDT as the text output can take a while to print.
        Serial.println(resultToSourceCode(&results));
        Serial.println(); // Blank line between entries
        yield();          // Feed the WDT (again)
        addRecivedIRtoTransportArray(resultToSourceCode(&results));
    }
}

#endif // recive_ir_h
