#ifndef __RADIO_HX1_H__
#define __RADIO_HX1_H__

#include "radio.h"

class RadioHx1 : public Radio {
  public:
    virtual void setup();
    virtual void ptt_on();
    virtual void ptt_off();
};

#endif
