#include <iostream>
#include <glad/gl.h>
#include <sRAT-RT/response_curve.h>

ResponseCurve::ResponseCurve(std::string respcurve_full_path)
{
    response_sample_vector = std::vector<response_sample>();
        response_sample_vector.empty();
        n_samples = 0;
        std::ifstream f(respcurve_full_path);
        if(!f.is_open()) throw std::runtime_error("Could not open response curve!");
        std::string line_wls, line_r, line_g, line_b;
        std::string s_wl, s_r, s_g, s_b;
        float wl, resp_r, resp_g, resp_b;   // It can be XYZ and not rgb, be careful
        
        if(std::getline(f, line_wls) && std::getline(f, line_r) 
        && std::getline(f, line_g) && std::getline(f, line_b))
        {
            std::stringstream ss_wl(line_wls);
            std::stringstream ss_r(line_r);
            std::stringstream ss_g(line_g);
            std::stringstream ss_b(line_b);

            while(std::getline(ss_wl, s_wl, ',') && std::getline(ss_r, s_r, ',') 
            && std::getline(ss_g, s_g, ',') && std::getline(ss_b, s_b, ','))
            {
                response_sample rs;
                rs.wavelength = std::stof(s_wl);
                rs.responses[0] = std::stof(s_r);
                rs.responses[1] = std::stof(s_r);
                rs.responses[2] = std::stof(s_b);

                response_sample_vector.push_back(rs);

                if(n_samples == 0)
                    wl_min = std::stof(s_wl);
                wl_max = std::stof(s_wl);

                n_samples++;
            }
        }
        generate_gltexture_from_data();
}

float ResponseCurve::get_wl_min() const
{
    return wl_min;
}

float ResponseCurve::get_wl_max() const
{
    return wl_max;
}

unsigned int ResponseCurve::get_n_samples() const
{
    return n_samples;
}

unsigned int ResponseCurve::get_tex_id() const
{
    return response_sample_tex_id;
}

std::vector<response_sample>* ResponseCurve::get_curve_data()
{
    return &response_sample_vector;
}

void ResponseCurve::generate_gltexture_from_data()
{
    float* buffer_tex = new float[n_samples * 3];
    for(int i = 0; i < response_sample_vector.size(); i++)
    {
        buffer_tex[i + 0] = response_sample_vector.at(i).responses[0];  // R
        buffer_tex[i + 1] = response_sample_vector.at(i).responses[1];  // G
        buffer_tex[i + 2] = response_sample_vector.at(i).responses[2];  // B
    }

    glGenTextures(1, &response_sample_tex_id);
    glBindTexture(GL_TEXTURE_1D, response_sample_tex_id);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, n_samples, 0, GL_RGB, GL_FLOAT, buffer_tex);
    delete buffer_tex;

    // std::cout << "Created 1D texture for response curve with " << n_samples << " samples, from "
    // << wl_min << "nm to " << wl_max << "nm. RGB[0] : " << response_sample_vector[0].responses[0] 
    // << ", " << response_sample_vector[0].responses[1] << ", " << response_sample_vector[0].responses[2] 
    // << std::endl;
}
