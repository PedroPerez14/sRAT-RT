#ifndef DIR_LIGHT_CLASS_H_
#define DIR_LIGHT_CLASS_H_

#include <glm/glm.hpp>
//#include <glm/gtc/type_ptr.hpp>
#include <sRAT-RT/light.h>

class DirLight : public Light
{

public:
    DirLight(glm::vec3 _dir, Spectrum* _spectrum, float _power_multiplier);

    glm::vec3 get_dir();
    void set_dir(glm::vec3 _dir);

    Spectrum* get_spectrum() override;
    void set_spectrum(Spectrum* spec) override;

protected:
    glm::vec3 dir;                  /// Direction FROM the light, not towards it!
};

#endif