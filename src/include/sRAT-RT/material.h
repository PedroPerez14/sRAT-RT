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
    
    virtual bool reload_shaders() = 0;          // Return false if the shaders could not be reloaded, i.e a compilation or linking error happened
    virtual ~Material() = 0;                    // Pure virtual

protected:    
};

#endif