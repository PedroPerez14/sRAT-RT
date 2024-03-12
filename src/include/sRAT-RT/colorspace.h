#ifndef COLORSPACE_H
#define COLORSPACE_H

#include <string>
#include <unordered_map>

enum colorspace{ ACES2065_1, PROPHOTORGB, REC2020, SRGB};

// Maintainability hell (I couldn't come up with something better in the 30 secs I pondered this)
static const std::string colorspace_translations[] = 
{
"aces2065_1", 
"prophotorgb", 
"rec2020", 
"sRGB"
};

static const std::unordered_map<std::string, colorspace> colorspace_translations_inv = 
{ 
    {"aces2065_1", ACES2065_1},
    {"prophotorgb", PROPHOTORGB},
    {"rec2020", REC2020},
    {"sRGB", SRGB},
};

// TODO: Add here more data like colorspace conversion tables and such?

#endif