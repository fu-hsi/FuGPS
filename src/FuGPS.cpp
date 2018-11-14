/**
 * FuGPS Library for Arduino
 * Copyright (c) 2018 Mariusz Kacki
 * The MIT License (MIT)
 * https://github.com/fu-hsi/fugps
 */

#include "FuGPS.h"

#ifdef FUGPS_DEBUG
unsigned int FuGPS::gga_counter = 0;
unsigned int FuGPS::rmc_counter = 0;
#endif

FuGPS::FuGPS(Stream & _stream) :
    _stream(_stream), _state(0), _tokensCount(0), _fix(false), _lastRead(0),
    Quality(0), Satellites(0), Accuracy(0), Altitude(0), Latitude(0), Longitude(0), Speed(0), Course(0)
{
}

byte FuGPS::checksum(const char * sentence)
{
    byte checksum = 0;
    while (*sentence)
    {
        checksum ^= *sentence++;
    }
    return checksum;
}

void FuGPS::parseDateTime(float value, byte & val1, byte & val2, byte & val3)
{
    val1 = (int)(value / 10000);
    value -= val1 * 10000.0;

    val2 = (int)(value / 100);
    value -= val2 * 100.0;

    val3 = (int)value;
}

float FuGPS::toDecimal(float coordinate, char coordinateRef)
{
    float decimal = (int)(coordinate / 100) + ((coordinate - ((int)(coordinate / 100) * 100)) / 60);
    if (coordinateRef == 'S' || coordinateRef == 'W')
    {
        return -decimal;
    }
    else
    {
        return decimal;
    }
}

void FuGPS::sendCommand(const char * command)
{
    _stream.println(command);
}

const char * FuGPS::getMessageId()
{
    if (_tokensCount > 0)
    {
        return _tokens[0];
    }
    else
    {
        return nullptr;
    }
}

const char * FuGPS::getSentenceId()
{
    if (_tokensCount > 0 && strlen(_tokens[0]) == 5)
    {
        return _tokens[0] + 2;
    }
    else
    {
        return nullptr;
    }
}

const char * FuGPS::getField(byte index)
{
    if (index < _tokensCount)
    {
        return _tokens[index];
    }   
    else
    {
        return nullptr;
    }    
}

bool FuGPS::hasFix()
{
    return _fix;
}

bool FuGPS::isAlive(unsigned int timeout)
{
    if (_lastRead == 0)
    {
        return false;
    }
    else
    {
        unsigned long timeDiff = millis() - _lastRead;
        return timeDiff <= timeout;
    }
}

bool FuGPS::isValid()
{
    return _tokensCount;
}

bool FuGPS::read()
{
    static byte idx = 0;
    if (_stream.available())
    {
        char ch = _stream.read();
        _currentBuff[idx++] = ch;

        switch (ch)
        {
        case '$':
            _currentBuff[idx = 0] = '\0';
            _state = 0;
            break;

        case '*':
        case '\r':
            _currentBuff[--idx] = '\0';
            _state ^= ch;
            break;

        case '\n':
        {
            DPRINT(F("State: "));
            DPRINTLN(_state);

            _currentBuff[--idx] = '\0';
            
            // Simple validation based on checksum
            if (_state == 39)
            {
                char * pChecksum = (char *)(&_currentBuff[idx - 2]);
                unsigned int givenChecksum;
                sscanf((const char *)pChecksum, "%x", &givenChecksum);
                
                // Cut-off checksum segment
                *pChecksum = '\0';

                DPRINT(F("Checksum is "));
                if (checksum(_currentBuff) == givenChecksum)
                {
                    DPRINTLN(F("valid"));
                    strcpy(_sentenceBuff, _currentBuff);

                    process();
                    return true;
                }
                else
                {
                    DPRINTLN(F("invalid"));
                }
            }
            else
            {
                DPRINTLN(F("Invalid NMEA message (missing $, * or <CR>"));
            };

            _state = 0;
            break;
        }

        default:
            break;
        }

        if (idx >= FUGPS_NMEA_BUFFER_LENGTH)
        {
            DPRINTLN(F("Buffer overflow"));
            _currentBuff[idx--] = '\0';
        }
    }
    return false;
}

void FuGPS::process()
{
    _tokensCount = 0;
    char * pStart = _sentenceBuff;
    char * pString = _sentenceBuff;

    while (*pString != '\0')
    {
        if (*pString == ',')
        {
            *pString = '\0';
            _tokens[_tokensCount++] = pStart;
            pStart = pString + 1;
        }
        pString++;
    }

    // Count the last token
    _tokens[_tokensCount++] = pStart;
    
    /**
     * Little slower than above, but less code and more readable.
     * For our misfortune, it does not count the empty values at the end in ESP2866.
     * strtok() is considered unsafe, maybe strtok_r() will be better than my function?
     * Both were not tested on ESP.
     *
     * char * token;
     * while ((token = strsep(&pStart, ",")) != NULL)
     * {
     *     _tokens[_tokensCount++] = token;
     * }
     */

    DPRINT(F("Tokens: "));
    DPRINTLN(_tokensCount);

    // RMC - Recommended Minimum Specific GPS/Transit Data
    if (_tokensCount == 13 && strcmp(getSentenceId(), "RMC") == 0)
    {
#ifdef FUGPS_DEBUG
        rmc_counter++;
#endif
        float time = atoi(_tokens[1]);
        parseDateTime(time, Hours, Minutes, Seconds);

        _fix = *_tokens[2] == 'A';

        Latitude = atof(_tokens[3]);
        Latitude = toDecimal(Latitude, *_tokens[4]);

        Longitude = atof(_tokens[5]);
        Longitude = toDecimal(Longitude, *_tokens[6]);

        Speed = atof(_tokens[7]);
        Course = atof(_tokens[8]);

        float _date = atof(_tokens[9]);
        parseDateTime(_date, Days, Months, Years);
    }
    // GGA - Global Positioning System Fix Data
    else if (_tokensCount == 15 && strcmp(getSentenceId(), "GGA") == 0)
    {
#ifdef FUGPS_DEBUG
        gga_counter++;
#endif
        float time = atoi(_tokens[1]);
        parseDateTime(time, Hours, Minutes, Seconds);

        Latitude = atof(_tokens[2]);
        Latitude = toDecimal(Latitude, *_tokens[3]);

        Longitude = atof(_tokens[4]);
        Longitude = toDecimal(Longitude, *_tokens[5]);

        Quality = atoi(_tokens[6]);
        _fix = Quality > 0;

        Satellites = atoi(_tokens[7]);
        Accuracy = atof(_tokens[8]);
        Altitude = atof(_tokens[9]);
    }

    _lastRead = millis();

    DPRINT(F("GGA: "));
    DPRINT(gga_counter);
    DPRINT(F(", RMC: "));
    DPRINTLN(rmc_counter);
    DPRINTLN();
}
