#ifndef _RENDERABLE_OBJECT_CLASS_H_
#define _RENDERABLE_OBJECT_CLASS_H_

#include <sRAT-RT/object.h>
#include <sRAT-RT/material.h>

class RenderableObject : public Object
{

public:

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