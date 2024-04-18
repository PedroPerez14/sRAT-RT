#ifndef _PBR_MATERIAL_CLASS_H
#define _PBR_MATERIAL_CLASS_H

#include <sRAT-RT/forward_material.h>

class PBRMaterial : public DeferredMaterial
{
public:
    // Public constructor
    PBRMaterial(const std::vector<Texture>& textures_to_load, 
                const char* vert_sh_geom_path, const char* frag_sh_geom_path, 
                const char* vert_sh_light_path, const char* frag_sh_light_path); // Textures have metadata only, actual loading done in constructor

    // Virtual methods/functions we have to define
    void set_shader_uniforms(RenderPass pass) override;
    Shader* get_shader(RenderPass pass) override;
    bool reload_shaders() override;
    
private:
    void load_textures(const std::vector<Texture>& textures_to_load);
    std::vector<Texture> mat_textures;                              // Check Texture.type for "diffuse", "roughness", etc.
};

#endif