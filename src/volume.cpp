#include <sRAT-RT/volume.h>

Volume::Volume(std::string spectral_coeffs_file_fullpath, glm::vec3 sigma_a_rgb, glm::vec3 sigma_s_rgb, glm::vec3 KD_rgb)
    : sigma_a_rgb(sigma_a_rgb), sigma_s_rgb(sigma_s_rgb), sigma_t_rgb(sigma_a_rgb + sigma_s_rgb), 
    albedo_rgb(sigma_s_rgb / sigma_t_rgb), KD_rgb(KD_rgb)
{
    rc = new ResponseCurve(spectral_coeffs_file_fullpath);
}

glm::vec3 Volume::get_sigma_s_rgb()
{
    return sigma_s_rgb;
}

glm::vec3 Volume::get_sigma_a_rgb()
{
    return sigma_a_rgb;
}

glm::vec3 Volume::get_sigma_t_rgb()
{
    return sigma_t_rgb;
}

glm::vec3 Volume::get_KD_rgb()
{
    return KD_rgb;
}

glm::vec3 Volume::get_albedo_rgb()
{
    return albedo_rgb;
}

ResponseCurve* Volume::get_response_curve()
{
    return rc;
}

unsigned int Volume::get_spectral_tex_id()
{
    return rc->get_tex_id();
}