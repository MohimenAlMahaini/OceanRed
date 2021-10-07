/** This calss is DEPRECATED! USING OLDER LIB 
#ifndef recive_ir_h
#define recive_ir_h
#include <Arduino.h>
#include <IRremote.h>

int RECV_PIN = 13;
IRrecv irrecv(RECV_PIN);
// decode_results results;

void initIRReciver(boolean enable)
{
    irrecv.enableIRIn(enable);
}

void ircode(decode_results *results)
{
    // Panasonic has an Address
    if (results->decode_type == PANASONIC)
    {
        Serial.print(results->address, HEX);
        Serial.print(":");
    }

    // Print Code
    Serial.print(results->value, HEX);
}

void encoding(decode_results *results)
{
    switch (results->decode_type)
    {
    default:
    case UNKNOWN:
        Serial.print("UNKNOWN");
        break;
    case NEC:
        Serial.print("NEC");
        break;
    case SONY:
        Serial.print("SONY");
        break;
    case RC5:
        Serial.print("RC5");
        break;
    case RC6:
        Serial.print("RC6");
        break;
    case DISH:
        Serial.print("DISH");
        break;
    case SHARP:
        Serial.print("SHARP");
        break;
    case JVC:
        Serial.print("JVC");
        break;
    case SANYO:
        Serial.print("SANYO");
        break;
    case MITSUBISHI:
        Serial.print("MITSUBISHI");
        break;
    case SAMSUNG:
        Serial.print("SAMSUNG");
        break;
    case LG:
        Serial.print("LG");
        break;
    case WHYNTER:
        Serial.print("WHYNTER");
        break;
    case AIWA_RC_T501:
        Serial.print("AIWA_RC_T501");
        break;
    case PANASONIC:
        Serial.print("PANASONIC");
        break;
    case DENON:
        Serial.print("Denon");
        break;
    }
}

void dumpInfo(decode_results *results)
{
    // Check if the buffer overflowed
    if (results->overflow)
    {
        Serial.println("IR code too long. Edit IRremoteInt.h and increase RAWBUF");
        return;
    }

    // Show Encoding standard
    Serial.print("Encoding  : ");
    encoding(results);
    Serial.println("");

    // Show Code & length
    Serial.print("Code      : ");
    ircode(results);
    Serial.print(" (");
    Serial.print(results->bits, DEC);
    Serial.println(" bits)");
}

void dumpRaw(decode_results *results)
{
    // Print Raw data
    Serial.print("Timing[");
    Serial.print(results->rawlen - 1, DEC);
    Serial.println("]: ");

    for (int i = 1; i < results->rawlen; i++)
    {
        unsigned long x = results->rawbuf[i] * USECPERTICK;
        if (!(i & 1))
        { // even
            Serial.print("-");
            if (x < 1000)
                Serial.print(" ");
            if (x < 100)
                Serial.print(" ");
            Serial.print(x, DEC);
        }
        else
        { // odd
            Serial.print("     ");
            Serial.print("+");
            if (x < 1000)
                Serial.print(" ");
            if (x < 100)
                Serial.print(" ");
            Serial.print(x, DEC);
            if (i < results->rawlen - 1)
                Serial.print(", "); //',' not needed for last one
        }
        if (!(i % 8))
            Serial.println("");
    }
    Serial.println(""); // Newline
}

void dumpCode(decode_results *results)
{
    // Start declaration
    Serial.print("unsigned int  ");         // variable type
    Serial.print("rawData[");               // array name
    Serial.print(results->rawlen - 1, DEC); // array size
    Serial.print("] = {");                  // Start declaration

    // Dump data
    for (int i = 1; i < results->rawlen; i++)
    {
        Serial.print(results->rawbuf[i] * USECPERTICK, DEC);
        if (i < results->rawlen - 1)
            Serial.print(","); // ',' not needed on last one
        if (!(i & 1))
            Serial.print(" ");
    }

    // End declaration
    Serial.print("};"); //

    // Comment
    Serial.print("  // ");
    encoding(results);
    Serial.print(" ");
    ircode(results);

    // Newline
    Serial.println("");

    // Now dump "known" codes
    if (results->decode_type != UNKNOWN)
    {

        // Some protocols have an address
        if (results->decode_type == PANASONIC)
        {
            Serial.print("unsigned int  addr = 0x");
            Serial.print(results->address, HEX);
            Serial.println(";");
        }

        // All protocols have data
        Serial.print("unsigned int  data = 0x");
        Serial.print(results->value, HEX);
        Serial.println(";");
    }
}

void reciveRX()
{
    decode_results results; // Somewhere to store the results

    if (irrecv.decode(&results))
    {                       // Grab an IR code
        dumpInfo(&results); // Output the results
        dumpRaw(&results);  // Output the results in RAW format
        dumpCode(&results); // Output the results as source code
        Serial.println(""); // Blank line between entries
        irrecv.resume();    // Prepare for the next value
    }
}

void sendRecivedIRtoAngular()
{
}

void vIRRXTask(void *pvParameters)
{
    for (;;)
    {
        // if(/*SOME CONDITION)
        initIRReciver(true);
        
        // placeholder task
        reciveRX();
        vTaskDelay(3);
        // call function to run in task.
    }
}


#endif

*/