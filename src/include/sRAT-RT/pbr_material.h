#ifndef _PBR_MATERIAL_CLASS_H
#define _PBR_MATERIAL_CLASS_H

#include <sRAT-RT/material.h>

class PBRMaterial : public Material
{
public:
    // Public constructor
    PBRMaterial(const std::vector<Texture>& textures_to_load, 
        const char* geom_pass_v_shader_path, const char* geom_pass_f_shader_path); // Textures have metadata only, actual loading done in constructor

    // Virtual methods/functions we have to define
    bool reload_shader() override;
    void set_shader_uniforms(glm::mat4 model, glm::mat4 view, glm::mat4 projection) override;
    Shader* get_shader() override;
    
    
private:
    void load_textures(const std::vector<Texture>& textures_to_load);
    std::vector<Texture> mat_textures;                              // Check Texture.type for "diffuse", "roughness", etc.
};

#endif