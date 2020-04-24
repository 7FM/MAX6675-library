// this library is public domain. enjoy!
// www.ladyada.net/learn/sensors/thermocouple

#ifndef max6675_h
#define max6675_h

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <FastPin.h>

template <uint8_t SCLK, uint8_t CS, uint8_t MISO>
class MAX6675 {
  private:
    MAX6675();

  public:
    static inline void init() {
        FastPin<SCLK>::setOutput();
        FastPin<SCLK>::hi();

        FastPin<CS>::setOutput();
        FastPin<CS>::hi();

        FastPin<MISO>::setInput();
    }

    static double readCelsius() {

        FastPin<CS>::lo();

        uint16_t v = 0;

        // Read temperature via spi
        for (uint16_t i = 0x8000; i >= 0x04; i >>= 1) {

        // MAX6675 is rated for 100ns delay between a signal change... 1 clock is about 62.5ns long (16MHz)
        // if implemented as loop no need for a delay (if cpu isn't too fast executing jump, decrement/shift, comparison & register write xD)
#ifdef SAFE_DELAY
#ifdef __AVR
            _delay_us(1);
#else
            delayMicroseconds(1);
#endif
#endif
            FastPin<SCLK>::lo();

#ifdef __AVR
            _delay_us(1);
#else
            delayMicroseconds(1);
#endif

//TODO implement digitalRead for other platforms too
#ifdef __AVR
            if (FastPin<MISO>::digitalRead()) {
#else
            if (digitalRead(MISO)) {
#endif
                //set the bit using the iteration variable
                v |= i;
            }

            FastPin<SCLK>::hi();
        }

        FastPin<CS>::hi();

        if (v & 0x4) {
            // uh oh, no thermocouple attached!
            return NAN;
        }

        // Remove unused bits
        v >>= 3;

        return v * 0.25;
    }

    static inline double readFahrenheit() {
        return readCelsius() * 1.8 + 32;
    }
};

#endif
