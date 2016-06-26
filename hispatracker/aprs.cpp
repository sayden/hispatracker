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
float meters_to_feet(float m)
{
  // 10000 ft = 3048 m
  return m / 0.3048;
}

// void aprs_send_header_mesage(char header[], char message[]){
//  ax25_send_header(header, sizeof(header));
//  ax25_send_byte('/');                // Report w/ timestamp, no APRS messaging. $ = NMEA raw data
//  ax25_send_string(message);
//  ax25_send_byte(' ');
//  ax25_send_footer();

//  ax25_flush_frame();
// }

// void aprs_send_raw(char message[]){
//  ax25_send_header();
//  ax25_send_string(message);
//  ax25_send_byte(' ');
//  ax25_send_footer();

//  ax25_flush_frame();
// }


// void APRSPacket::testSendAprsWithChars(HardwareSerial *hx1) {
//     APRSCharPacket packet = this->getAPRScharPacket();

//     hx1->print("LAT=");
//     hx1->print(packet.latitude);
      
//     hx1->print(" LON=");
//     hx1->print(packet.longitude);
      
//     hx1->print("  SPEED=");
//     hx1->print(packet.speed);
//     hx1->print("kmh ");
      
//     hx1->print(" ALT=");
//     hx1->print(packet.altitude);
//     hx1->print("m ");
      
//     hx1->print("ALT2=");
//     hx1->print(packet.altitude2);
//     hx1->print("m ");
      
//     hx1->print(" HEA=");
//     hx1->print(packet.heading);
//     hx1->print("deg ");
            
//     hx1->print(" TMPe=");
//     hx1->print(packet.extTemp);
//     hx1->print("c ");
      
//     hx1->print("TMPi=");
//     hx1->print(packet.intTemp);
//     hx1->print("c ");
      
//     hx1->print("PRES=");
//     hx1->print(packet.pressure);
//     hx1->print("Pa");
    
//     hx1->println("");
// }

APRSPacket::APRSPacket(float _latitude,
      float _longitude,
      float _altitude2,
      float _speed,
      float _heading,
      float _extTemp,
      float _intTemp)
{

    dtostrf(_latitude, 8, 6, this->latitude);
    dtostrf(_longitude, 8, 6, this->longitude);
    dtostrf(_altitude2, 3, 0, this->altitude2);
    dtostrf(_speed, 1, 2, this->speed);
    dtostrf(_heading, 1, 0, this->heading);
    dtostrf(_extTemp, 5, 2, this->extTemp);
    dtostrf(_intTemp, 5, 2, this->intTemp);
}


void APRSPacket::writeToSD(){
  delay(100);
  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  this->myFile = SD.open("a.txt", FILE_WRITE);
  if (this->myFile) {
    char sep = EEPROM.read(43);
  
    char temp[5];
    EEPROM.get(73, temp);
    this->myFile.print(temp);
    this->myFile.print(this->latitude);
    this->myFile.print(sep);
  
    EEPROM.get(78, temp);
    this->myFile.print(temp);
    this->myFile.print(this->longitude);
    this->myFile.print(sep);
  
    EEPROM.get(83,temp);
    this->myFile.print(temp);
    this->myFile.print(this->heading);             // Course (degrees)
    this->myFile.print(sep);
  
    EEPROM.get(88,temp);
    this->myFile.print(temp); 
    this->myFile.print(this->speed);             // speed (knots)
    this->myFile.print(sep);
  
    EEPROM.get(93,temp);
    this->myFile.print(temp);                     // Altitude (feet). Goes anywhere in the comment area
    this->myFile.print(this->altitude2);
    this->myFile.print(sep);
  
    EEPROM.get(98,temp);
    this->myFile.print(temp);
    this->myFile.print(this->intTemp);
    this->myFile.print(sep);
  
    EEPROM.get(103,temp);
    this->myFile.print(temp);
    this->myFile.print(this->extTemp);
    this->myFile.println(sep);
    
    this->myFile.flush();
    this->myFile.close();
  }

  delay(100);
}

void APRSPacket::aprs_send(){
 
  const struct s_address addresses[] = { 
    {{ EEPROM.read(0), EEPROM.read(1), EEPROM.read(2), EEPROM.read(3)}, 11},                           // Destination callsign
    {{ EEPROM.read(5), EEPROM.read(6), EEPROM.read(7), EEPROM.read(8)}, 0},                            // Source callsign (-11 = balloon, -9 = car)
    {{ EEPROM.read(10), EEPROM.read(11), EEPROM.read(12), EEPROM.read(13), EEPROM.read(14)}, 1}                             // Digi1 (first digi in the chain)
  };

  char sep = EEPROM.read(43);
  char temp[5];

  ax25_send_header(addresses, sizeof(addresses)/sizeof(s_address));
  ax25_send_byte(sep);                // Report w/ timestamp, no APRS messaging. $ = NMEA raw data
  ax25_send_string(this->latitude);     // Lat: 38deg and 22.20 min (.20 are NOT seconds, but 1/100th of minutes)
  ax25_send_byte(sep);                // Symbol table

  ax25_send_string(this->longitude);     // Lon: 000deg and 25.80 min
  ax25_send_byte(EEPROM.read(14));                // Symbol: O=balloon, -=QTH
  ax25_send_byte(sep);

  EEPROM.get(83,temp);
  ax25_send_string(temp);
  ax25_send_string(this->heading);             // Course (degrees)
  ax25_send_byte(sep);

  EEPROM.get(88,temp);
  ax25_send_string(temp); 
  ax25_send_string(this->speed);             // speed (knots)
  ax25_send_byte(sep);

  EEPROM.get(93,temp);
  ax25_send_string(temp);                     // Altitude (feet). Goes anywhere in the comment area
  ax25_send_string(this->altitude2);
  ax25_send_byte(sep);

  EEPROM.get(98,temp);
  ax25_send_string(temp);
  ax25_send_string(this->intTemp);
  ax25_send_byte(sep);

  EEPROM.get(103,temp);
  ax25_send_string(temp);
  ax25_send_string(this->extTemp);
  ax25_send_byte(sep);

  ax25_send_byte(sep);     // Comment

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
