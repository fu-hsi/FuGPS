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
