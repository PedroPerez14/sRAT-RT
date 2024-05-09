#include <sRAT-RT/point_light.h>

PointLight::PointLight(glm::vec3 _pos, Spectrum* _spectrum, float _power_multiplier, float att_c, float att_l, float att_q)
{
    transform = new Transform(_pos);
    emission_spectrum = _spectrum;
    power_multiplier = _power_multiplier;
    l_type = POINT_LIGHT;
    att_constant = att_c;
    att_linear = att_l;
    att_quadratic = att_q;
}

glm::vec3 PointLight::get_pos()
{
    return transform->get_pos();
}

void PointLight::set_pos(glm::vec3 _pos)
{
    transform->set_pos(_pos);
}

Spectrum* PointLight::get_spectrum()
{
    return emission_spectrum;
}

void PointLight::set_spectrum(Spectrum* spec)
{
    emission_spectrum = spec;
}

float PointLight::get_power_multiplier()
{
    return power_multiplier;
}

void PointLight::set_power_multiplier(float pm)
{
    power_multiplier = pm;
}

light_type PointLight::get_light_type()
{
    return l_type;
}

glm::vec3 PointLight::get_att_vec() const
{
    return glm::vec3(att_constant, att_linear, att_quadratic);
}

void PointLight::set_att_vec(glm::vec3 att)
{
    att_constant = att.x;
    att_linear = att.y;
    att_quadratic = att.z;
}

float PointLight::get_att_constant() const
{
    return att_constant;
}

float PointLight::get_att_linear() const
{
    return att_linear;
}

float PointLight::get_att_quadratic() const
{
    return att_quadratic;
}

void PointLight::set_att_constant(float constant)
{
    att_constant = constant;
}

void PointLight::set_att_linear(float linear)
{
    att_linear = linear;
}

void PointLight::set_att_quadratic(float quadratic)
{
    att_quadratic = quadratic;
}
