#ifndef _TRANSFORM_CLASS_H_
#define _TRANSFORM_CLASS_H_

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Transform
{
public:

Transform(glm::vec3 pos = glm::vec3(), glm::vec3 rot = glm::vec3(), 
                    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f))
                    : pos(pos), rot(rot), scale(scale)
{
    // empty, do I need anything else?
}

inline glm::mat4 get_model() const
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::rotate(model, rot.x, const glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, rot.y, const glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, rot.z, const glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);

    return model;
}

// getters
glm::vec3 get_pos() const   { return pos; }
glm::vec3 get_rot() const   { return rot; }
glm::vec3 get_scale() const { return scale; }

// setters
void set_pos(const glm::vec3& _pos) { pos = _pos; }
void set_rot(const glm::vec3& _rot) { rot = _rot; }
void set_scale(const glm::vec3& _scale) { scale = _scale; }

private:
    glm::vec3 pos;
    glm::vec3 rot;
    glm::vec3 scale;
};


#endif