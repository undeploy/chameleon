#include "Chameleon.h"
#include "Arduino.h"

Chameleon::Chameleon(uint8_t redPin, uint8_t greenPin, uint8_t bluePin, int writeRange) {
    this->redPin = redPin;
    this->greenPin = greenPin;
    this->bluePin = bluePin;
    this->writeRange = clamp(writeRange, 1, INT32_MAX);
    analogWriteRange(static_cast<uint32_t>(this->writeRange));
}

void Chameleon::setColor(int red, int green, int blue){
    this->red = clamp(red, 0 , this->writeRange);
    this->green = clamp(green, 0 , this->writeRange);
    this->blue = clamp(blue, 0 , this->writeRange);
    analogWrite(this->redPin, this->red);
    analogWrite(this->greenPin, this->green);
    analogWrite(this->bluePin, this->blue);
}

void Chameleon::fadeTo(int red, int green, int blue, long duration){
    // Setting max possible value of step, so it will be clamped next method, to a valid number.
    this->fadeTo(clamp(red, 0, writeRange), clamp(green, 0, writeRange), clamp(blue, 0, writeRange), this->writeRange, duration);
}

void Chameleon::fadeTo(int red, int green, int blue, int steps, long duration){

    // Colors cannot have values greater than writeRange and lower than 0
    red = this->clamp(red, 0, this->writeRange);
    green = this->clamp(green, 0, this->writeRange);
    blue = this->clamp(blue, 0, this->writeRange);

    Serial.print("red=");Serial.print(red);
    Serial.print(", green=");Serial.print(green);
    Serial.print(", blue=");Serial.print(blue);
    Serial.println();

    // Difference from initial to new color
    int redDifference = red - this->red;
    int greenDifference = green - this->green;
    int blueDifference = blue - this->blue;

    Serial.print("redDifference=");Serial.print(redDifference);
    Serial.print(", greenDifference=");Serial.print(greenDifference);
    Serial.print(", blueDifference=");Serial.print(blueDifference);
    Serial.println();

    // Maximum number of steps calculation
    uint16_t maxSteps = max(abs(redDifference), max(abs(greenDifference), abs(blueDifference)));
    Serial.print("maxSteps=");Serial.print(maxSteps);
    Serial.println();

    // Step argument cannot be greater than maximum steps
    steps = clamp(steps, 0, maxSteps);

    Serial.print("steps=");Serial.print(steps);
    Serial.println();

    //There is no change in colors
    if(steps == 0){
        return;
    }

    // Step fraction of total duration
    long stepDelay = clamp(duration, 0 , INT32_MAX) / steps;

    // Coefficient to calculate how much a color will increase/decrease each step
    double redCoefficient = redDifference / (double)steps;
    double greenCoefficient = greenDifference / (double)steps;
    double blueCoefficient = blueDifference / (double)steps;

    // Logs
    Serial.print(" redCoefficient=");Serial.print(redCoefficient);
    Serial.print(", greenCoefficient=");Serial.print(greenCoefficient);
    Serial.print(", blueCoefficient=");Serial.print(blueCoefficient);
    Serial.println();

    for (int i = steps ; i >= 0; i--){
        //Setting the new color
        this->setColor((uint16_t) (ceil(red - (i * redCoefficient))),
                       (uint16_t) (ceil(green - (i * greenCoefficient))),
                       (uint16_t) (ceil(blue - (i * blueCoefficient)))
        );

        // Logs
        Serial.print(this->red);
        Serial.print(",");Serial.print(this->green);
        Serial.print(",");Serial.print(this->blue);
        Serial.println();

        // Step delay
        delay((unsigned long)stepDelay);
    }
}

void Chameleon::fadeIn(int red, int green, int blue, long duration) {
    fadeIn(red, green, blue, writeRange, duration);
}

void Chameleon::fadeIn(int red, int green, int blue, int steps, long duration) {
    this->setColor(0,0,0);
    this->fadeTo(red,green,blue, steps, duration);
}

void Chameleon::fadeOut(int red, int green, int blue, long duration) {
    fadeOut(red,green,blue,writeRange, duration);
}

void Chameleon::fadeOut(int red, int green, int blue, int steps, long duration) {
    this->setColor(clamp(red, 0, writeRange),clamp(green, 0, writeRange),clamp(blue, 0, writeRange));
    this->fadeTo(0,0,0, steps, duration);
}

int Chameleon::clamp(int value, int lower, int upper){
    return max(lower, min(value, upper));
}