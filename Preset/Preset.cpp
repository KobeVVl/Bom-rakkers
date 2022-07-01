#include "Arduino.h"
#include "Preset.h"

Preset::Preset(String code, int bananaPreset, int wire, int amQuestions, int plantTime)
{
    _code = code;
    _bananaPreset = bananaPreset;
    _wire = wire;
    _amQuestions = amQuestions;
    _plantTime = plantTime;
}

Preset::Preset(int amQuestions, int plantTime)
{
    _code = "";
    _bananaPreset = -1;
    _wire = -1;
    _amQuestions = amQuestions;
    _plantTime = plantTime;
}

Preset::Preset(String code, int bananaPreset, int wire)
{
    _code = code;
    _bananaPreset = bananaPreset;
    _wire = wire;
    _amQuestions = 0;
    _plantTime = PLANTTIME;
}

Preset::Preset()
{
    _code = "";
    _bananaPreset = -1;
    _wire = -1;
    _amQuestions = 0;
    _plantTime = PLANTTIME;
}

String Preset::getCode()
{
    return _code;
}

int Preset::getBananaPreset()
{
    return _bananaPreset;
}

int Preset::getWire()
{
    return _wire;
}

int Preset::getAmQuestions()
{
    return _amQuestions;
}

int Preset::getPlantTime()
{
    return _plantTime;
}