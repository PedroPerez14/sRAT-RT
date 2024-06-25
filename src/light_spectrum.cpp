#include <fstream>
#include <sstream>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <sRAT-RT/light_spectrum.h>
#include <iostream>

Spectrum::Spectrum(std::string spectrum_file, glm::vec3 resp_rgb = glm::vec3(1.0f,1.0f,1.0f))
{
    responses_rgb = resp_rgb;
    responses = new std::vector<light_response_sample>();
    n_samples = 0;
    std::ifstream f(spectrum_file);
    if(!f.is_open()) throw std::runtime_error("Could not open response curve!");
    std::string line_wls, line_resp;
    std::string s_wl, s_resp;
    float wl, resp_r, resp_g, resp_b;   // It can be XYZ and not rgb, be careful

    float max_resp = 0.0f;

    if(std::getline(f, line_wls) && std::getline(f, line_resp))
    {
        std::stringstream ss_wl(line_wls);
        std::stringstream ss_resp(line_resp);

        while(std::getline(ss_wl, s_wl, ',') && std::getline(ss_resp, s_resp, ','))
        {
            light_response_sample lrs;
            lrs.wavelength = std::stof(s_wl);
            lrs.response = std::stof(s_resp);

            responses->push_back(lrs);

            if(lrs.response > max_resp)
                max_resp = lrs.response;

            if(n_samples == 0)
                min_wl = std::stof(s_wl);
            max_wl = std::stof(s_wl);

            n_samples++;
        }
    }

    // normalization step
    for(int i = 0; i < responses->size(); i++)
    {
        responses->at(i).response /= max_resp;
        // std::cout << "NORMALIZATION SAMPLE " << i << " " << responses->at(i).wavelength << " " << responses->at(i).response << std::endl;
    }
    // gen_emission_tex_1d();          // sets the id class member
}

float Spectrum::get_wl_min() const
{
    return min_wl;
}

float Spectrum::get_wl_max() const
{
    return max_wl;
}

int Spectrum::get_n_samples() const
{
    return n_samples;
}

// unsigned int Spectrum::get_emission_tex_id() const
// {
//     return emission_tex_id;
// }

glm::vec3 Spectrum::get_responses_rgb() const
{
    return responses_rgb;
}

std::vector<light_response_sample>* Spectrum::get_responses() const
{
    return responses;
}

// void Spectrum::gen_emission_tex_1d()
// {
//     float* buffer_tex = new float[n_samples];
//     for(int i = 0; i < n_samples; i++)
//     {
//         buffer_tex[i] = responses->at(i).response;
//     }

//     glGenTextures(1, &emission_tex_id);
//     glBindTexture(GL_TEXTURE_1D, emission_tex_id);
//     glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

//     glTexImage1D(GL_TEXTURE_1D, 0, GL_RED, n_samples, 0, GL_RED, GL_FLOAT, buffer_tex);
//     delete buffer_tex;

// }
