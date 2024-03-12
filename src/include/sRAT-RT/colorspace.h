#ifndef COLORSPACE_H
#define COLORSPACE_H

#include <string>
#include <unordered_map>

enum colorspace{ ACES_2065_1, PROPHOTORGB, REC2020, SRGB};

// Maintainability hell (I couldn't come up with something better in the 30 secs I pondered this)
static const std::string colorspace_translations[] = 
{
"ACES_2065_1", 
"PROPHOTORGB", 
"REC2020", 
"SRGB"
};

static const std::unordered_map<std::string, colorspace> colorspace_translations_inv = 
{ 
    {"ACES_2065_1", ACES_2065_1},
    {"PROPHOTORGB", PROPHOTORGB},
    {"REC2020", REC2020},
    {"SRGB", SRGB},
};

// TODO: Add here more data like colorspace conversion tables and such?

#endif