#ifndef __APRS_H__
#define __APRS_H__

#if (ARDUINO + 1) >= 100
#  include <Arduino.h>
#else

#  include <WProgram.h>

#endif	

class APRSPacket {
    public:
   	char datetime[7];
	char latitude[9];
	char longitude[10];
	char altitude[6];
	char speed[6];
	char heading[4];
	char intTemp[6];
	char pressure[6];
	float latitudeF;
	float longitudeF;

	void aprs_send();
	// void writeToSD();

	void setLatitude(char latitude[9]);
	void setLongitude(char longitude[10]);	
	void setLatitudeF(float _latitude);
	void setLongitudeF(float _longitude);

	APRSPacket(
   	  char _datetime[7],
	  float _altitude,
	  float _speed,
	  float _heading,
	  float _intTemp,
	  int32_t _pressure);
};

#endif
