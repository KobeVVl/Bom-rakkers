#include "Arduino.h"
#include "Preset.h"

Preset::Preset(String code, int bananaPreset, String wire)
{
    _code = code;
    _bananaPreset = bananaPreset;
    _wire = wire;
}

Preset::Preset()
{
    _code = "";
    _bananaPreset = -1;
    _wire = "";
}

String Preset::getCode()
{
    return _code;
}

int Preset::getBananaPreset()
{
    return _bananaPreset;
}

String Preset::getWire()
{
    return _wire;
}