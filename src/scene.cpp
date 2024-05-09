#include <vector>

#include <sRAT-RT/scene.h>
#include <sRAT-RT/model.h>

#include <sRAT-RT/dir_light.h>      /// TODO: Delete after hardcoding my test scene
#include <sRAT-RT/point_light.h>    /// TODO: Delete after hardcoding my test scene

Scene::Scene()
{
    /// Default scene?
    /// TODO: Define later a proper scene format,
    ///     once I'm more sure about how I'm going to implement all this.
    camera = new Camera(glm::vec3(0.0f, 0.0f, 2.0f));
    m_renderables.empty();
}

Scene::Scene(const std::string& scene_file_path)
{
    std::cout << "APP::STATUS::INIT::SCENE_INIT: Initializing the scene..." << std::endl;
    //camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    m_renderables.empty();

    /////////////////////////
    /// TODO: Under construction, will finish this at some point in development
    //          For now I'm only going to use a test scene so who cares
    //          THIS IS HARDCODED, PROCEED WITH CAUTION
    // bool wasd = load_renderable("../resources/objects/backpack/backpack.obj", 
    //             "../src/shaders/vertex_deferred_gbuffer.glsl", "../src/shaders/fragment_deferred_gbuffer.glsl",
    //             "../src/shaders/vertex_forward.glsl", "../src/shaders/fragment_forward.glsl");
    bool wasd = hardcoded_scene_test();
    if(wasd)
    {
        std::cout << "Scene successfully loaded!" << std::endl;
    }
    else
    {
        std::cout << "APP::STATUS::SCENE_LOAD::ERROR: Test model was not properly loaded!" << std::endl;
    }
    /////////////////////////
}

/// TODO: Fix all this once the hardcoded scene works, so I can load scenes properly :)
bool Scene::load_renderable(const std::string& model_path, std::string v_deferred, std::string f_deferred, std::string v_forward, std::string f_forward)
{
    /// Néstor's code
    // m_renderables.emplace_back();
    // RenderableObject& ro = m_renderables.back();

    /// TODO: Substitute this with a method that loads both a RO and  [...]
    // if(!ro.load_model(model_path)) {
    //     std::cout << "APP::STATUS::INIT::SCENE_ERROR: Could not load model " << model_path << std::endl;
    //     return false;
    // }
    // Shader* _deferred = new Shader(v_deferred.c_str(), f_deferred.c_str());
    // Shader* _forward = new Shader(v_forward.c_str(), f_forward.c_str());
    // model.set_deferred_shader(_deferred);
    // model.set_forward_shader(_forward);

    return true;
}

void Scene::generate_emission_tex_array()
{
    glGenTextures(1, &m_lights_emission_tex_array_id);
    glBindTexture(GL_TEXTURE_1D_ARRAY, m_lights_emission_tex_array_id);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Determine the maximum size among all spectral data arrays
    GLsizei maxArraySize = 0;
    for (auto light : m_scene_lights) {
        GLsizei arraySize = light->get_spectrum()->get_n_samples();
        if (arraySize > maxArraySize) {
            maxArraySize = arraySize;
        }
    }

    // Allocate storage for the texture array
    GLsizei numLayers = m_scene_lights.size();
    glTexStorage2D(GL_TEXTURE_1D_ARRAY, 1, GL_R32F, maxArraySize, numLayers);

    // Upload data to the texture array
    for (size_t i = 0; i < numLayers; i++) {
        auto light = m_scene_lights.at(i);
        GLsizei arraySize = light->get_spectrum()->get_n_samples();
        float* data = new float[arraySize];
        for(int j = 0; j < arraySize; j++)
        {
            data[j] = light->get_spectrum()->get_responses()->at(j).response;
        }
        glTexSubImage2D(GL_TEXTURE_1D_ARRAY, 0, 0, i, arraySize, 1, GL_RED, GL_FLOAT,  data);
        delete data;
    }
}

Camera* Scene::get_camera() const
{
    return camera;
}

Volume* Scene::get_global_volume() const
{
    return global_volume;
}

std::vector<RenderableObject*> Scene::get_renderables() const
{
    return m_renderables;
}

std::vector<Light*> Scene::get_lights() const
{
    return m_scene_lights;
}

