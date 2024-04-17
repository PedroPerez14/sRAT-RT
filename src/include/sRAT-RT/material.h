#ifndef _MATERIAL_CLASS_H_
#define _MATERIAL_CLASS_H_

#include <string>
#include <sRAT-RT/shader.h>
#include <sRAT-RT/render_passes.h>

struct Texture {
    unsigned int id;        // OpenGL's texture ID
    unsigned int binding;   // for example, diff_texture<N>, where N = binding
    std::string type;       // diff_texture, normal_texture, etc. among others
    std::string path;       // we store the path of the texture to compare with other textures
};

class Material
{
public:
    std::string mat_name;
    RenderPasses shader_pass;                   // Which draw pass does the material's shader belong to?
    
    virtual void set_shader_uniforms() = 0;     // Sets the uniforms for the shader associated with this material. Also binds any necessary textures.
    virtual Shader* get_shader() = 0;           // Returns the current shader in case we need to use() it
    virtual bool reload_shader() = 0;           // Return false if the shader could not be reloaded, i.e a compilation or linking error happened

    /// TODO: I should have a Shader Cache to avoid creating multiple instances of the same shader.
    // I can do it either the easy way or the C-ish way: 
    //      Easy way:               Map with Key: shader file paths and Value: std::shared_ptr to the shader. If doesn't exist, create it
    //      C-ish way (torture):    Map with Key: shader file paths and Value: pair of pointer to the shader and ref counter. Need to track it [DON'T]
    // Probably I should also add an unsigned int or 64 bit mask to order the render passes of the same type? I don't know [NOT REQUIRED, FOR NOW]

    virtual ~Material()
    {
        
    }

private:
    // Each material will have a few uniforms (i.e roughness, specularity, etc)
    //      but those will be defined for each child material.
    //      A map of <std::string, std::vector<textures or uints> would be overengineering

    // Each material will have one shader attached to it
    Shader* mat_shader;
};

#endif