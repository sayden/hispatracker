#include <Arduino.h>
#include "NMEAGPS.h"

//======================================================================
//  Program: NMEA_isr.ino
//
//  Prerequisites:
//     1) NMEA.ino works with your device
//
//  Description:  This minimal program parses the GPS data during the 
//     RX character interrupt.  The ISR passes the character to
//     the GPS object for parsing.  The GPS object will add gps_fix 
//     structures to a buffer that can be later read() by loop().
//======================================================================

#if defined( UBRR1H )
  // Default is to use NeoSerial1 when available.  You could also
  #include <NeoHWSerial.h>
  // NOTE: There is an issue with IDEs before 1.6.6.  The above include 
  // must be commented out for non-Mega boards, even though it is
  // conditionally included.  If you are using an earlier IDEs, 
  // comment the above include.
#else  
  // Only one serial port is available, uncomment one of the following:
  //#include <NeoICSerial.h>
  #include <NeoSWSerial.h>
  //#include <SoftwareSerial.h> /* NOT RECOMMENDED */
#endif
#include "GPSport.h"

#include "Streamers.h"
#ifdef NeoHWSerial_h
  #define DEBUG_PORT NeoSerial
#else
  #define DEBUG_PORT Serial
#endif

// Check configuration

#ifndef NMEAGPS_INTERRUPT_PROCESSING
  #error You must define NMEAGPS_INTERRUPT_PROCESSING in NMEAGPS_cfg.h!
#endif

static NMEAGPS   gps;


//Import required local headers
#include "config.h"
#include "afsk_avr.h"
#include "aprs.h"
#include "pin.h"
#include "power.h"



//------------------------------------------------------------
// Internal Temp sensor
#include <Wire.h>
#include <Adafruit_BMP085.h>  // Adafruit BMP085 library
Adafruit_BMP085 bmp;


//-------------------------------------------------------------
static uint8_t next_aprs = 0;


//--------------------------

static void GPSisr( uint8_t c )
{
  gps.handle( c );

} // GPSisr

//--------------------------

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  pin_write(LED_PIN, LOW);

  afsk_setup();

  //Internal temp and pressure sensor 
  bmp.begin();

  // Start the normal trace output
  DEBUG_PORT.begin(9600);
  while (!DEBUG_PORT)
    ;


  trace_header( DEBUG_PORT );

  DEBUG_PORT.flush();

  // Start the UART for the GPS device
  gps_port.attachInterrupt( GPSisr );
  gps_port.begin( 9600 );
}

static gps_fix  fix_data;

static void doSomeWork() {
  // Print all the things!

  next_aprs++;
  if (next_aprs >= 5) {
    next_aprs = 0;

    char latitude[9];
    char longitude[10];
    char datetime[7];
    if(fix_data.valid.location) {
      snprintf(latitude, 9, "%02d%02d.%02u%c", fix_data.latitudeDMS.degrees, fix_data.latitudeDMS.minutes, truncate(fix_data.latitudeDMS.seconds_whole, 2), fix_data.latitudeDMS.NS() );
      snprintf(longitude, 10, "%03d%02d.%02u%c", fix_data.longitudeDMS.degrees, fix_data.longitudeDMS.minutes, truncate(fix_data.longitudeDMS.seconds_whole, 2), fix_data.longitudeDMS.EW() );
    }


    snprintf(datetime, 7, "%02d%02d%02d", fix_data.dateTime.hours, fix_data.dateTime.minutes, fix_data.dateTime.seconds);

  
    //Check pressure the flight day in http://weather.noaa.gov/weather/current/LEMD.html
    //We take the value between parenthesis. If pressure is 1013 write in readAltitude
    //this value multiplied by 100
    APRSPacket packet(datetime,
                      bmp.readAltitude(101900),
                      fix_data.speed(),
                      fix_data.heading(),
                      bmp.readTemperature(),
                      bmp.readPressure());

    packet.setLatitude(latitude);
    packet.setLongitude(longitude);
    if(fix_data.valid.location){
      packet.setLatitudeF(fix_data.latitude());
      packet.setLongitudeF(fix_data.longitude());
    }
  

    // packet.aprs_send();
  
    // while (afsk_flush()) {
    //   power_save();
    // }
    DEBUG_PORT.println(bmp.readTemperature());
  }
} // doSomeWork

//--------------------------

void loop() {
  if (gps.available( gps_port )) {
    DEBUG_PORT.println(bmp.readTemperature());
    // Print all the things!
    trace_all( DEBUG_PORT, gps, gps.read() );
    fix_data = gps.fix();
    // fix_data = gps.read();
    doSomeWork();
  }

  if (gps.overrun()) {
    gps.overrun( false );
    DEBUG_PORT.println( F("DATA OVERRUN: took too long to use gps.read() data!") );

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