#ifndef _FORWARD_MATERIAL_CLASS_H
#define _FORWARD_MATERIAL_CLASS_H

#include <sRAT-RT/shader.h>

class ForwardMaterial : public Material
{
public:
    virtual void set_shader_uniforms() = 0; // Sets the uniforms for the shader associated with this material. Also binds any necessary textures.
    virtual Shader* get_shader() = 0;       // Returns the current shader in case we need to use() it
    virtual ~ForwardMaterial() = 0;         // Pure virtual class

protected:
    Shader* shader_forward_pass;
};

#endif