#ifndef _RENDERABLE_OBJECT_CLASS_H_
#define _RENDERABLE_OBJECT_CLASS_H_

#include <sRAT-RT/object.h>
#include <sRAT-RT/material.h>

class RenderableObject : public Object
{
public:
    /// TODO: Do I need to put anything else here for now?
    /// TODO: (part 2): Inherited by Model, maybe also by Particle in the (distant) future?
    virtual void draw() = 0;
protected:
    Material* mat;          
};

#endif