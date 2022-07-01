#ifndef Preset_h
#define Preset_h

#include "Arduino.h"
#include "Question.h"

#ifndef PLANTTIME
#define PLANTTIME 5
#endif

class Preset
{
private:
    String _code;
    int _bananaPreset;
    int _wire;
    int _amQuestions;
    int _plantTime;

public:
    Preset(String code, int bananaPreset, int wire, int amQuestions, int plantTime);
    Preset(int amQuestions, int plantTime);
    Preset(String code, int bananaPreset, int wire);
    Preset();
    String getCode();
    int getBananaPreset();
    int getWire();
    int getAmQuestions();
    int getPlantTime();
};

#endif