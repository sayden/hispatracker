#ifndef __RADIO_H__
#define __RADIO_H__

class Radio {
  public:
    virtual void setup() = 0;
    virtual void ptt_on() = 0;
    virtual void ptt_off() = 0;
};

#endif
