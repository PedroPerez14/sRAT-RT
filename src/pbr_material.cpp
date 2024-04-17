#include <sRAT-RT/pbr_material.h>
#include <sRAT-RT/stb_image.h>

unsigned int texture_from_file(const std::string& full_path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(full_path.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    }
    else
    {
        std::cout << "Failed to load texture: " << full_path << std::endl;
    }
    stbi_image_free(data);

    return textureID;
}

PBRMaterial::PBRMaterial(const std::vector<Texture>& textures_to_load, const char* vert_sh_path, const char* frag_sh_path)
{
    mat_textures.empty();   // just in case
    
    load_textures(textures_to_load);

    mat_shader = new Shader(vert_sh_path, frag_sh_path);

    /// TODO: ANYTHING ELSE
}

void PBRMaterial::set_shader_uniforms()
{

}

Shader* PBRMaterial::get_shader()
{

}

bool PBRMaterial::reload_shader()
{

}

void PBRMaterial::load_textures(const std::vector<Texture>& textures_to_load)
{
    for(unsigned int i = 0; i < textures_to_load.size(); i++)
    {
        bool skip = false;
        unsigned int currently_loaded = mat_textures.size();
        for(unsigned int j = 0; j < currently_loaded; j++)
        {
            // If we have already loaded that texture, copy its data
            if(mat_textures[j].path == textures_to_load[i].path)
            {
                mat_textures.push_back(mat_textures[j]);
                skip == true;
                break;
            }
        }
        // If not previously loaded, load it
        if(!break)
        {
            Texture tex;
            tex.id = texture_from_file(textures_to_load[i].path);
            tex.binding = textures_to_load[i].binding;
            tex.type = textures_to_load[i].type;
            tex.path = textures_to_load[i].path;

            mat_textures.push_back(tex);
        }
    }
}