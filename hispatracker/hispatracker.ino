#include <Arduino.h>
#include "NMEAGPS.h"


#include "config.h"
#include "afsk_avr.h"
#include "aprs.h"
#include "pin.h"
#include "power.h"


#define LOAD_EEPROM

#include <AltSoftSerial.h>

#include "GPSport.h"
//------------------------------------------------------------
// This object parses received characters
//   into the gps.fix() data structure
static NMEAGPS  gps;

//------------------------------------------------------------
//  Define a set of GPS fix information.  It will
//  hold on to the various pieces as they are received from
//  an RMC sentence.  It can be used anywhere in your sketch.
static gps_fix  fix_data;

//This values counts when is moment to send a new message
static uint8_t next_aprs = 0;

//Sends an APRS packet every...
#define SEND_TIME_SPACING 60
static APRSPacket packet;


//Internal Temp sensor
#include <Wire.h>
#include <Adafruit_BMP085.h>  // Adafruit BMP085 library

Adafruit_BMP085 bmp;


//--------------------------
// APRS Compression table is stored in the EEPROM to save some space
#ifdef LOAD_EEPROM
    #include "eeprom_loader.cpp"
#endif

void setup()
{
  #ifdef LOAD_EEPROM
      loadEEPROM();
  #endif


  pinMode(LED_PIN, OUTPUT);
  pin_write(LED_PIN, LOW);

  bmp.begin();

  afsk_setup();

  // Start the UART for the GPS device
  gps_port.begin( 9600 );
}


static void doSomeWork() {
  packet.aprs_send();

  while (afsk_flush()) {
    power_save();
  }
  
} // doSomeWork

void getDatetimeReading(){
  char datetime[7];
  snprintf(datetime, 7, "%02d%02d%02d", fix_data.dateTime.hours, fix_data.dateTime.minutes, fix_data.dateTime.seconds);
  packet.setDatetime(datetime);
}

void getBMPReadings(){
  packet.setAltitude(bmp.readAltitude(101900));
  packet.setTemperature(bmp.readTemperature());
  packet.setPressure(bmp.readPressure());
}

void getSpeed(){
  int speed = pow(1.08,fix_data.speed()) - 1;
  packet.setSpeed(EEPROM.read(speed+33));
}

void getHeading(){
  int course = fix_data.heading() / 4.0;
  char c = EEPROM.read(course+33);
  packet.setHeading(c);
}


//Takes a floating point value of longitude and returns its corresponding 4 char value
//in APRS compressed format
void getLongitude(float l, char temp[5]){
    unsigned long res = 190463 * (180.0 + l);

    uint32_t res1 = res/753571;
    uint32_t rem1 = res%753571;
    uint32_t res2 = rem1/8281;
    uint32_t rem2 = rem1%8281;
    uint16_t res3 = rem2/91;
    uint16_t rem3 = rem2%91;

    sprintf(temp, "%c%c%c%c\0", EEPROM.read(res1+33), EEPROM.read(res2+33), EEPROM.read(res3+33), EEPROM.read(rem3+33));
}

//Takes a floating point value of latitude and returns its corresponding 4 char value
//in APRS compressed format
void getLatitude(float l, char temp[5]){
    unsigned long res = 380926 * (90 - l);

    uint32_t res1 = res/753571;
    uint32_t rem1 = res%753571;
    uint32_t res2 = rem1/8281;
    uint32_t rem2 = rem1%8281;
    uint16_t res3 = rem2/91;
    uint16_t rem3 = rem2%91;

    sprintf(temp, "%c%c%c%c\0", EEPROM.read(res1+33), EEPROM.read(res2+33), EEPROM.read(res3+33), EEPROM.read(rem3+33));
}

static char latitude[5];
static char longitude[5];

void getLatitude(){
  getLatitude(fix_data.latitude(), latitude);
  packet.setLatitude(latitude);
}

void getLongitude(){
  getLongitude(fix_data.longitude(), longitude);
  packet.setLongitude(longitude);
}

//--------------------------

void loop()
{
  while (gps.available( gps_port )) {
    fix_data = gps.read();
    switch(next_aprs){
      case SEND_TIME_SPACING-5:
        getBMPReadings();
        break;
      case SEND_TIME_SPACING-4:
        if(fix_data.valid.speed) getSpeed();
        break;
      case SEND_TIME_SPACING-3:
        if(fix_data.valid.heading) getHeading();
        break;
      case SEND_TIME_SPACING-2:
        getDatetimeReading();
        break;
      case SEND_TIME_SPACING-1:
        if(fix_data.valid.location) {
          getLatitude();
          getLongitude();
        }
        break;
      case SEND_TIME_SPACING:
        doSomeWork();
        next_aprs=-1;
        break;
      default:
        break;
    }
    next_aprs++;
  }
}