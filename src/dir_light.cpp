#include <sRAT-RT/dir_light.h>

DirLight::DirLight(glm::vec3 _dir, Spectrum* _spectrum, float _power_multiplier = 1.0f)
{
    emission_spectrum = _spectrum;
    power_multiplier = _power_multiplier;
    dir = _dir;
    l_type = DIR_LIGHT;
}

glm::vec3 DirLight::get_dir()
{
    return dir;
}

void DirLight::set_dir(glm::vec3 _dir)
{
    dir = _dir;
}

Spectrum* DirLight::get_spectrum()
{
    return emission_spectrum;
}

void DirLight::set_spectrum(Spectrum* spec)
{
    emission_spectrum = spec;
}

float DirLight::get_power_multiplier()
{
    return power_multiplier;
}

void DirLight::set_power_multiplier(float pm)
{
    power_multiplier = pm;
}

light_type DirLight::get_light_type()
{
    return l_type;
}
