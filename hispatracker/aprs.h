#ifndef __APRS_H__
#define __APRS_H__

// void aprs_send(char gps_time[4], char gps_aprs_lat[12], char gps_aprs_lon[12], double gps_course,
//   double gps_speed, float gps_altitude, char int_temperature[], char ext_temperature[],
//   char sensors_vin[]);

#include <EEPROM.h>
//SD Reader Writer
#define USE_SPI_LIB
//SD Reader Writer
#include <SPI.h>
#include <SD.h>

class APRSPacket {
    public:
	char latitude[12];
	char longitude[12];
	char altitude2[6];
	char speed[6];
	char heading[4];
	char extTemp[6];
	char intTemp[6];
	char pressure[6];

	void aprs_send();
	void writeToSD();

	APRSPacket(float _latitude,
	  float _longitude,
	  float _altitude2,
	  float _speed,
	  float _heading,
	  float _extTemp,
	  float _intTemp,
	  int32_t pressure);
};


// void aprs_send_header_mesage(char header[], char message[]);
// void aprs_send_raw(char message[]);

#endif
