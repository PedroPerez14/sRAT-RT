#ifndef LIGHT_SPECTRUM_H_
#define LIGHT_SPECTRUM_H_

#include <vector>
#include <string>

struct light_response_sample
{
    float wavelength;   // Several values (400 to 700nm) or 3 (R,G,B)
    float response;
};

class Spectrum
{
public:
    Spectrum(std::string spectrum_file, bool rgb);

    std::vector<light_response_sample>* get_responses();

    bool is_rgb_response();

private:
    std::vector<light_response_sample>* responses;
    bool is_rgb;        // true for rgb, false for spectral
    float min_wl;
    float max_wl;
    int n_samples;
};

#endif