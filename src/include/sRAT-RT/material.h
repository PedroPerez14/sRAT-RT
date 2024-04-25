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
    
    virtual bool reload_shader() = 0;           // Return false if the shaders could not be reloaded, i.e a compilation or linking error happened
    virtual void set_shader_uniforms(glm::mat4 model, glm::mat4 view, glm::mat4 projection) = 0;
    virtual Shader* get_shader() = 0;           // Returns the current shader in case we need to use() it
    virtual ~Material() = 0;                    // Pure virtual

    RenderPasses get_pass() { return render_pass; }

protected:
    int mat_id;                                 // Big deferred shader
    Shader* mat_shader;
    RenderPasses render_pass;                   // hard-set by each material constructor (i.e pbr will be deferred, maybe translucid will be forward)
};

#endif