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

PBRMaterial::PBRMaterial(const std::vector<Texture>& textures_to_load, 
                        const char* vert_sh_geom_path, const char* frag_sh_geom_path,
                        const char* vert_sh_light_path, const char* frag_sh_light_path)
{
    mat_textures.empty();   // just in case
    // Load the textures and compile the shaders
    load_textures(textures_to_load);
    shader_geometry_pass = new Shader(vert_sh_geom_path, frag_sh_geom_path);
    shader_lighting_pass = new Shader(vert_sh_light_path, frag_sh_light_path);
}


/// WARNING: WE OPERATE UNDER THE ASSUMPTION THAT THE TEXTURES ARE IN THE SAME ORDER AS THEIR BINDINGS
void PBRMaterial::set_shader_uniforms(RenderPass pass) override
{
    /// TODO: Change this method

    // For this type of PBR shader we assume that every uniform is a texture
    for(unsigned int i = 0; i < mat_textures.size(); i++)
    {
        Texture _tex = mat_textures[i];
        glActiveTexture(GL_TEXTURE0 + _tex.binding);
        glBindTexture(GL_TEXTURE_2D, _tex.id);
        mat_shader->setInt((const std::string)(_tex.type), _tex.binding);
    }
    // Good practice to set everything back to default
    glActiveTexture(GL_TEXTURE0);
}

Shader* PBRMaterial::get_shader(RenderPass pass) override
{
    if(pass == DEFERRED_GEOMETRY)
    {
        return shader_geometry_pass;
    }
    if(pass == DEFERRED_ILLUMINATION)
    {
        return shader_forward_pass;
    }
    // should not happen (it will, somehow)
    return nullptr;
}

bool PBRMaterial::reload_shaders() override
{
    bool reload_ok = true;
    bool reload_ok_2 = true;

    Shader* shader_geometry_old = shader_geometry_pass;
    Shader* shader_geometry_new = new Shader(shader_geometry_pass->m_vertexPath, shader_geometry_pass->m_fragmentPath, reload_ok);

    Shader* shader_lighting_old = shader_lighting_pass;
    Shader* shader_lighting_new = new Shader(shader_lighting_pass->m_vertexPath, shader_lighting_pass->m_fragmentPath, reload_ok_2);

    if(reload_ok && reload_ok_2)
    {
        // if compilation or linking of the shader failed, it won't be used
        //      to avoid program-crashing failures (in this case both have to compile)
        shader_geometry_pass = shader_geometry_new;
        shader_lighting_pass = shader_lighting_new;
        delete shader_geometry_old;
        delete shader_lighting_old;
    }

    return (reload_ok && reload_ok_2);
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
        if(!skip)
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