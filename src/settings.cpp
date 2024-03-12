#include <iostream>
#include <sRAT-RT/settings.h>

#define MINI_CASE_SENSITIVE
#include <mINI/mINI.h>

Settings::Settings()
{
    set_defaults();
}

void Settings::set_defaults()
{
    // Default settings
    path_LUTs_uplifting = "./";
    file_extension_LUTs_uplifting = ".coeff";
    scene_to_load = "./scenes/test_uplifting.scene";    // DUMMY, NOT IMPLEMENTED YET
    colorspace_render = SRGB;
    num_wavelengths = 4;
    window_width = 800;
    window_height = 600;
}

Settings::Settings(const std::string& config_file)
{
    // In case we can't find some settings, we first set them to their default values in case we don't set them later
    set_defaults();
    std::cout << "SETTINGS::STATUS::LOADING: From file " << config_file << std::endl;
    if(!load_settings(config_file))
    {
        std::cout << "SETTINGS::STATUS::LOAD_ERROR: Could not load the settings for the renderer! Using default settings..." << std::endl;
        set_defaults();
    }
    else
    {
        std::cout << "SETTINGS::STATUS::LOAD_SUCCESS: Settings loaded correctly!" << std::endl;
    }
}

bool Settings::load_settings(const std::string& config_file)
{
    // Read and process the file with the help of an external library
    mINI::INIFile file(config_file);
    mINI::INIStructure ini;
    bool read_success = file.read(ini);

    if(!read_success)
    {
        std::cout << "SETTINGS::STATUS::LOAD_ERROR::READ_ERROR: Config .ini " << config_file << " could not be read!" << std::endl;
        return false;
    }

    // Read values
    if(ini.has("SETTINGS"))
    {
        if(ini["SETTINGS"].has("path_LUTs_uplifting"))
            path_LUTs_uplifting = ini["SETTINGS"]["path_LUTs_uplifting"];
        if(ini["SETTINGS"].has("file_extension_LUTs_uplifting"))
            file_extension_LUTs_uplifting = ini["SETTINGS"]["file_extension_LUTs_uplifting"];
        if(ini["SETTINGS"].has("scene_to_load"))
            scene_to_load = ini["SETTINGS"]["scene_to_load"];
        if(ini["SETTINGS"].has("colorspace_render"))
        {
            std::string aux = ini["SETTINGS"]["colorspace_render"];
            colorspace_render = colorspace_translations_inv.at(aux);
        }
        if(ini["SETTINGS"].has("num_wavelengths"))
            num_wavelengths = std::stoul(ini["SETTINGS"]["num_wavelengths"]);
        if(ini["SETTINGS"].has("window_width"))
            num_wavelengths = std::stoul(ini["SETTINGS"]["window_width"]);
        if(ini["SETTINGS"].has("window_height"))
            num_wavelengths = std::stoul(ini["SETTINGS"]["window_height"]);
    }
    else
    {
        std::cout << "SETTINGS::STATUS::LOAD_ERROR::READ_ERROR: [SETTINGS] not found in config file " << config_file << std::endl;
        std::cout << "Using default settings instead" << std::endl;
        return false;
    }
    return true;
}

bool Settings::save_settings(const std::string& config_file)
{
    // create a file instance
    mINI::INIFile file(config_file);
    mINI::INIStructure ini;

    // Populate it with our data
    ini["SETTINGS"]["path_LUTs_uplifting"] = path_LUTs_uplifting;
    ini["SETTINGS"]["file_extension_LUTs_uplifting"] = file_extension_LUTs_uplifting;
    ini["SETTINGS"]["scene_to_load"] = scene_to_load;
    ini["SETTINGS"]["colorspace_render"] = colorspace_translations[colorspace_render];
    ini["SETTINGS"]["num_wavelengths"] = std::to_string(num_wavelengths);
    ini["SETTINGS"]["window_width"] = std::to_string(window_width);
    ini["SETTINGS"]["window_height"] = std::to_string(window_height);

    // Try to write it, it'll overwrite files with matching names. 2nd parameter is for pretty printing
    bool success = file.generate(ini, true);
    if(!success)
    {
        std::cout << "SETTINGS::STATUS::WRITE_ERROR: Error while saving the settings file " << config_file << "! Check that the path folders exist and other possible errors!" << std::endl;
    }
    return success;
}


// Getters

std::string Settings::get_path_LUTs() const
{
    return path_LUTs_uplifting;
}

std::string Settings::get_file_extension_LUTs() const
{
    return file_extension_LUTs_uplifting;
}

std::string Settings::get_scene() const
{
    return scene_to_load;
}

colorspace Settings::get_colorspace() const
{
    return colorspace_render;
}

unsigned int Settings::get_num_wavelengths() const
{
    return num_wavelengths;
}

unsigned int Settings::get_window_width() const
{
    return window_width;
}

unsigned int Settings::get_window_height() const
{
    return window_height;
}

float Settings::get_wl_min() const
{
    return wl_min;
}

float Settings::get_wl_max() const
{
    return wl_max;
}

std::string Settings::get_window_name() const
{
    return window_name;
}

// Setters

void Settings::set_path_LUTs(std::string path)
{
    path_LUTs_uplifting = path;
}

void Settings::set_file_extension_LUTs(std::string ext)
{
    file_extension_LUTs_uplifting = ext;
}

void Settings::set_scene(std::string scene)
{
    scene_to_load = scene;
}

void Settings::set_colorspace(colorspace csp)
{
    colorspace_render = csp;
}

void Settings::set_num_wavelengths(unsigned int wls)
{
    num_wavelengths = wls;
}

void Settings::set_window_width(unsigned int w)
{
    window_width = w;
}

void Settings::set_window_height(unsigned int h)
{
    window_height = h;
}

void Settings::set_wl_min(float wlmin)
{
    wl_min = wlmin;
}

void Settings::set_wl_max(float wlmax)
{
    wl_max = wlmax;
}

void Settings::set_window_name(std::string winname)
{
    window_name = winname;
}


// To String
std::string Settings::to_string()
{
    std::string settings_str = "SETTINGS_INFO: \n[\n\tpath_LUTs: " + path_LUTs_uplifting
                + "\n\tfile_extension_LUTs: " + file_extension_LUTs_uplifting
                + "\n\tscene_to_load: " + scene_to_load
                + "\n\tcolorspace_render: " + colorspace_translations[colorspace_render]
                + "\n\tnum_wavelengths: " + std::to_string(num_wavelengths);
                + "\n\twindow_width: " + std::to_string(window_width);
                + "\n\twindow_height: " + std::to_string(window_height);
                + "\n\twl_min: " + std::to_string(wl_min);
                + "\n\twl_max: " + std::to_string(wl_max);
                + "\n\twindow_name: " + window_name;
                + "\n]";
    return settings_str;
}
