#ifndef LIGHT_CLASS_H_
#define LIGHT_CLASS_H_

#include <sRAT-RT/object.h>
#include <sRAT-RT/light_spectrum.h>

enum light_type {DIR_LIGHT, POINT_LIGHT};

class Light : public Object
{

public:

    virtual Spectrum* get_spectrum() = 0;
    virtual void set_spectrum(Spectrum* spec) = 0;
    virtual float get_power_multiplier() = 0;
    virtual void set_power_multiplier(float pm) = 0;
    virtual light_type get_light_type() = 0;

protected:
    Spectrum* emission_spectrum;
    float power_multiplier;
    light_type l_type;
};


#endif