#ifndef _POINT_LIGHT_CLASS_H
#define _POINT_LIGHT_CLASS_H

#include <sRAT-RT/light.h>

class PointLight : public Light
{

public:
    PointLight(glm::vec3 _pos, Spectrum* _spectrum, float _power_multiplier, float att_c, float att_l, float att_q);

    glm::vec3 get_pos();
    void set_pos(glm::vec3 _pos);

    Spectrum* get_spectrum() override;
    void set_spectrum(Spectrum* spec) override;

    float get_power_multiplier() override;
    void set_power_multiplier(float pm) override;

    light_type get_light_type() override;

    glm::vec3 get_att_vec() const;
    void set_att_vec(glm::vec3 att);
    float get_att_constant() const;
    float get_att_linear() const;
    float get_att_quadratic() const;
    void set_att_constant(float constant);
    void set_att_linear(float linear);
    void set_att_quadratic(float quadratic);

protected:
    float att_constant;
    float att_linear;
    float att_quadratic;
};

#endif