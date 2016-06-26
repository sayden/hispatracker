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

// Arduino/AVR libs
#if (ARDUINO + 1) >= 100
#  include <Arduino.h>
#else
#  include <WProgram.h>
#endif


// Trackuino custom libs
#include "config.h"
#include "afsk_avr.h"
#include "aprs.h"
#include "pin.h"
#include "power.h"
#include "AprsSender.h"

//External temp sensor
#include <OneWire.h>
OneWire  ds(2);  // on pin 10 (a 4.7K resistor is necessary)

//GPS
#include <NMEAGPS.h>
#include <NeoSWSerial.h>

static NMEAGPS gps;  // The parsing object
static gps_fix fix;  // GPS fields are stored here as they are parsed
static uint8_t gpsUpdates = 0; // how many updates since power_save

NeoSWSerial gpsS(RXPIN, TXPIN);
HardwareSerial & hx1 = Serial; // an alias for pins 0/1

//Internal Temp sensor
#include <Wire.h>
#include "Adafruit_BMP085.h"  // Adafruit BMP085 library
Adafruit_BMP085 bmp;

// Module constants
static const uint32_t VALID_POS_TIMEOUT = 2000;  // ms

// Module variables
static int32_t next_aprs = 0;

void setup(){
  pinMode(LED_PIN, OUTPUT);
  pin_write(LED_PIN, LOW);
  
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }

  afsk_setup();
  gpsS.begin(9600);
  hx1.begin(9600);
}

void loop(){
  // Read the GPS port and parse the characters.  When
  //   a complete fix is available, we'll read it.

  if (gps.available( gpsS )) {
    fix = gps.read();

    gpsUpdates++;
    if (gpsUpdates >= 2) {
      // Wait for two full GPS updates to come in before we use them

      //Check pressure the flight day in http://weather.noaa.gov/weather/current/LEMD.html
      //We take the value between parenthesis. If pressure is 1013 write in readAltitude
      //this value multiplied by 100
      APRSPacket packet{fix.latitude(),
                        fix.longitude(),
                        fix.altitude(),
                        bmp.readAltitude(101300),
                        fix.speed(),
                        fix.heading(),
                        externalTemp(&ds),
                        bmp.readTemperature(),
                        bmp.readPressure() };

      packet.testSendAprsWithChars(&hx1);

      // Time for another APRS frame?
      next_aprs++;
      if (next_aprs >= APRS_PERIOD) {
        next_aprs  = 0; // reset for next time
        gpsUpdates = 0;

        aprs_send("gps_time", "gps_aprs_lat", "gps_aprs_lon", 100, 101, 102, "int_temperature",
        "ext_temperature", "sensors_vin");
        next_aprs += APRS_PERIOD * 1000L;
        while (afsk_flush()) {
          power_save();
        }
      }
    }
  }
}


float lastExtTemp = 0;

//externalTemp uses DS18B20 to return a floating point value representing the
//external temperature in celsius
float externalTemp(OneWire *ds){
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;
  
  if ( !ds->search(addr)) {
    ds->reset_search();
    delay(250);
    return lastExtTemp;
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      hx1.println("CRC is not valid!");
      return -100.0;
  }
  type_s = 0; 

  ds->reset();
  ds->select(addr);
  ds->write(0x44, 1);        // start conversion, with parasite power on at the end
  
//  delay(1000);     // maybe 750ms is enough, maybe not
  
  present = ds->reset();
  ds->select(addr);    
  ds->write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds->read();
  }

  // Convert the data to actual temperature
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  lastExtTemp = (float)raw / 16.0;

  return lastExtTemp;
}