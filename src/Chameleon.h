#ifndef CHAMELEON_H
#define CHAMELEON_H
#include "Arduino.h"

class Chameleon {
public:
    Chameleon(uint8_t redPin, uint8_t greenPin, uint8_t bluePin, int writeRange);
    void addColorCommand(int red, int green, int blue, int steps, long duration);
    void changeTo(int red, int green, int blue);
    void changeTo(int red, int green, int blue, long duration);
    void fadeTo(int red, int green, int blue, long duration);
    void fadeTo(int red, int green, int blue, int steps, long duration);
    void fadeIn(int red, int green, int blue, long duration);
    void fadeIn(int red, int green, int blue, int steps, long duration);
    void fadeOut(int red, int green, int blue, long duration);
    void fadeOut(int red, int green, int blue, int steps, long duration);
    void info();
    void execute();
    void reset();

private:
    class ColorChangeCommand {
    public:
        int red = 0, green = 0, blue = 0;
        double redCoefficient = 0, greenCoefficient = 0, blueCoefficient = 0;
        int steps = 0;
        long duration = 0;
        ColorChangeCommand(int red, int green, int blue, double redCoefficient, double greenCoefficient, double blueCoefficient, int steps, long duration){
            this->red = red;
            this->green = green;
            this->blue = blue;
            this->redCoefficient = redCoefficient;
            this->greenCoefficient = greenCoefficient;
            this->blueCoefficient = blueCoefficient;
            this->steps = steps;
            this->duration = duration;
        }
        ColorChangeCommand(int red, int green, int blue){
            this->red = red;
            this->green = green;
            this->blue = blue;
            this->steps = 1;
        }
        ColorChangeCommand()=default;
    };
    std::vector<ColorChangeCommand> commands;
    uint8_t redPin, greenPin, bluePin;
    int red, green, blue;
    int transitionIndex = 0;
    int writeRange;
    int commandIndex = 0;
    unsigned long startTime = 0;

    void setColor(int red, int green, int blue);
    int clamp(int value, int lower, int upper);
};

#endif //CHAMELEON_H


