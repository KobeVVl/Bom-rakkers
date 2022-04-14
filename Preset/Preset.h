#ifndef Preset_h
#define Preset_h

#include "Arduino.h"

class Preset
{
private:
    String _code;
    int _bananaPreset;
    String _wire;

public:
    Preset(String code, int bananaPreset, String wire);
    Preset();
    String getCode();
    int getBananaPreset();
    String getWire();
};

#endif