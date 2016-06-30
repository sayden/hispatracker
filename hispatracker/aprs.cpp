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

APRSPacket::APRSPacket( char _datetime[7],
                        float _altitude,
                        float _speed,
                        float _heading,
                        float _intTemp,
                        int32_t _pressure) {

    strcpy(this->datetime, _datetime);

    dtostrf(_altitude, 3, 0, this->altitude);
    this->altitude[5] = '\0';

    dtostrf(_speed, 1, 2, this->speed);
    this->speed[5] = '\0';

    dtostrf(_heading, 1, 0, this->heading);
    this->heading[3] = '\0';

    dtostrf(_intTemp, 5, 2, this->intTemp);
    this->intTemp[5] = '\0';

    sprintf(this->pressure, "%li", _pressure);
    this->pressure[5] = '\0';
}


void APRSPacket::setLatitudeF(float _latitude){
    this->latitudeF = _latitude;
}
void APRSPacket::setLongitudeF(float _longitude){
    this->longitudeF = _longitude;
}


void APRSPacket::setLatitude(char _latitude[9]){
    strcpy(this->latitude, _latitude);
}
void APRSPacket::setLongitude(char _longitude[10]){
    strcpy(this->longitude, _longitude);
}

// void APRSPacket::writeToSD() {

//    // open the file. note that only one file can be open at a time,
//    // so you have to close this one before opening another.
//    File myFile = this->SD->open("LOGP.TXT", FILE_WRITE);

//    char sep = '_';

//    myFile.print(this->latitude);
//    myFile.print(sep);

//    myFile.print(this->longitude);
//    myFile.print(sep);

//    myFile.print(this->heading);
//    myFile.print(sep);

//    myFile.print(this->speed);
//    myFile.print(sep);

//    myFile.print(this->altitude);
//    myFile.print(sep);

//    myFile.print(this->intTemp);
//    myFile.print(sep);

//    myFile.println(this->pressure);

//    myFile.close();
// }

void APRSPacket::aprs_send() {

    const struct s_address addresses[] = { 
      {D_CALLSIGN, D_CALLSIGN_ID},  // Destination callsign
      {S_CALLSIGN, S_CALLSIGN_ID},  // Source callsign (-11 = balloon, -9 = car)
      {DIGI_PATH1, DIGI_PATH1_TTL}, // Digi1 (first digi in the chain)
    };

    char sep = '/';

    ax25_send_header(addresses, sizeof(addresses) / sizeof(s_address));
    ax25_send_string(this->datetime);         // 170915 = 17h:09m:15s zulu (not allowed in Status Reports)
    ax25_send_byte('h');               // Report w/ timestamp, no APRS messaging. $ = NMEA raw data
    ax25_send_string(this->latitude);     // Lat: 38deg and 22.20 min (.20 are NOT seconds, but 1/100th of minutes)             
    ax25_send_byte(sep);                // Symbol table

    ax25_send_string(this->longitude);     // Lon: 000deg and 25.80 min
    ax25_send_byte('O');                // Symbol: O=balloon, -=QTH

    ax25_send_string(this->heading);             // Course (degrees)
    ax25_send_byte(sep);
    ax25_send_string(this->speed);             // speed (knots)
    ax25_send_byte(sep);

    ax25_send_string("A=");                     // Altitude (feet). Goes anywhere in the comment area
    ax25_send_string(this->altitude);
    ax25_send_byte(sep);

    ax25_send_string("Ti=");
    ax25_send_string(this->intTemp);
    ax25_send_byte(sep);

    ax25_send_string("P=");
    ax25_send_string(this->pressure);
    ax25_send_byte(' ');

    char temp[20];
    char lat[12];
    char lon[12];
    dtostrf(this->latitudeF, 5, 8, lat);
    dtostrf(this->longitudeF, 5, 8, lon);
    sprintf(temp, "%s_%s", lat, lon);
    ax25_send_string(temp);     
    ax25_send_string(" mariocaster@gmail.com");     // Comment

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
