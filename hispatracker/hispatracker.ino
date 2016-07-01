#include <Arduino.h>
#include <NMEAGPS.h>

#if defined( UBRR1H )
  // Default is to use Serial1 when available.  You could also
  // use NeoHWSerial, especially if you want to handle GPS characters
  // in an Interrupt Service Routine.
  //#include <NeoHWSerial.h>
#else
  // Only one serial port is available, uncomment one of the following:
  //#include <NeoICSerial.h>
  //#include <AltSoftSerial.h>
  #include <NeoSWSerial.h>
  //#include <SoftwareSerial.h> /* NOT RECOMMENDED */
#endif

#include <GPSport.h>

//------------------------------------------------------------
// When NeoHWSerial is used, none of the built-in HardwareSerial
//   variables can be used: Serial, Serial1, Serial2 and Serial3
//   *cannot* be used.  Instead, you must use the corresponding
//   NeoSerial, NeoSerial1, NeoSerial2 or NeoSerial3.  This define
//   is used to substitute the appropriate Serial variable in
//   all debug prints below.

#ifdef NeoHWSerial_h
  #define DEBUG_PORT NeoSerial
#else
  #define DEBUG_PORT Serial
#endif

//------------------------------------------------------------
// This object parses received characters
//   into the gps.fix() data structure

static NMEAGPS  gps;

//------------------------------------------------------------
//  Define a set of GPS fix information.  It will
//  hold on to the various pieces as they are received from
//  an RMC sentence.  It can be used anywhere in your sketch.

static gps_fix  fix;


static uint8_t next_aprs = 0;

#include "config.h"
#include "afsk_avr.h"
#include "aprs.h"
#include "pin.h"
#include "power.h"


//Internal Temp sensor
#include <Wire.h>
#include <Adafruit_BMP085.h>  // Adafruit BMP085 library

Adafruit_BMP085 bmp;

//----------------------------------------------------------------
//  This function gets called about once per second, during the GPS
//  quiet time.  It's the best place to do anything that might take
//  a while: print a bunch of things, write to SD, send an SMS, etc.
//
//  By doing the "hard" work during the quiet time, the CPU can get back to
//  reading the GPS chars as they come in, so that no chars are lost.

static void doSomeWork() {
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

} // doSomeWork

//------------------------------------
//  This is the main GPS parsing loop.

static void GPSloop() {
  while (gps.available( gps_port )) {
    fix = gps.read();
    doSomeWork();
  }

} // GPSloop

//--------------------------

void setup() {
  // Start the UART for the GPS device
  gps_port.begin( 9600 );

	pinMode(LED_PIN, OUTPUT);
	pin_write(LED_PIN, LOW);

	bmp.begin();

	afsk_setup();

	next_aprs = 0;
}

//--------------------------

void loop() {
  GPSloop();
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
