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
	char latitude[5];
	char longitude[5];
	char altitude[6];
	char speed;
	char heading;
	char intTemp[6];
	char pressure[6];

	void aprs_send();

	//Setters
	void setLatitude(char latitude[5]);
	void setLongitude(char longitude[5]);	
	void setAltitude(float _altitude);
	void setTemperature(float _intTemp);
	void setPressure(int32_t _pressure);
	void setSpeed(char _speed);
	void setHeading(char _heading);
	void setDatetime(char datetime[7]);

	//Constructors
	APRSPacket();
	APRSPacket(
	  float _altitude,
	  float _intTemp,
	  int32_t _pressure);
};

#endif
