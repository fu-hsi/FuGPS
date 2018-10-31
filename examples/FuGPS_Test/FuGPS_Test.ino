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
