#include <sRAT-RT/mesh.h>

Mesh::Mesh()
{
}

// Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures)
// 	: vertices(vertices), indices(indices), textures(textures)
Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
	: vertices(vertices), indices(indices)
{
	init();
}

void Mesh::init() 
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
            &indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}

void Mesh::draw(Shader* shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
    // // bind appropriate textures
    // unsigned int diffuseNr = 1;
    // unsigned int specularNr = 1;
    // unsigned int normalNr = 1;
    // unsigned int heightNr = 1;
    // for (unsigned int i = 0; i < textures.size(); i++)
    // {
    //     glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
    //     // retrieve texture number (the N in diffuse_textureN)
    //     std::string number;
    //     std::string name = textures[i].type;
    //     if (name == "texture_diffuse")
    //         number = std::to_string(diffuseNr++);
    //     else if (name == "texture_specular")
    //         number = std::to_string(specularNr++); // transfer unsigned int to string
    //     else if (name == "texture_normal")
    //         number = std::to_string(normalNr++); // transfer unsigned int to string
    //     else if (name == "texture_height")
    //         number = std::to_string(heightNr++); // transfer unsigned int to string

    //     // now set the sampler to the correct texture unit
    //     //glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
    //     shader.setFloat(("material." + name + number).c_str(), i);
        
    //     // and finally bind the texture
    //     glBindTexture(GL_TEXTURE_2D, textures[i].id);
    // }

    /// TODO: Call Material->set_shader_uniforms()

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

std::string Mesh::to_string() const
{
    std::string s = std::to_string(vertices.size()) + " vertices";
    //for (const auto& v : vertices) {
    //	s += v.to_string() + " ";
    //}
    s += ", " + std::to_string(indices.size()) + " indices";
    //s += ", " + std::to_string(textures.size()) + " textures";

    /*for (const auto& i : indices) {
        s += std::to_string(i) + " ";
    }*/
    return s;
}
