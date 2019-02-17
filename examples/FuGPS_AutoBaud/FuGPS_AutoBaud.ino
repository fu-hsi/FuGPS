/**
  Auto Baud Rate Detect Example.
  Working and tested on WeMos D1 ESP-Wroom-02 ESP8266.

  Connection:
     D4 (TX) => Debug output
     D7 (RX) => GPS (TX)
     D8 (TX) => GPS (RX)
         GND => GND
    3V3 - 5V => VCC

  Sample output:
    Booting Sketch...
    Automatic baud rate detection...
    9600...OK!
*/

#include <FuGPS.h>

#define GPS Serial
#define DBG Serial1

FuGPS fuGPS(GPS);

int detectBaud()
{
    DBG.println(F("Automatic baud rate detection..."));

    int baudRates[] = { 9600 /* default - first for faster detect */, 4800, 14400, 19200, 38400, 57600, 115200 };
    int count = sizeof(baudRates) / sizeof(baudRates[0]);
    
    for (int i = 0; i < count; i++)
    {
        DBG.print(baudRates[i]);
        DBG.print("...");

        GPS.end();
        GPS.begin(baudRates[i]);
        GPS.swap();

        unsigned long timeNow = millis();

        while (millis() - timeNow < 10000)
        {
            if (fuGPS.read())
            {
                DBG.println(F("OK!"));
                return baudRates[i];
            }
            yield(); // Watchdog Timer Reset 
        }
    }
    return 0;
}

void setup()
{
    DBG.begin(38400);

    DBG.println();
    DBG.println("Booting Sketch...");

    int baud = detectBaud();
    while (baud == 0)
    {
        DBG.println("GPS not detected, retrying...");
    }
}

void loop()
{
    // Do what you want...
}
