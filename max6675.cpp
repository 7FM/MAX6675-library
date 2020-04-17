// this library is public domain. enjoy!
// www.ladyada.net/learn/sensors/thermocouple

#ifdef __AVR
#include <avr/pgmspace.h>

//////////////////////////////////////////
// macros from http://masteringarduino.blogspot.com.es/2013/10/fastest-and-smallest-digitalread-and.html
/////////////////////////////////////////
#define portOfPin(P) \
    (((P) >= 0 && (P) < 8) ? &PORTD : (((P) > 7 && (P) < 14) ? &PORTB : &PORTC))
#define ddrOfPin(P) \
    (((P) >= 0 && (P) < 8) ? &DDRD : (((P) > 7 && (P) < 14) ? &DDRB : &DDRC))
#define pinOfPin(P) \
    (((P) >= 0 && (P) < 8) ? &PIND : (((P) > 7 && (P) < 14) ? &PINB : &PINC))
#define pinIndex(P) ((uint8_t)(P > 13 ? P - 14 : P & 7))
#define pinMask(P) ((uint8_t)(1 << pinIndex(P)))

#define pinAsInput(P) *(ddrOfPin(P)) &= ~pinMask(P)
#define pinAsInputPullUp(P)        \
    *(ddrOfPin(P)) &= ~pinMask(P); \
    digitalHigh(P)
#define pinAsOutput(P) *(ddrOfPin(P)) |= pinMask(P)
#define digitalLow(P) *(portOfPin(P)) &= ~pinMask(P)
#define digitalHigh(P) *(portOfPin(P)) |= pinMask(P)

#elif defined(ESP8266)
#include <pgmspace.h>
#endif
#include "max6675.h"

MAX6675::MAX6675(uint8_t SCLK, uint8_t CS, uint8_t MISO) {
    //define pin modes
    // Equivalent to pinMode(CS, OUTPUT); followed by digitalWrite(CS, HIGH);
#ifdef __AVR
    csBitMask = pinMask(CS);
    csReg = portOfPin(CS);
    pinAsOutput(CS);
#else
    uint8_t port = digitalPinToPort(CS);
    csBitMask = digitalPinToBitMask(CS);
    *portModeRegister(port) |= csBitMask;
    csReg = portOutputRegister(port);
#endif
    *csReg |= csBitMask;

    // Equivalent to pinMode(SCLK, OUTPUT); followed by digitalWrite(SCLK, HIGH);
#ifdef __AVR
    sclkBitMask = pinMask(SCLK);
    sclkReg = portOfPin(SCLK);
    pinAsOutput(SCLK);
#else
    port = digitalPinToPort(SCLK);
    sclkBitMask = digitalPinToBitMask(SCLK);
    *portModeRegister(port) |= sclkBitMask;
    sclkReg = portOutputRegister(port);
#endif
    *sclkReg |= sclkBitMask;

    // Equivalent to pinMode(MISO, INPUT);
#ifdef __AVR
    misoBitMask = pinMask(MISO);
    misoReg = portOfPin(MISO);
    pinAsInput(MISO);
#else
    port = digitalPinToPort(MISO);
    misoBitMask = digitalPinToBitMask(MISO);
    *portModeRegister(port) &= ~misoBitMask;
    misoReg = portInputRegister(port);
#endif
}

double MAX6675::readCelsius(void) {

    uint16_t v;

    //Toggle cs pin assumes that cs has HIGH value and should be equvalent to: digitalWrite(cs, LOW);
    *csReg ^= csBitMask;
#ifdef __AVR
    _delay_us(1);
#else
    delayMicroseconds(1);
#endif

    v = spiread();
    v <<= 8;
    v |= spiread();

    //Toggle cs pin assumes that cs has LOW value and should be equvalent to: digitalWrite(cs, HIGH);
    *csReg ^= csBitMask;

    if (v & 0x4) {
        // uh oh, no thermocouple attached!
        return NAN;
    }

    // Remove unused bits
    v >>= 3;

    return v * 0.25;
}

double MAX6675::readFahrenheit(void) {
    return readCelsius() * 9.0 / 5.0 + 32;
}

byte MAX6675::spiread(void) {
    int i;
    byte d = 0;

    for (i = 7; i >= 0; i--) {
        //Toggle sclk pin assumes that sclk has HIGH value and should be equvalent to: digitalWrite(sclk, LOW);
        *sclkReg ^= sclkBitMask;
#ifdef __AVR
        _delay_us(1);
#else
        delayMicroseconds(1);
#endif
        // equivalent to: if (digitalRead(miso)) {
        if (*misoReg & misoBitMask) {
            //set the bit to 0 no matter what
            d |= (1 << i);
        }

        //Toggle sclk pin assumes that sclk has LOW value and should be equvalent to: digitalWrite(sclk, HIGH);
        *sclkReg ^= sclkBitMask;
#ifdef __AVR
        _delay_us(1);
#else
        delayMicroseconds(1);
#endif
    }

    return d;
}