int Scene::get_num_lights() const 
{
    return m_scene_lights.size();
}

unsigned int Scene::get_emission_tex_array_id() const
{
    return m_lights_emission_tex_array_id;
}

bool Scene::hardcoded_scene_test()
{
    /// SCENE CAMERA
    camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));   // camera at position [0,0,3]

    /// SCENE MODELS (EACH NEEDS A MATERIAL WHICH NEEDS TEXTURES)
    std::vector<Texture> textures;
    std::vector<Texture> textures_2;

    /// TEXTURES 1 ///
    Texture albedo_tex;
    albedo_tex.binding = 0;
    albedo_tex.id = 0;
    //albedo_tex.path = "../resources/pbr_materials/used-stainless-steel2-bl/used-stainless-steel2_albedo.png";
    //albedo_tex.path = "../resources/pbr_materials/whispy-grass-meadow-bl/wispy-grass-meadow_albedo.png";
    //albedo_tex.path = "../resources/pbr_materials/gold-scuffed-bl/gold-scuffed_basecolor.png";
    albedo_tex.path = "../resources/objects/backpack/diffuse.jpg";
    albedo_tex.type = "diff_texture";
    textures.push_back(albedo_tex);

    Texture normals_tex;
    normals_tex.binding = 1;
    normals_tex.id = 0;
    //normals_tex.path = "../resources/pbr_materials/used-stainless-steel2-bl/used-stainless-steel2_normal-ogl.png";
    //normals_tex.path = "../resources/pbr_materials/whispy-grass-meadow-bl/wispy-grass-meadow_normal-ogl.png";
    //normals_tex.path = "../resources/pbr_materials/gold-scuffed-bl/gold-scuffed_normal.png";
    normals_tex.path = "../resources/objects/backpack/normal.png";
    normals_tex.type = "normal_texture";
    textures.push_back(normals_tex);

    Texture metallic_tex;
    metallic_tex.binding = 2;
    metallic_tex.id = 0;
    //metallic_tex.path = "../resources/pbr_materials/used-stainless-steel2-bl/used-stainless-steel2_metallic.png";
    //metallic_tex.path = "../resources/pbr_materials/whispy-grass-meadow-bl/wispy-grass-meadow_metallic.png";
    //metallic_tex.path = "../resources/pbr_materials/gold-scuffed-bl/gold-scuffed_metallic.png";
    metallic_tex.path = "../resources/objects/backpack/specular.jpg";
    metallic_tex.type = "metallic_texture";
    textures.push_back(metallic_tex);

    Texture roughness_tex;
    roughness_tex.binding = 3;
    roughness_tex.id = 0;
    //roughness_tex.path = "../resources/pbr_materials/used-stainless-steel2-bl/used-stainless-steel2_roughness.png";
    //roughness_tex.path = "../resources/pbr_materials/whispy-grass-meadow-bl/wispy-grass-meadow_roughness.png";
    //roughness_tex.path = "../resources/pbr_materials/gold-scuffed-bl/gold-scuffed_roughness.png";
    roughness_tex.path = "../resources/objects/backpack/roughness.jpg";
    roughness_tex.type = "roughness_texture";
    textures.push_back(roughness_tex);

    Texture ao_tex;
    ao_tex.binding = 4;
    ao_tex.id = 0;
    //ao_tex.path = "../resources/pbr_materials/used-stainless-steel2-bl/used-stainless-steel2_ao.png";
    //ao_tex.path = "../resources/pbr_materials/whispy-grass-meadow-bl/wispy-grass-meadow_ao.png";
    //ao_tex.path = "../resources/pbr_materials/whispy-grass-meadow-bl/wispy-grass-meadow_ao.png";
    ao_tex.path = "../resources/objects/backpack/ao.jpg";
    ao_tex.type = "ao_texture";
    textures.push_back(ao_tex);




    /// TEXTURES 2
    Texture albedo_tex_2;
    albedo_tex_2.binding = 0;
    albedo_tex_2.id = 0;
    albedo_tex_2.path = "../resources/pbr_materials/white-marble-bl/white-marble_albedo.png";
    albedo_tex_2.type = "diff_texture";
    textures_2.push_back(albedo_tex_2);

    Texture normals_tex_2;
    normals_tex_2.binding = 1;
    normals_tex_2.id = 0;
    normals_tex_2.path = "../resources/pbr_materials/white-marble-bl/white-marble_normal-ogl.png";
    normals_tex_2.type = "normal_texture";
    textures_2.push_back(normals_tex_2);

    Texture metallic_tex_2;
    metallic_tex_2.binding = 2;
    metallic_tex_2.id = 0;
    metallic_tex_2.path = "../resources/pbr_materials/white-marble-bl/white-marble_metallic.png";
    metallic_tex_2.type = "metallic_texture";
    textures_2.push_back(metallic_tex_2);

    Texture roughness_tex_2;
    roughness_tex_2.binding = 3;
    roughness_tex_2.id = 0;
    roughness_tex_2.path = "../resources/pbr_materials/white-marble-bl/white-marble_roughness.png";
    roughness_tex_2.type = "roughness_texture";
    textures_2.push_back(roughness_tex_2);

    Texture ao_tex_2;
    ao_tex_2.binding = 4;
    ao_tex_2.id = 0;
    ao_tex_2.path = "../resources/pbr_materials/white-marble-bl/white-marble_ao.png";
    ao_tex_2.type = "ao_texture";
    textures_2.push_back(ao_tex_2);


    PBRMaterial* pbr_mat = new PBRMaterial(textures, "../src/shaders/pbr_spectral/vertex_deferred_gbuffer.glsl",
                                                    "../src/shaders/pbr_spectral/fragment_deferred_gbuffer.glsl");

    PBRMaterial* pbr_mat_2 = new PBRMaterial(textures_2, "../src/shaders/pbr_spectral/vertex_deferred_gbuffer.glsl",
                                                    "../src/shaders/pbr_spectral/fragment_deferred_gbuffer.glsl");
    //Model* scene_model = new Model("../resources/objects/funny/HARD_AF.obj", pbr_mat);                    //meshes are loaded automatically
    Model* scene_model = new Model("../resources/objects/backpack/backpack.obj", pbr_mat);                  //meshes are loaded automatically
    Model* scene_model_2 = new Model("../resources/objects/unit_sphere/sphere.obj", pbr_mat_2);             //meshes are loaded automatically

    scene_model->get_transform()->set_pos(glm::vec3(0.0f, 0.0f, 0.0f));
    scene_model->get_transform()->set_scale(glm::vec3(1.0f, 1.0f, 1.0f));
    scene_model_2->get_transform()->set_pos(glm::vec3(4.0f, 0.0f, 0.0f));
    scene_model_2->get_transform()->set_scale(glm::vec3(1.0f, 1.0f, 1.0f));
    m_renderables.push_back(scene_model);
    m_renderables.push_back(scene_model_2);

    /// SCENE LIGHTS
    float light_mult = 10.0f;
    glm::vec3 light_dir = glm::vec3(0.5f, -0.25f, -1.0f);
    Spectrum* spectrum_D65 = new Spectrum("../resources/emitter_curves/CIE_std_illum_D65.csv", glm::vec3(1.0f, 1.0f, 1.0f));
    Spectrum* spectrum_A = new Spectrum("../resources/emitter_curves/CIE_std_illum_A.csv", glm::vec3(1.0f, 0.68235294117f, 0.3725490196f));
    
    DirLight* dir_light = new DirLight(light_dir, spectrum_D65, light_mult);
    m_scene_lights.push_back(dir_light);
    
    float att_c = 1.0f;
    float att_l = 0.7f;
    float att_q = 1.8f;
    //PointLight* pl_1 = new PointLight(glm::vec3(-1, 0, 1), spectrum_D65, light_mult, att_c, att_l, att_q);
    PointLight* pl_2 = new PointLight(glm::vec3(1, 0, 1), spectrum_A, light_mult, att_c, att_l, att_q);
    //m_scene_lights.push_back(pl_1);
    m_scene_lights.push_back(pl_2);

    generate_emission_tex_array();      // needs to be called after placing all the lights in the scene

    /// FINALLY, SCENE VOLUME (FOG)
    global_volume = new Volume("../resources/volume_data/waterType_JerlovI_properties.csv", glm::vec3(0.309, 0.053, 0.009), glm::vec3(0.001, 0.002, 0.004)); // hardcoded hehehe

    return true;
}