// this library is public domain. enjoy!
// https://learn.adafruit.com/thermocouple/

#ifndef ADAFRUIT_MAX6675_H
#define ADAFRUIT_MAX6675_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <FastPin.h>

// if enabled the result is represented in Q11.2 format
#ifdef MAX6675_USE_FIX_PT
// The upper three bits can not be set so use this as value (with compatibility for signed)
#define NO_THERMO_VALUE 0b0110000000000000
#define MAX6675_NO_THERMO(x) ((x) == NO_THERMO_VALUE)
#define MAX6675_CONVERT_TO_FIX_PT(x) ((int16_t)((x) * 4))
#define MAX6675_PATCH_MUL(x) ((x) >> 2)
#else
#define NO_THERMO_VALUE NAN
#define MAX6675_NO_THERMO(x) isnan(x)
#endif

template <uint8_t SCLK, uint8_t CS, uint8_t MISO>
class MAX6675 {
  private:
    MAX6675();

  public:
    static inline void init() __attribute__((always_inline)) {
        FastPin<SCLK>::setOutput();
        FastPin<SCLK>::hi();

        FastPin<CS>::setOutput();
        FastPin<CS>::hi();

        FastPin<MISO>::setInput();
    }

#ifdef MAX6675_USE_FIX_PT
    static uint16_t readCelsius() {
#else
    static float readCelsius() {
#endif

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
            return NO_THERMO_VALUE;
        }

        // Remove unused bits
        v >>= 3;

#ifndef MAX6675_USE_FIX_PT
        return v * 0.25f;
#else
        return v;
#endif
    }

#ifndef MAX6675_USE_FIX_PT
    static inline float readFahrenheit() {
        return readCelsius() * 1.8f + 32;
    }
#else
    static inline uint16_t readFahrenheit() {
        //TODO 1.8 can not be represented exactly with Q11.2 format...
        return MAX6675_PATCH_MUL(readCelsius() * MAX6675_CONVERT_TO_FIX_PT(1.8f)) + MAX6675_CONVERT_TO_FIX_PT(32);
    }
#endif
};

#endif
