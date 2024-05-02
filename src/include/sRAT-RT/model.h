#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <sRAT-RT/mesh.h>
#include <sRAT-RT/shader.h>
#include <sRAT-RT/renderable_object.h>

#include <sRAT-RT/pbr_material.h>

/*
 * Now the Model class loads the 3d models, along with its meshes and textures,
 *  but we're ignoring its textures completely since we'll override them with our materials
 *  in our own Material class. I'll leave the code commented in case it's needed later on.
 */

class Model : public RenderableObject
{
public:

    Model();

    Model(char* path, Material* mat)
    {
        transform = new Transform();
        load_model(path);
        set_material(mat);
    }
    
    void draw(Shader* shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection);
    bool load_model(const std::string& path);

private:
    // std::vector<Texture> textures_loaded;
    std::vector<Mesh> meshes;
    std::string directory;

    bool process_node(aiNode *node, const aiScene *scene);
    Mesh process_mesh(aiMesh *mesh, const aiScene *scene);
    // std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

#endif