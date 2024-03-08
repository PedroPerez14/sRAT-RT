#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <sRAT-RT/shader.h>
#include <sRAT-RT/mesh.h>

class Model
{
public:
    Model(char* path)
    {
        loadModel(path);
    }

    void Draw(Shader& shader);

private:
    std::vector<Texture> textures_loaded;
    std::vector<Mesh> meshes;
    std::string directory;

    void loadModel(const std::string& path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, 
        aiTextureType type, std::string typeName);
};

#endif