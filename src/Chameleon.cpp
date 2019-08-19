#include "Chameleon.h"
#include "Arduino.h"

Chameleon::Chameleon(uint8_t redPin, uint8_t greenPin, uint8_t bluePin, int writeRange) {
    this->redPin = redPin;
    this->greenPin = greenPin;
    this->bluePin = bluePin;
    this->writeRange = clamp(writeRange, 1, INT32_MAX);
    analogWriteRange(static_cast<uint32_t>(this->writeRange));
}

//Change the color.
void Chameleon::setColor(int red, int green, int blue){
    this->red = clamp(red, 0 , this->writeRange);
    this->green = clamp(green, 0 , this->writeRange);
    this->blue = clamp(blue, 0 , this->writeRange);
    Serial.print("R,G,B=");Serial.print(red);Serial.print(",");Serial.print(green);Serial.print(",");Serial.println(blue);

    analogWrite(this->redPin, this->red);
    analogWrite(this->greenPin, this->green);
    analogWrite(this->bluePin, this->blue);
}

//Clear the color commands list and change the color. This color will not change until another function is executed.
//This method doesnt add a command to list.
//Doesnt need to call execute() method to do any change.
void Chameleon::changeTo(int red, int green, int blue){
    commands.clear();
    setColor(red,green,blue);
}

//Adds a command that will do a color change and keeps that color by duration time.
//After this, the next command will be executed.
//Need to call execute() method to do any change.
void Chameleon::changeTo(int red, int green, int blue, long duration){
    addColorCommand(red,green,blue,1,duration);
}

//Fades to color given a duration, using the writeRange as the number of steps.
//Need to call execute() method to do any change.
void Chameleon::fadeTo(int red, int green, int blue, long duration){
    // Setting max possible value of step, so it will be clamped next method, to a valid number.
    this->fadeTo(red, green, blue, this->writeRange, duration);
}

//Fades to color in n steps, given a duration.
//Need to call execute() method to do any change.
void Chameleon::fadeTo(int red, int green, int blue, int steps, long duration){
    addColorCommand(red, green, blue, steps, duration);
}

//Adds a Color command to queue with all necessary data to do a color change or fade. That command will be
// executed in the method execute().
void Chameleon::addColorCommand(int red, int green, int blue, int steps, long duration){

    // Colors cannot have values greater than writeRange and lower than 0
    red = this->clamp(red, 0, this->writeRange);
    green = this->clamp(green, 0, this->writeRange);
    blue = this->clamp(blue, 0, this->writeRange);

    // Difference from initial color to new color
    int redDifference;
    int greenDifference;
    int blueDifference;

    // If there is no command then use the actual r,g,b values
    if(commands.empty()) {
        redDifference = red - this->red;
        greenDifference = green - this->green;
        blueDifference = blue - this->blue;
    } else {
        redDifference = red - commands.back().red;
        greenDifference = green - commands.back().green;
        blueDifference = blue - commands.back().blue;
    }

    // Maximum number of steps calculation
    uint16_t maxSteps = max(abs(redDifference), max(abs(greenDifference), abs(blueDifference)));

    // Step argument cannot be greater than maximum steps
    steps = clamp(steps, 0, maxSteps);

    //There is no change in colors
    if(steps == 0){
        return;
    }

    // Coefficient to calculate how much a color will increase/decrease each step
    double redCoefficient = redDifference / (double)steps;
    double greenCoefficient = greenDifference / (double)steps;
    double blueCoefficient = blueDifference / (double)steps;

    // Logs
    Serial.print(" redCoefficient=");Serial.print(redCoefficient);
    Serial.print(", greenCoefficient=");Serial.print(greenCoefficient);
    Serial.print(", blueCoefficient=");Serial.println(blueCoefficient);

    commands.emplace_back(ColorChangeCommand(red, green, blue, redCoefficient, greenCoefficient, blueCoefficient, steps, duration));
}

//Fades from black(leds off) to argument color, given a duration and using writeRange as number of steps.
//Need to call execute() method to do any change.
void Chameleon::fadeIn(int red, int green, int blue, long duration) {
    fadeIn(red, green, blue, writeRange, duration);
}

//Fades from black(leds off) to argument color in n steps, given a duration.
//Need to call execute() method to do any change.
void Chameleon::fadeIn(int red, int green, int blue, int steps, long duration) {
    commands.emplace_back(ColorChangeCommand(0,0,0));
    this->fadeTo(red,green,blue, steps, duration);
}

//Fades from argument color to black(leds off), given a duration and using writeRange as number of steps.
//Need to call execute() method to do any change.
void Chameleon::fadeOut(int red, int green, int blue, long duration) {
    fadeOut(red,green,blue,writeRange, duration);
}

//Fades from argument color to black(leds off) in n steps, given a duration.
//Need to call execute() method to do any change.
void Chameleon::fadeOut(int red, int green, int blue, int steps, long duration) {
    commands.emplace_back(ColorChangeCommand(clamp(red, 0, writeRange),clamp(green, 0, writeRange),clamp(blue, 0, writeRange)));
    this->fadeTo(0,0,0, steps, duration);
}

//Assures that value is between lower and upper.
int Chameleon::clamp(int value, int lower, int upper){
    return (unsigned int) max(lower, min(value, upper));
}

// Executes the color command chain.
void Chameleon::execute() {

    //There is no commands to execute, exiting.
    if (commands.empty()) {
        return;
    }

    // First execution of a command
    if (startTime == 0) {
        startTime = millis();
        transitionIndex = 1;
    }

    ColorChangeCommand command = commands[commandIndex];

    //Check if its the end of command duration
    if (millis() > startTime + command.duration) {

        setColor((uint16_t) command.red,
                 (uint16_t) command.green,
                 (uint16_t) command.blue);

        transitionIndex = 1;
        startTime = 0;

        //Check if it have more commands to execute
        if (commandIndex < (commands.size() - 1)) {
            //Next command
            commandIndex++;
        } else {
            //Executed last command, going back to first command.
            commandIndex = 0;
        }
        return;
    }

    //If its to change to one color, no fade
    if(command.steps == 1){
        setColor((uint16_t) command.red,
                 (uint16_t) command.green,
                 (uint16_t) command.blue);
    }

    //If its to fade
    //Check if its time to do a color transition
    if (millis() > startTime + (command.duration / command.steps * transitionIndex) && transitionIndex < command.steps) {
        setColor((uint16_t) (ceil(command.red - ((command.steps - transitionIndex) * command.redCoefficient))),
                 (uint16_t) (ceil(command.green - ((command.steps - transitionIndex) * command.greenCoefficient))),
                 (uint16_t) (ceil(command.blue - ((command.steps - transitionIndex) * command.blueCoefficient))));
        transitionIndex++;
    }
}

//Returns information about the actual color, color commands and indexes.
void Chameleon::info() {

}

//Removes all commands from the list.
void Chameleon::reset(){
    commands.clear();
}