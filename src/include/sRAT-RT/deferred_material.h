#ifndef _DEFERRED_MATERIAL_CLASS_H
#define _DEFERRED_MATERIAL_CLASS_H

#include <sRAT-RT/shader.h>

class DeferredMaterial : public Material
{
public:
    virtual void set_shader_uniforms() = 0;                 // We pass everything we need via the gbuffer (uniforms only to the )
    virtual Shader* get_shader(RenderPass pass) = 0;        // Returns the current shader in case we need to use() it
    virtual ~DeferredMaterial() = 0;                        // Pure virtual class

protected:
    Shader* shader_geometry_pass;
    Shader* shader_lighting_pass; 
};

#endif