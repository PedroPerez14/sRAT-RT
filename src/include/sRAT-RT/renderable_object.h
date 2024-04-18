#ifndef _RENDERABLE_OBJECT_CLASS_H_
#define _RENDERABLE_OBJECT_CLASS_H_

#include <sRAT-RT/object.h>
#include <sRAT-RT/material.h>

class RenderableObject : public Object
{

public:
    /// WARNING: You can use the material's shader or a different one.
    //      To use the material one, you can set the shader parameter to RO->get_material()
    //      or you can set it to nullptr. This will call mat->set_shader_uniforms().
    //
    //      If you override the shader param MAKE SURE YOU HAVE SET THE UNIFORMS CORRECTLY!
    virtual void draw(Shader* shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection) = 0;

    Material* get_material()
    {
        return mat;
    }

    void set_material(Material* _mat)
    {
        mat = _mat;
    }

protected:
    Material* mat;
};

#endif