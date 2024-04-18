#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include <string>
#include <vector>
#include <glm/glm.hpp>

#include <sRAT-RT/shader.h>

// Adapted from https://learnopengl.com/Model-Loading/Mesh

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;

    std::string to_string() const {
        std::string s = "[ " + std::to_string(Position.x) + " " + std::to_string(Position.y) + " " + std::to_string(Position.z) + " ]\n";
        return s;
    }
};

// struct Texture {
//     unsigned int id;
//     std::string type;
//     std::string path;  // we store the path of the texture to compare with other textures
// };

class Mesh {
public:
    // mesh data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    //std::vector<Texture>      textures;

	Mesh();
    //Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures);
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

	void draw(Shader* shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection);

    std::string to_string() const;

private:
    //  render data
    unsigned int VAO, VBO, EBO;

    void init();
};

#endif
