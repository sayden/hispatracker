#ifndef __APRS_SENDER__
#define __APRS_SENDER__

#if (ARDUINO + 1) >= 100
#  include <Arduino.h>
#else
#  include <WProgram.h>
#endif
#include <stdint.h>


class APRSCharPacket {
    public:
      char latitude[12];
      char longitude[12];
      char altitude[8];
      char altitude2[8];
      char speed[8];
      char heading[12];
      char extTemp[8];
      char intTemp[8];
      char pressure[8];
};

class APRSPacket {
    public:
      float latitude;
      float longitude;
      float altitude;
      float altitude2;
      float speed;
      float heading;
      float extTemp;
      float intTemp;
      int32_t pressure;
      void testSendAprsWithChars(HardwareSerial *hx1);
      APRSCharPacket getAPRScharPacket();
};

#endif
 