/**
 * FuGPS Library for Arduino
 * Copyright (c) 2018 Mariusz Kacki
 * The MIT License (MIT)
 * https://github.com/fu-hsi/fugps
 */

#ifndef _FUGPS_h
#define _FUGPS_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define FUGPS_PMTK_CMD_HOT_START                "$PMTK101*32"
#define FUGPS_PMTK_CMD_WARM_START               "$PMTK102*31"
#define FUGPS_PMTK_CMD_COLD_START               "$PMTK103*30"
#define FUGPS_PMTK_CMD_FULL_COLD_START          "$PMTK104*37"

#define FUGPS_PMTK_API_SET_NMEA_OUTPUT_DEFAULT  "$PMTK314,-1*04"
#define FUGPS_PMTK_API_SET_NMEA_OUTPUT_RMCGGA   "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
#define FUGPS_PMTK_API_SET_NMEA_OUTPUT_NONE     "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"

#define FUGPS_PMTK_SET_NMEA_BAUDRATE_DEFAULT    "$PMTK251,0*28"
#define FUGPS_PMTK_SET_NMEA_BAUDRATE_4800       "$PMTK251,4800*14"
#define FUGPS_PMTK_SET_NMEA_BAUDRATE_9600       "$PMTK251,9600*17"
#define FUGPS_PMTK_SET_NMEA_BAUDRATE_14400      "$PMTK251,14400*29"
#define FUGPS_PMTK_SET_NMEA_BAUDRATE_19200      "$PMTK251,19200*22"
#define FUGPS_PMTK_SET_NMEA_BAUDRATE_38400      "$PMTK251,38400*27"
#define FUGPS_PMTK_SET_NMEA_BAUDRATE_57600      "$PMTK251,57600*2C"
#define FUGPS_PMTK_SET_NMEA_BAUDRATE_115200     "$PMTK251,115200*1F"

#define FUGPS_PMTK_SET_NMEA_UPDATERATE_1HZ      "$PMTK220,1000*1F"
#define FUGPS_PMTK_SET_NMEA_UPDATERATE_5HZ      "$PMTK220,200*2C"
#define FUGPS_PMTK_SET_NMEA_UPDATERATE_10HZ     "$PMTK220,100*2F"

// 80 characters of visible text (plus the line terminators).
#define FUGPS_NMEA_BUFFER_LENGTH 82

// 20 is more than we need for GGA and RMC
#define FUGPS_MAX_TOKENS 20

// Uncomment for debug
// #define FUGPS_DEBUG

#ifdef FUGPS_DEBUG
#define DPRINT(...)   Serial.print(__VA_ARGS__)
#define DPRINTLN(...) Serial.println(__VA_ARGS__)
#else
#define DPRINT(...)
#define DPRINTLN(...)
#endif

class FuGPS
{
private:
    Stream & _stream;

    char _currentBuff[FUGPS_NMEA_BUFFER_LENGTH + 1];
    char _sentenceBuff[FUGPS_NMEA_BUFFER_LENGTH + 1];
    char * _tokens[FUGPS_MAX_TOKENS];
    
    byte _state = 0;
    byte _tokensCount;

    bool _fix;
    unsigned long _lastRead;
    
protected:
    void process();

public:
#ifdef FUGPS_DEBUG
    static unsigned int gga_counter;
    static unsigned int rmc_counter;
#endif

    FuGPS(Stream & _stream);

    static byte checksum(const char * sentence);
    static void parseDateTime(float data, byte & val1, byte & val2, byte & val3);
    static float toDecimal(float coordinate, char coordinateRef);

    void sendCommand(const char* command);

    // Comma separated fields (Zero-based numbering)
    const char * getField(byte index);

    byte getFieldCount() const;

    // E.g. GPRMC
    const char * getMessageId();

    // E.g. RMC
    const char * getSentenceId();

    // Based on GGA and RMC messages data
    bool hasFix();

    // Checks whether module still sends valid data (no matter what message)
    bool isAlive(unsigned int timeout = 10000);

    // Checks if we have valid NMEA message (see isAlive())
    bool isValid();

    // Reads one char (non blocking)
    bool read();

    byte Hours, Minutes, Seconds;
    byte Days, Months, Years;

    // Fix Quality
    byte Quality;

    // Number of Satellites
    byte Satellites;

    // Horizontal Dilution of Precision (HDOP)
    float Accuracy;

    // Altitude above mean sea level (m or ft)
    float Altitude;

    // Latitude (decimal degrees)
    float Latitude;

    // Longitude (decimal degrees)
    float Longitude;

    // Speed in knots
    float Speed;

    // True course (Track)
    float Course;
};

#endif
