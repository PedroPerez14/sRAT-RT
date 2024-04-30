#ifndef LIGHT_SPECTRUM_H_
#define LIGHT_SPECTRUM_H_

#include <vector>
#include <string>

struct light_response_sample
{
    float wavelength;   // Several values (400 to 700nm)
    float response;
};

class Spectrum
{
public:
    Spectrum(std::string spectrum_file, glm::vec3 resp_rgb);
    
    float get_wl_min() const;
    float get_wl_max() const;
    int get_n_samples() const;
    unsigned int get_emission_tex_id() const;
    glm::vec3 get_responses_rgb() const;
    std::vector<light_response_sample>* get_responses() const;
    

private:
    std::vector<light_response_sample>* responses;
    glm::vec3 responses_rgb;
    float min_wl;
    float max_wl;
    int n_samples;
    unsigned int emission_tex_id;

    void gen_emission_tex_1d();
};

#endif