#ifndef _OBJECT_CLASS_H
#define _OBJECT_CLASS_H

#include <sRAT-RT/transform.h>

class Object
{
public:

    Transform get_transform()
    {
        return transform;
    }

    /// TODO: Add stuff here if needed

protected:
    Transform transform;
};

#endif