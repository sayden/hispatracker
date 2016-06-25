#include "AprsSender.h";

#if (ARDUINO + 1) >= 100
#  include <Arduino.h>
#else
#  include <WProgram.h>
#endif
#include <stdint.h>

#include <NeoSWSerial.h>

void APRSPacket::testSendAprsWithChars(HardwareSerial *hx1) {
    APRSCharPacket packet = this->getAPRScharPacket();

    hx1->print("LAT=");
    hx1->print(packet.latitude);
      
    hx1->print(" LON=");
    hx1->print(packet.longitude);
      
    hx1->print("  SPEED=");
    hx1->print(packet.speed);
    hx1->print("kmh ");
      
    hx1->print(" ALT=");
    hx1->print(packet.altitude);
    hx1->print("m ");
      
    hx1->print("ALT2=");
    hx1->print(packet.altitude2);
    hx1->print("m ");
      
    hx1->print(" HEA=");
    hx1->print(packet.heading);
    hx1->print("deg ");
            
    hx1->print(" TMPe=");
    hx1->print(packet.extTemp);
    hx1->print("c ");
      
    hx1->print("TMPi=");
    hx1->print(packet.intTemp);
    hx1->print("c ");
      
    hx1->print("PRES=");
    hx1->print(packet.pressure);
    hx1->print("Pa");
    
    hx1->println("");
}

APRSCharPacket APRSPacket::getAPRScharPacket(){
    APRSCharPacket packet;

    dtostrf(this->latitude, 8, 6, packet.latitude);
    dtostrf(this->longitude, 8, 6, packet.longitude);
    dtostrf(this->altitude, 3, 0, packet.altitude);
    dtostrf(this->altitude2, 3, 0, packet.altitude2);
    dtostrf(this->speed, 1, 2, packet.speed);
    dtostrf(this->heading, 1, 0, packet.heading);
    dtostrf(this->extTemp, 5, 2, packet.extTemp);
    dtostrf(this->intTemp, 5, 2, packet.intTemp);
    sprintf( packet.pressure, "%li", this->pressure );

    return packet;
}
