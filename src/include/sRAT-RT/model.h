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

    Model(char* path)
    {
        loadModel(path);
    }
    
    void draw(Shader& shader);
    bool loadModel(const std::string& path);

private:
    // std::vector<Texture> textures_loaded;
    std::vector<Mesh> meshes;
    std::string directory;

    bool processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    // std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

#endif