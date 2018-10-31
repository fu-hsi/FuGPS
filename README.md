# FuGPS Library

It is a simple Arduino library for parse NMEA sentences (e.g. GPS/GLONASS/GALILEO) from serial port or any other object derived from Stream class.  
I wrote only a few basic functions for the simplicity of use.  

## Why another library?
I wrote it for fun, learning and for my next project (meteorological balloon).

## Installation
This library is available on Arduino Library Manager (should be after some time).  
If not, just download project from GitHub and put (after extraction) into Arduino library folder.  
Library was written in Visual Micro, but can be compiled also on Arduino IDE.

## Setup
```cpp
#include <NeoSWSerial.h>
#include <FuGPS.h>

NeoSWSerial in(8, 9);
FuGPS fuGPS(in);

void setup()
{
    Serial.begin(38400);
    in.begin(9600);

    // fuGPS.sendCommand(FUGPS_PMTK_SET_NMEA_BAUDRATE_9600);
    // fuGPS.sendCommand(FUGPS_PMTK_SET_NMEA_UPDATERATE_1HZ);
    fuGPS.sendCommand(FUGPS_PMTK_API_SET_NMEA_OUTPUT_RMCGGA);
}
```
## Example 1
Good for testing purpose.

```cpp
void loop()
{
    // Valid NMEA message
    if (fuGPS.read())
    {
        byte tokenIdx = 0;
        while (const char * token = fuGPS.getField(tokenIdx++))
        {
            Serial.print("Token [" + String(tokenIdx) + "]: ");
            Serial.println(token);
        }
        Serial.println();
    }
}
```

## Example 2
Typical usage.

```cpp
bool gpsAlive = false;

void loop()
{
    // Valid NMEA message
    if (fuGPS.read())
    {
        // We don't know, which message was came first (GGA or RMC).
        // Thats why some fields may be empty.

        gpsAlive = true;

        Serial.print("Quality: ");
        Serial.println(fuGPS.Quality);

        Serial.print("Satellites: ");
        Serial.println(fuGPS.Satellites);

        if (fuGPS.hasFix() == true)
        {
            // Data from GGA message
            Serial.print("Accuracy (HDOP): ");
            Serial.println(fuGPS.Accuracy);

            Serial.print("Altitude (above sea level): ");
            Serial.println(fuGPS.Altitude);

            // Data from GGA or RMC
            Serial.print("Location (decimal degrees): ");
            Serial.println("https://www.google.com/maps/search/?api=1&query=" + String(fuGPS.Latitude, 6) + "," + String(fuGPS.Longitude, 6));
        }
    }

    // Default is 10 seconds
    if (fuGPS.isAlive() == false)
    {
        if (gpsAlive == true)
        {
            gpsAlive = false;
            Serial.println("GPS module not responding with valid data.");
            Serial.println("Check wiring or restart.");
        }
    }
}
```

## Example 3
Suitable only with HardwareSerial (not tested).
```cpp
void loop()
{
    // Valid NMEA message
    if (fuGPS.isValid())
    {
        // Rest is the same...
    }
}

// SerialEvent occurs whenever a new data comes in the hardware serial RX.
void serialEvent() {
    fuGPS.read();
}
```

## Possible problems

##### Dropping frames (messages)

Please, don't use *SoftwareSerial* if you must capture more than one type of message, it is very ineffective.  
You can experiment with setting proper input and output baud rate, but the best solution to me is to use better implementations of Serial:

- **HardwareSerial (always the best way).**
- NeoSWSerial (ESP8266 not supported).
- AltSoftSerial (pins 8 & 9 for a Nano, ESP8266 not supported).

## Compatibility
I tested on that I had, it means GY-NEO6MV2 and below modules.

| MCU                                                 | Works | Notes
|---                                                  |:---:  |:--
| Arduino Nano w/ ATmega328P                          | YES   |
| Arduino Pro or Pro Mini w/ ATmega328P (3.3V, 8 MHz) | YES   |
| Arduino Pro or Pro Mini w/ ATmega328P (5V, 16 MHz)  | YES   |
| ESP8266                                             | YES   | WeMos D1 mini.<br>I had to implemenent my own strsep() function.
| ESP32                                               | ?     | Waiting for shipment from AliExpress ;-)

## Developing
I would rather not complicate the library by adding too many functionalities.  
There are exists many other complex libraries and everyone can find something suitable for themselves.  
I cared about simplicity and stability.

## Debugging
There are two macros:

```cpp
DPRINT(...)
DPRINTLN(...)
```

To use them, uncomment:

```cpp
#define FUGPS_DEBUG
```

We don't need GPS module for testing.
My library needs Stream as input, so we need some object implementing this class.  
I found this:
https://github.com/paulo-raca/ArduinoBufferedStreams.

Now, code might look like this:

```cpp
#include <LoopbackStream.h>
#include <FuGPS.h>

LoopbackStream in;
FuGPS fuGPS(in);

const char * gpsStream = 
    "$GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,*76\r\n"
    "$GPGSA,A,3,10,07,05,02,29,04,08,13,,,,,1.72,1.03,1.38*0A\r\n"
    "$GPGSV,3,1,11,10,63,137,17,07,61,098,15,05,59,290,20,08,54,157,30*70\r\n"
    "$GPGSV,3,2,11,02,39,223,19,13,28,070,17,26,23,252,,04,14,186,14*79\r\n"
    "$GPGSV,3,3,11,29,09,301,24,16,09,020,,36,,,*76\r\n"
    "$GPRMC,092750.000,A,5321.6802,N,00630.3372,W,0.02,31.66,280511,,,A*43\r\n"
    "$GPGGA,092751.000,5321.6802,N,00630.3371,W,1,8,1.03,61.7,M,55.3,M,,*75\r\n"
    "$GPGSA,A,3,10,07,05,02,29,04,08,13,,,,,1.72,1.03,1.38*0A\r\n"
    "$GPGSV,3,1,11,10,63,137,17,07,61,098,15,05,59,290,20,08,54,157,30*70\r\n"
    "$GPGSV,3,2,11,02,39,223,16,13,28,070,17,26,23,252,,04,14,186,15*77\r\n"
    "$GPGSV,3,3,11,29,09,301,24,16,09,020,,36,,,*76\r\n"
    "$GPRMC,092751.000,A,5321.6802,N,00630.3371,W,0.06,31.66,280511,,,A*45\r\n";

void setup()
{
    Serial.begin(38400);
    Serial.println("Setup");
}

void loop()
{
    const char * pStart = gpsStream;
    
    while (*pStart)
    {
        in.write(*pStart++);
        if (fuGPS.read())
        {
            byte tokenIdx = 0;
            while (const char * token = fuGPS.getField(tokenIdx++))
            {
                Serial.print("Token [" + String(tokenIdx) + "]: ");
                Serial.println(token);
            }
            Serial.println();
        }
    }
}
```
