#ifndef _VOLUME_CLASS_H_
#define _VOLUME_CLASS_H_

#include <glm/glm.hpp>
#include <sRAT-RT/response_curve.h>

// homogeneous isotropic infinite volume, if i need bounded / heterogeneous volumes
// i will update the class and refactor accordingly but for now it's not necessary
class Volume
{
private:
    ResponseCurve* rc;           // R channel --> sigma_a, G channel --> sigma_s, B channel --> Kd (unused)

    glm::vec3 sigma_a_rgb;
    glm::vec3 sigma_s_rgb;
    glm::vec3 sigma_t_rgb;
    glm::vec3 albedo_rgb;
public:
    Volume(std::string spectral_coeffs_file_fullpath, glm::vec3 sigma_a_rgb, glm::vec3 sigma_s_rgb);
    ~Volume();

    glm::vec3 get_sigma_s_rgb();
    glm::vec3 get_sigma_a_rgb();
    glm::vec3 get_sigma_t_rgb();
    glm::vec3 get_albedo_rgb();

    ResponseCurve* get_response_curve();
    unsigned int get_spectral_tex_id();
};

#endif