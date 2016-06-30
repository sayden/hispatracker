/* trackuino copyright (C) 2010  EA5HAV Javi
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

// Mpide 22 fails to compile Arduino code because it stupidly defines ARDUINO 
// as an empty macro (hence the +0 hack). UNO32 builds are fine. Just use the
// real Arduino IDE for Arduino builds. Optionally complain to the Mpide
// authors to fix the broken macro.
// #if (ARDUINO + 0) == 0
// #error "Oops! We need the real Arduino IDE (version 22 or 23) for Arduino builds."
// #error "See trackuino.pde for details on this"

// // Refuse to compile on arduino version 21 or lower. 22 includes an 
// // optimization of the USART code that is critical for real-time operation
// // of the AVR code.
// #elif (ARDUINO + 0) < 22
// #error "Oops! We need Arduino 22 or 23"
// #error "See trackuino.pde for details on this"

// #endif

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
static uint8_t gpsUpdates = 0; // how many updates since power_save

NeoSWSerial gpsS(RXPIN, TXPIN);

//Internal Temp sensor
#include <Wire.h>
#include <Adafruit_BMP085.h>  // Adafruit BMP085 library

Adafruit_BMP085 bmp;

unsigned long next_aprs = 0;
bool switched = 1;

void setup() {
    pinMode(LED_PIN, OUTPUT);

    bmp.begin();

    afsk_setup();
    
    gpsS.begin(9600);

    next_aprs = millis() + 5000l;
}


void loop() {
  pin_write(LED_PIN, switched);
    if (gps.available(gpsS)) {
        fix = gps.read();
        switched = 0;
    } else {
      switched = 1;
    }

  if (millis() >= next_aprs) {
    char latitude[9];
    snprintf(latitude, 9, "%04d.%02u%c", fix.latitudeDMS.degrees, truncate(fix.latitudeDMS.seconds_frac, 2), fix.latitudeDMS.NS() );

    char longitude[10];
    snprintf(longitude, 9, "%05d.%02u%c", fix.longitudeDMS.degrees, truncate(fix.longitudeDMS.seconds_frac, 2), fix.longitudeDMS.EW() );
  
    //Check pressure the flight day in http://weather.noaa.gov/weather/current/LEMD.html
    //We take the value between parenthesis. If pressure is 1013 write in readAltitude
    //this value multiplied by 100
    APRSPacket packet(bmp.readAltitude(101900),
                      fix.speed(),
                      fix.heading(),
                      bmp.readTemperature(),
                      bmp.readPressure());
    packet.setLatitude(latitude);
    packet.setLongitude(longitude);
  
    packet.aprs_send();
  
    while (afsk_flush()) {
      power_save();
    }
  
    next_aprs = millis() + 5000l;
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