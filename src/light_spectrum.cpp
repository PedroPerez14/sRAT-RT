#include <fstream>
#include <sstream>
#include <sRAT-RT/light_spectrum.h>

Spectrum::Spectrum(std::string spectrum_file, bool rgb) : is_rgb(rgb)
{
    responses = new std::vector<light_response_sample>();
    n_samples = 0;
    std::ifstream f(spectrum_file);
    if(!f.is_open()) throw std::runtime_error("Could not open response curve!");
    std::string line_wls, line_resp;
    std::string s_wl, s_resp;
    float wl, resp_r, resp_g, resp_b;   // It can be XYZ and not rgb, be careful
    
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

            if(n_samples == 0)
                min_wl = std::stof(s_wl);
            max_wl = std::stof(s_wl);

            n_samples++;
        }
    }
}

std::vector<light_response_sample>* Spectrum::get_responses()
{
    return responses;
}

bool Spectrum::is_rgb_response()
{
    return is_rgb;
}