#ifndef _PBR_MATERIAL_CLASS_H
#define _PBR_MATERIAL_CLASS_H

#include <sRAT-RT/material.h>

class PBRMaterial : public Material
{
public:
    // Public constructor
    PBRMaterial(const std::vector<Texture>& textures_to_load, 
                const char* vert_sh_path, const char* frag_sh_path, 
                RenderPasses _shader_pass = DEFERRED_ILLUMINATION); // Textures have metadata only, actual loading done in constructor

    // Virtual methods/functions we have to define

    /// WARNING: WE OPERATE UNDER THE ASSUMPTION THAT THE TEXTURES 
    //              ARE IN THE SAME ORDER AS THEIR BINDINGS
    //              IN set_shader_uniforms() !!!!!
    void set_shader_uniforms(); 
    Shader* get_shader();
    bool reload_shader();
    
private:
    void load_textures(const std::vector<Texture>& textures_to_load);

    std::vector<Texture> mat_textures;     // Check Texture.type for "diffuse", "roughness", etc.
};

#endif