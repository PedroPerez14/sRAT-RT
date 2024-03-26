#ifndef RESPONSE_CURVE_CLASS_H
#define RESPONSE_CURVE_CLASS_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

struct response_sample
{
    float wavelength;
    float responses[3];
};

class ResponseCurve
{
public:
    ResponseCurve(std::string respcurve_full_path);

    float get_wl_min() const;
    float get_wl_max() const;
    unsigned int get_n_samples() const;
    unsigned int get_tex_id() const;
    std::vector<response_sample>* get_curve_data();

private:

    float wl_min;
    float wl_max;
    unsigned int n_samples;
    unsigned int response_sample_tex_id;
    std::vector<response_sample> response_sample_vector;

    void generate_gltexture_from_data();    
};

#endif