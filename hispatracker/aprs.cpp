#include "config.h"
#include "ax25.h"
#include "aprs.h"
#include <stdio.h>
#include <stdlib.h>

#if (ARDUINO + 1) >= 100
#  include <Arduino.h>
#else

#  include <WProgram.h>

#endif

// Module functions
float meters_to_feet(float m) {
    // 10000 ft = 3048 m
    return m / 0.3048;
}

APRSPacket::APRSPacket(float _latitude,
                       float _longitude,
                       float _altitude2,
                       float _speed,
                       float _heading,
                       float _extTemp,
                       float _intTemp,
                       int32_t _pressure) {

    dtostrf(_latitude, 8, 6, this->latitude);
    this->latitude[11] = '\0';

    dtostrf(_longitude, 8, 6, this->longitude);
    this->longitude[11] = '\0';

    dtostrf(_altitude2, 3, 0, this->altitude2);
    this->altitude2[5] = '\0';

    dtostrf(_speed, 1, 2, this->speed);
    this->speed[5] = '\0';

    dtostrf(_heading, 1, 0, this->heading);
    this->heading[3] = '\0';

    dtostrf(_extTemp, 5, 2, this->extTemp);
    this->extTemp[5] = '\0';

    dtostrf(_intTemp, 5, 2, this->intTemp);
    this->intTemp[5] = '\0';

    sprintf(this->pressure, "%li", _pressure);
    this->pressure[5] = '\0';
}


void APRSPacket::writeToSD() {

    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    File myFile = SD.open("LOGL.TXT", FILE_WRITE);
    if (myFile) {

        char sep = '_';

        myFile.print(this->latitude);
        myFile.print(sep);

        myFile.print(this->longitude);
        myFile.print(sep);

        myFile.print(this->heading);
        myFile.print(sep);

        myFile.print(this->speed);
        myFile.print(sep);

        myFile.print(this->altitude2);
        myFile.print(sep);

        myFile.print(this->intTemp);
        myFile.print(sep);

        myFile.print(this->extTemp);
        myFile.print(sep);

        myFile.println(this->pressure);

        myFile.close();
    }
}

void APRSPacket::aprs_send() {
    char s1[4], s2[4], s3[5];
    for(int i = 0; i<=4; i++){
        s3[i] = EEPROM.read(i+10);
        if (i < 4) {
            s1[i] = EEPROM.read(i);
            s2[i] = EEPROM.read(i+5);
        }
    }

    const struct s_address addresses[] = {
            {*s1, 11},                           // Destination callsign
            {*s2,0},                             // Source callsign (-11 = balloon, -9 = car)
            {*s3, 1},                            // Digi1 (first digi in the chain)
    };

    byte separator = 43;

    char sep = EEPROM.read(separator);
    char temp[5];

    byte heading = 83;
    byte speed = 88;
    byte altitude = 93;
    byte internalTemp = 98;
    byte pressure = 108;
    byte externalTemp = 103;

            ax25_send_header(addresses, sizeof(addresses) / sizeof(s_address));
    ax25_send_byte(sep);                // Report w/ timestamp, no APRS messaging. $ = NMEA raw data
    ax25_send_string(this->latitude);     // Lat: 38deg and 22.20 min (.20 are NOT seconds, but 1/100th of minutes)
    ax25_send_byte(sep);                // Symbol table

    ax25_send_string(this->longitude);     // Lon: 000deg and 25.80 min
    ax25_send_byte(EEPROM.read(14));                // Symbol: O=balloon, -=QTH
    ax25_send_byte(sep);

    EEPROM.get(heading, temp);
    ax25_send_string(temp);
    ax25_send_string(this->heading);             // Course (degrees)
    ax25_send_byte(sep);

    EEPROM.get(speed, temp);
    ax25_send_string(temp);
    ax25_send_string(this->speed);             // speed (knots)
    ax25_send_byte(sep);

    EEPROM.get(altitude, temp);
    ax25_send_string(temp);                     // Altitude (feet). Goes anywhere in the comment area
    ax25_send_string(this->altitude2);
    ax25_send_byte(sep);

    EEPROM.get(internalTemp, temp);
    ax25_send_string(temp);
    ax25_send_string(this->intTemp);
    ax25_send_byte(sep);

    EEPROM.get(pressure, temp);
    ax25_send_string(temp);
    ax25_send_string(this->pressure);
    ax25_send_byte(sep);

    EEPROM.get(externalTemp, temp);
    ax25_send_string(temp);
    ax25_send_string(this->extTemp);
    ax25_send_byte(sep);

    ax25_send_footer();

    ax25_flush_frame();                 // Tell the modem to go
}

// Exported functions
// void aprs_send( char gps_time[], 
//                 char gps_aprs_lat[],
//                 char gps_aprs_lon[],
//                 double gps_course,
//                 double gps_speed,
//                 float gps_altitude,
//                 char int_temperature[],
//                 char ext_temperature[],
//                 char sensors_vin[]
//               )
// {
//   char temp[12];                   // Temperature (int/ext)
//   const struct s_address addresses[] = { 
//     {D_CALLSIGN, D_CALLSIGN_ID},  // Destination callsign
//     {S_CALLSIGN, S_CALLSIGN_ID},  // Source callsign (-11 = balloon, -9 = car)
// #ifdef DIGI_PATH1
//     {DIGI_PATH1, DIGI_PATH1_TTL}, // Digi1 (first digi in the chain)
// #endif
// #ifdef DIGI_PATH2
//     {DIGI_PATH2, DIGI_PATH2_TTL}, // Digi2 (second digi in the chain)
// #endif
//   };

//   ax25_send_header(addresses, sizeof(addresses)/sizeof(s_address));
//   ax25_send_byte('/');                // Report w/ timestamp, no APRS messaging. $ = NMEA raw data
//   ax25_send_string("021709z");     // 021709z = 2nd day of the month, 17:09 zulu (UTC/GMT)
//   ax25_send_string(gps_time);         // 170915 = 17h:09m:15s zulu (not allowed in Status Reports)
//   ax25_send_byte('h');
//   ax25_send_string(gps_aprs_lat);     // Lat: 38deg and 22.20 min (.20 are NOT seconds, but 1/100th of minutes)
//   ax25_send_byte('/');                // Symbol table
//   ax25_send_string(gps_aprs_lon);     // Lon: 000deg and 25.80 min
//   ax25_send_byte('O');                // Symbol: O=balloon, -=QTH
//   snprintf(temp, 4, "%03d", (int)(gps_course + 0.5)); 
//   ax25_send_string(temp);             // Course (degrees)
//   ax25_send_byte('/');                // and
//   snprintf(temp, 4, "%03d", (int)(gps_speed + 0.5));
//   ax25_send_string(temp);             // speed (knots)
//   ax25_send_string("/A=");            // Altitude (feet). Goes anywhere in the comment area
//   snprintf(temp, 7, "%06ld", (long)(meters_to_feet(gps_altitude) + 0.5));
//   ax25_send_string(temp);
//   ax25_send_string("/Ti=");
//   snprintf(temp, 6, "%d", int_temperature);
//   ax25_send_string(temp);
//   ax25_send_string("/Te=");
//   snprintf(temp, 6, "%d", ext_temperature);
//   ax25_send_string(temp);
//   ax25_send_string("/V=");
//   snprintf(temp, 6, "%d", sensors_vin);
//   ax25_send_string(temp);
//   ax25_send_byte(' ');
//   ax25_send_string(APRS_COMMENT);     // Comment
//   ax25_send_footer();

//   ax25_flush_frame();                 // Tell the modem to go
// }
