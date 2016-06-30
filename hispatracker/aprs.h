#ifndef __APRS_H__
#define __APRS_H__

#if (ARDUINO + 1) >= 100
#  include <Arduino.h>
#else

#  include <WProgram.h>

#endif	

class APRSPacket {
    public:
	char latitude[12];
	char longitude[12];
	char altitude[6];
	char speed[6];
	char heading[4];
	char intTemp[6];
	char pressure[6];

	void aprs_send();
	// void writeToSD();

	APRSPacket(float _latitude,
	  float _longitude,
	  float _altitude,
	  float _speed,
	  float _heading,
	  float _intTemp,
	  int32_t _pressure);
};

#endif
