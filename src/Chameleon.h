#ifndef CHAMELEON_H
#define CHAMELEON_H
#include "Arduino.h"
#include <math.h>

class Chameleon {
public:
    Chameleon(uint8_t redPin, uint8_t greenPin, uint8_t bluePin, int writeRange);
    void setColor(int red, int green, int blue);
    void fadeTo(int red, int green, int blue, int steps, long duration);
    void fadeTo(int red, int green, int blue, long duration);
    void fadeIn(int red, int green, int blue, long duration);
    void fadeOut(int red, int green, int blue, long duration);
    void fadeIn(int red, int green, int blue, int steps, long duration);
    void fadeOut(int red, int green, int blue, int steps, long duration);
private:
    uint8_t redPin, greenPin, bluePin;
    int red, green, blue;
    int writeRange;
    int clamp(int value, int lower, int upper);
};

#endif //CHAMELEON_H


