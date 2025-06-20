#include <sRAT-RT/model.h>
#include <sRAT-RT/gl_check.h>

Model::Model()
{
    transform = new Transform();
}

void Model::draw(Shader* shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
    for(unsigned int i = 0; i < meshes.size(); i++)
    {
        if(shader == nullptr)
        {
            shader = mat->get_shader();
        }
        if(shader == mat->get_shader()) // Intended use case
        {
            shader->use();
            mat->set_shader_uniforms(model, view, projection);
        }
        // else
        // {
        //      IF YOU ARE TRYING TO DRAW WITH A SHADER OTHER THAN THE ONE IN THE MATERIAL
        //      YOU HAVE TO MANAGE THE UNIFORMS YOURSELF! (WILL PROBABLY CRASH)
        // }
        meshes.at(i).draw(shader);
    }
    // Good practice to set everything back to default
    //GL_CHECK(glActiveTexture(GL_TEXTURE0));   // I think I can delete this tbh
}

bool Model::load_model(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    // if the scene is not correctly loaded, abort
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return false;
    }
    directory = path.substr(0, path.find_last_of('/'));

    return process_node(scene->mRootNode, scene);       // Start recursive processing of nodes
}

bool Model::process_node(aiNode* node, const aiScene *scene)
{
    // First process all the nodes
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(process_mesh(mesh, scene));
    }

    bool success = true;
    // Then process all of its children, making this method recursive
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        aiNode* child = node->mChildren[i];
        success = (success && process_node(child, scene));
    }
    return success;
}

Mesh Model::process_mesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    // std::vector<Texture> textures;

    // First process all the vertices (position, normals, texcoords)
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector;
        // mVertices is for vertex positions, not all vertex data
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        // mNormals for normals
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;

        // and UVs in mTextureCoords, but this works slightly different (see learnopenGL book, chapter 21)
        if(mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
        {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }

    // Then, all the indices
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // And lastly, the materials and textures
    // if(mesh->mMaterialIndex >= 0)
    // {
    //     aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    //     std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    //     textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    //     std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    //     textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // }

    // return Mesh(vertices, indices, textures);
    return Mesh(vertices, indices);
}

// std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, 
//                                                 std::string typeName)
// {
//     std::vector<Texture> textures;
//     for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
//     {
//         aiString str;
//         mat->GetTexture(type, i, &str);
//         bool skip  = false;
//         for(unsigned int j = 0; j < textures_loaded.size(); j++)
//         {
//             if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
//             {
//                 textures.push_back(textures_loaded[j]);
//                 skip = true;
//                 break;
//             }
//         }
//         if(!skip)
//         {
//             Texture texture;
//             texture.id = TextureFromFile(str.C_Str(), directory);
//             texture.type = typeName;
//             texture.path = str.C_Str();
//             textures.push_back(texture);
//             textures_loaded.push_back(texture);
//         }
//     }
//     return textures;
// }
