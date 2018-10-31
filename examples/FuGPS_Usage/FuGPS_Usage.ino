#include <NeoSWSerial.h>
#include <FuGPS.h>

NeoSWSerial in(8, 9);
FuGPS fuGPS(in);
bool gpsAlive = false;

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
