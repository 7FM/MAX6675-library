// this library is public domain. enjoy!
// www.ladyada.net/learn/sensors/thermocouple

#ifndef max6675_h
#define max6675_h

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class MAX6675 {
  public:
    MAX6675(uint8_t SCLK, uint8_t CS, uint8_t MISO);

    double readCelsius(void);
    double readFahrenheit(void);

  private:
    uint8_t spiread(void);

    volatile uint8_t *csReg;
    volatile uint8_t *sclkReg;
    volatile uint8_t *misoReg;
    uint8_t csBitMask;
    uint8_t sclkBitMask;
    uint8_t misoBitMask;
};

#endif
