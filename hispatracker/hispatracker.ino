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
#if (ARDUINO + 0) == 0
#error "Oops! We need the real Arduino IDE (version 22 or 23) for Arduino builds."
#error "See trackuino.pde for details on this"

// Refuse to compile on arduino version 21 or lower. 22 includes an 
// optimization of the USART code that is critical for real-time operation
// of the AVR code.
#elif (ARDUINO + 0) < 22
#error "Oops! We need Arduino 22 or 23"
#error "See trackuino.pde for details on this"

#endif


// Trackuino custom libs
#include "config.h"
#include "afsk_avr.h"
#include "aprs.h"
#include "pin.h"
#include "power.h"


//TinyGPS gps;
#include <NMEAGPS.h>
#include <NeoSWSerial.h>

static NMEAGPS gps;  // The parsing object
static gps_fix fix;  // GPS fields are stored here as they are parsed
static uint8_t gpsUpdates = 0; // how many updates since power_save

NeoSWSerial gpsS(RXPIN, TXPIN);
HardwareSerial & hx1 = Serial; // an alias for pins 0/1

// Arduino/AVR libs
#if (ARDUINO + 1) >= 100
#  include <Arduino.h>
#else
#  include <WProgram.h>
#endif


// Module constants
static const uint32_t VALID_POS_TIMEOUT = 2000;  // ms

// Module variables
static int32_t next_aprs = 0;


void setup(){
  pinMode(LED_PIN, OUTPUT);
  pin_write(LED_PIN, LOW);

  afsk_setup();
  gpsS.begin(9600);
  hx1.begin(115200);
}
void loop()
{
  // Read the GPS port and parse the characters.  When
  //   a complete fix is available, we'll read it.

  if (gps.available( gpsS )) {
    fix = gps.read();

    gpsUpdates++;
    if (gpsUpdates >= 2) {
      // Wait for two full GPS updates to come in before we use them

      hx1.print("LAT=");
      if (fix.valid.location)
        hx1.print( fix.latitude(), 6 );
      hx1.print(" LON=");
      if (fix.valid.location)
        hx1.print( fix.longitude(), 6 );
      hx1.println();

      // Time for another APRS frame?
      next_aprs++;
      if (next_aprs >= APRS_PERIOD) {
        next_aprs  = 0; // reset for next time
        gpsUpdates = 0;

        char lati[12];
        dtostrf(fix.latitude(), 8, 6, lati);
        aprs_send("gps_time", lati, "gps_aprs_lon", 100, 101, 102, "int_temperature",
        "ext_temperature", "sensors_vin");
        next_aprs += APRS_PERIOD * 1000L;
        while (afsk_flush()) {
          power_save();
        }

        #ifdef DEBUG_MODEM
            // Show modem ISR stats from the previous transmission
            afsk_debug();
        #endif
      }
    }
  }
}

void sendAprsWithChars(fix gps_fix) {
        char latitudeti[12];
        dtostrf(fix.latitude(), 8, 6, latitudeti);
        
        char longitude[12];
        dtostrf(fix.latitude(), 8, 6, longitude);
        
        char altitude[12];
        dtostrf(fix.altitude(), 8, 6, altitude);
        
        char speed[12];
        dtostrf(fix.speed_kph(), 8, 6, speed);
        
        char heading[12];
        dtostrf(fix.heading_cd(), 8, 6, heading);
}

