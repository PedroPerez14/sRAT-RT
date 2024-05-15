#include <sRAT-RT/diffuse_material.h>
#include <sRAT-RT/stb_image.h>
#include <sRAT-RT/gl_check.h>

/// TODO: Change the material id system, at least a .h file mapping the materials?
DiffuseMaterial::DiffuseMaterial(const std::vector<Texture>& textures_to_load, 
                        const char* geom_pass_v_shader_path, const char* geom_pass_f_shader_path)
{
    mat_textures.empty();   // just in case
    // Load the textures and compile the shaders
    load_textures(textures_to_load);
    mat_shader = new Shader(geom_pass_v_shader_path, geom_pass_f_shader_path);
    render_pass = DEFERRED_GEOMETRY;
    mat_id = 3;         // Hardcoded (I'll do some table in a .h file, in the future this _should_ be different)
}

void DiffuseMaterial::set_shader_uniforms(glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
    mat_shader->use();
    glm::mat3 normal_mat = glm::transpose(glm::inverse(glm::mat3(model)));
    mat_shader->setMat4("model", model);
    mat_shader->setMat4("view", view);
    mat_shader->setMat4("projection", projection);
    mat_shader->setMat3("normal_mat", normal_mat);  // don't compute this in the vertex shader please
    mat_shader->setInt("mat_id", mat_id);

    // For this type of diffuse shader we assume that every uniform is a texture
    for(unsigned int i = 0; i < mat_textures.size(); i++)
    {
        Texture _tex = mat_textures[i];
        unsigned int binding = _tex.binding;
        unsigned int id = _tex.id;
        glActiveTexture(GL_TEXTURE0 + binding);
        glBindTexture(GL_TEXTURE_2D, id);
    }
}

Shader* DiffuseMaterial::get_shader()
{
    return mat_shader;
}

bool DiffuseMaterial::reload_shader()
{
    bool reload_ok = true;

    Shader* shader_old = mat_shader;
    Shader* shader_new = new Shader(mat_shader->m_vertexPath, mat_shader->m_fragmentPath, reload_ok);
    if(reload_ok)
    {
        // if compilation or linking of the shader failed, it won't be used
        //      to avoid program-crashing failures (in this case both have to compile)
        mat_shader = shader_new;
        delete shader_old;
    }
    return reload_ok;
}

/// NOTE: The idea here is to pass an array of incomplete Texture structs
/// and the method completes them, along with the actual texture loading (I should change this)
void DiffuseMaterial::load_textures(const std::vector<Texture>& textures_to_load)
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
                skip = true;
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
