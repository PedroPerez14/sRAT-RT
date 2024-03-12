#ifndef SETTINGS_CLASS_H
#define SETTINGS_CLASS_H

#include <string>
#include <sRAT-RT/colorspace.h>

class Settings
{

private:
    std::string path_LUTs_uplifting;
    std::string file_extension_LUTs_uplifting;
    std::string scene_to_load;                          // TODO: Not supported yet, will change it
    colorspace colorspace_render;                       // Will run tests only in sRGB for now TODO: test others
    unsigned int num_wavelengths;                       // Works best with multiples of 4
    unsigned int window_width;
    unsigned int window_height;
    float wl_min;
    float wl_max;
    std::string window_name;

    void set_defaults();

public:  
    Settings();                                         // Default values, can be overriden with load_settings
    Settings(const std::string& config_file);           // Load settings directly from file

    bool load_settings(const std::string& config_file); // Check if settings were loaded, if not, defaults will be used
    bool save_settings(const std::string& config_file); // Check if saved correctly, it might generate a corrupt file

    std::string get_path_LUTs() const;
    std::string get_file_extension_LUTs() const;
    std::string get_scene() const;
    colorspace get_colorspace() const;
    unsigned int get_num_wavelengths() const;
    unsigned int get_window_width() const;
    unsigned int get_window_height() const;
    float get_wl_min() const;
    float get_wl_max() const;
    std::string get_window_name() const;

    void set_path_LUTs(std::string path);
    void set_file_extension_LUTs(std::string ext);
    void set_scene(std::string scene);
    void set_colorspace(colorspace csp);
    void set_num_wavelengths(unsigned int wls);
    void set_window_width(unsigned int w);
    void set_window_height(unsigned int h);
    void set_wl_min(float wlmin);
    void set_wl_max(float wlmax);
    void set_window_name(std::string winname);

    std::string to_string();
};

#endif