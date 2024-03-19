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

    Model();

    Model(char* path)
    {
        loadModel(path);
    }

    void draw(Shader& shader);
    bool loadModel(const std::string& path);

    Shader* get_deferred_shader() const;
    Shader* get_forward_shader() const;

    void set_deferred_shader(Shader* def);
    void set_forward_shader(Shader* fwd);

private:
    std::vector<Texture> textures_loaded;
    std::vector<Mesh> meshes;
    std::string directory;

    Shader* m_shader_deferred;              /// The lighting shader, the geometry pass one is standard (for now?)
    Shader* m_shader_forward;               /// The shader we'll use in case we have to do forward rendering instead of deferred

    bool processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, 
        aiTextureType type, std::string typeName);
};

#endif