// // Arduino/AVR libs
// #if (ARDUINO + 1) >= 100
// #  include <Arduino.h>
// #else
// #  include <WProgram.h>
// #endif
#include <Arduino.h>

// Trackuino custom libs
#include "config.h"
#include "afsk_avr.h"
#include "aprs.h"
#include "pin.h"
#include "power.h"

//External temp sensor
//#include <OneWire.h>

//GPS
#include <NMEAGPS.h>
#include <NeoSWSerial.h>

static NMEAGPS gps;  // The parsing object
static gps_fix fix;  // GPS fields are stored here as they are parsed

static NeoSWSerial gpsS(RXPIN, TXPIN);

//Internal Temp sensor
#include <Wire.h>
#include <Adafruit_BMP085.h>  // Adafruit BMP085 library

Adafruit_BMP085 bmp;

static uint8_t next_aprs = 0;

void setup() {
    pinMode(LED_PIN, OUTPUT);
    pin_write(LED_PIN, LOW);

    bmp.begin();

    afsk_setup();
    
    gpsS.begin(9600);

    next_aprs = 0;
}

void loop() {
  while (gps.available(gpsS)) {
    fix = gps.read();
    next_aprs++;

    if (next_aprs >= 5) {
      next_aprs = 0;

      char latitude[9];
      char longitude[10];
      char datetime[7];
      if(fix.valid.location) {
        snprintf(latitude, 9, "%02d%02d.%02u%c", fix.latitudeDMS.degrees, fix.latitudeDMS.minutes, truncate(fix.latitudeDMS.seconds_whole, 2), fix.latitudeDMS.NS() );
        snprintf(longitude, 10, "%03d%02d.%02u%c", fix.longitudeDMS.degrees, fix.longitudeDMS.minutes, truncate(fix.longitudeDMS.seconds_whole, 2), fix.longitudeDMS.EW() );
      }


      snprintf(datetime, 7, "%02d%02d%02d", fix.dateTime.hours, fix.dateTime.minutes, fix.dateTime.seconds);

    
      //Check pressure the flight day in http://weather.noaa.gov/weather/current/LEMD.html
      //We take the value between parenthesis. If pressure is 1013 write in readAltitude
      //this value multiplied by 100
      APRSPacket packet(datetime,
                        bmp.readAltitude(101900),
                        fix.speed(),
                        fix.heading(),
                        bmp.readTemperature(),
                        bmp.readPressure());

      packet.setLatitude(latitude);
      packet.setLongitude(longitude);
      if(fix.valid.location){
        packet.setLatitudeF(fix.latitude());
        packet.setLongitudeF(fix.longitude());
      }
    
      packet.aprs_send();
    
      while (afsk_flush()) {
        power_save();
      }
    }
  }
}

//truncate takes a uint16_t number and returns its "amount" most significant
//number: For example from 159 and 2 it will return 15 or from 14231 and 3 
//it will return 142
uint8_t truncate(uint16_t n, uint8_t amount) {
    if (n > pow(10, amount) && n < pow(10, amount + 1)) {
        return (uint8_t) (round(n / 10.0));
    } else if (n > pow(10, amount + 1)) {
        return truncate((uint16_t) round(n / 10), (uint8_t) (amount - 1));
    } else {
        return (uint8_t) n;
    }
}