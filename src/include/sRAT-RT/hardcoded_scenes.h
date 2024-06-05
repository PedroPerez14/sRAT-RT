#ifndef _HARDCODED_SCENES_H_
#define _HARDCODED_SCENES_H_

/**
 * Here I'm going to place all the scenes 
 * that I need to hardcode for my experiments
 * 
 * (Loading scenes will not be in the version I submit for my master's thesis project)
*/
#include <sRAT-RT/scene.h>
#include <sRAT-RT/diffuse_material.h>

bool Scene::test_scene()
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
    
    //DirLight* dir_light = new DirLight(light_dir, spectrum_D65, light_mult);
    //m_scene_lights.push_back(dir_light);
    
    float att_c = 1.0f;
    float att_l = 0.7f;
    float att_q = 1.8f;
    PointLight* pl_1 = new PointLight(glm::vec3(-1, 0, 1), spectrum_D65, light_mult, att_c, att_l, att_q);
    PointLight* pl_2 = new PointLight(glm::vec3(1, 0, 1), spectrum_A, light_mult, att_c, att_l, att_q);
    m_scene_lights.push_back(pl_1);
    m_scene_lights.push_back(pl_2);

    generate_emission_tex_array();      // needs to be called after placing all the lights in the scene

    /// FINALLY, SCENE VOLUME (FOG)
    global_volume = new Volume("../resources/volume_data/waterType_JerlovI_properties.csv", glm::vec3(0.309, 0.053, 0.009), glm::vec3(0.001, 0.002, 0.004), glm::vec3(0.517, 0.112, 0.162)); // hardcoded hehehe

    return true;
}

bool Scene::diffuse_spheres_scene()
{
    /// SCENE CAMERA
    camera = new Camera(glm::vec3(-8.135436f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f, 1920.0f, 1080.0f, 0.1f, 100.0f);
    camera->Zoom = 22.895193470118905f; // 39.597755 deg Xfov toYfov (see formula in my notebook!)
    /// SCENE MODELS
    std::vector<Texture> textures_floor;
    std::vector<Texture> textures_wall;
    std::vector<Texture> textures_1;
    std::vector<Texture> textures_2;
    std::vector<Texture> textures_3;
    std::vector<Texture> textures_4;
    std::vector<Texture> textures_5;
    std::vector<Texture> textures_6;
    std::vector<Texture> textures_7;
    std::vector<Texture> textures_8;
    std::vector<Texture> textures_9;
    std::vector<Texture> textures_10;
    std::vector<Texture> textures_11;
    std::vector<Texture> textures_12;
    std::vector<Texture> textures_13;
    std::vector<Texture> textures_14;
    std::vector<Texture> textures_15;

    /// TEXTURES FLOOR ///
    Texture albedo_tex_floor{0, 0, "diff_texture", "../resources/pbr_materials/diffuse_pbr/albedo/diffuse_pbr_albedo_dark_grey.png"};
    textures_floor.push_back(albedo_tex_floor);
    Texture normals_tex_floor{0, 1, "normal_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_normal.png"};
    textures_floor.push_back(normals_tex_floor);
    Texture metallic_tex_floor{0, 2, "metallic_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_metallic.png"};
    textures_floor.push_back(metallic_tex_floor);
    Texture roughness_tex_floor{0, 3, "roughness_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_roughness.png"};
    textures_floor.push_back(roughness_tex_floor);
    Texture ao_tex_floor{0, 4, "ao_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_ao.png"};
    textures_floor.push_back(ao_tex_floor);

    /// TEXTURES WALL ///
    Texture albedo_tex_wall{0, 0, "diff_texture", "../resources/pbr_materials/diffuse_pbr/albedo/diffuse_pbr_albedo_light_grey.png"};
    textures_wall.push_back(albedo_tex_wall);
    Texture normals_tex_wall{0, 1, "normal_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_normal.png"};
    textures_wall.push_back(normals_tex_wall);
    Texture metallic_tex_wall{0, 2, "metallic_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_metallic.png"};
    textures_wall.push_back(metallic_tex_wall);
    Texture roughness_tex_wall{0, 3, "roughness_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_roughness.png"};
    textures_wall.push_back(roughness_tex_wall);
    Texture ao_tex_wall{0, 4, "ao_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_ao.png"};
    textures_wall.push_back(ao_tex_wall);

    /// TEXTURES SPHERE 1 ///
    Texture albedo_tex_sphere_1{0, 0, "diff_texture", "../resources/pbr_materials/diffuse_pbr/albedo/diffuse_pbr_albedo_1_dark_blue.png"};
    textures_1.push_back(albedo_tex_sphere_1);
    Texture normals_tex_sphere_1{0, 1, "normal_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_normal.png"};
    textures_1.push_back(normals_tex_sphere_1);
    Texture metallic_tex_sphere_1{0, 2, "metallic_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_metallic.png"};
    //textures_1.push_back(metallic_tex_sphere_1);
    Texture roughness_tex_sphere_1{0, 3, "roughness_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_roughness.png"};
    //textures_1.push_back(roughness_tex_sphere_1);
    Texture ao_tex_sphere_1{0, 4, "ao_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_ao.png"};
    //textures_1.push_back(ao_tex_sphere_1);

    /// TEXTURES SPHERE 2 ///
    Texture albedo_tex_sphere_2{0, 0, "diff_texture", "../resources/pbr_materials/diffuse_pbr/albedo/diffuse_pbr_albedo_2_blue.png"};
    textures_2.push_back(albedo_tex_sphere_2);
    Texture normals_tex_sphere_2{0, 1, "normal_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_normal.png"};
    textures_2.push_back(normals_tex_sphere_2);
    Texture metallic_tex_sphere_2{0, 2, "metallic_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_metallic.png"};
    textures_2.push_back(metallic_tex_sphere_2);
    Texture roughness_tex_sphere_2{0, 3, "roughness_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_roughness.png"};
    textures_2.push_back(roughness_tex_sphere_2);
    Texture ao_tex_sphere_2{0, 4, "ao_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_ao.png"};
    textures_2.push_back(ao_tex_sphere_2);

    /// TEXTURES SPHERE 3 ///
    Texture albedo_tex_sphere_3{0, 0, "diff_texture", "../resources/pbr_materials/diffuse_pbr/albedo/diffuse_pbr_albedo_3_green.png"};
    textures_3.push_back(albedo_tex_sphere_3);
    Texture normals_tex_sphere_3{0, 1, "normal_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_normal.png"};
    textures_3.push_back(normals_tex_sphere_3);
    Texture metallic_tex_sphere_3{0, 2, "metallic_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_metallic.png"};
    textures_3.push_back(metallic_tex_sphere_3);
    Texture roughness_tex_sphere_3{0, 3, "roughness_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_roughness.png"};
    textures_3.push_back(roughness_tex_sphere_3);
    Texture ao_tex_sphere_3{0, 4, "ao_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_ao.png"};
    textures_3.push_back(ao_tex_sphere_3);

    /// TEXTURES SPHERE 4 ///
    Texture albedo_tex_sphere_4{0, 0, "diff_texture", "../resources/pbr_materials/diffuse_pbr/albedo/diffuse_pbr_albedo_4_orange.png"};
    textures_4.push_back(albedo_tex_sphere_4);
    Texture normals_tex_sphere_4{0, 1, "normal_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_normal.png"};
    textures_4.push_back(normals_tex_sphere_4);
    Texture metallic_tex_sphere_4{0, 2, "metallic_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_metallic.png"};
    textures_4.push_back(metallic_tex_sphere_4);
    Texture roughness_tex_sphere_4{0, 3, "roughness_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_roughness.png"};
    textures_4.push_back(roughness_tex_sphere_4);
    Texture ao_tex_sphere_4{0, 4, "ao_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_ao.png"};
    textures_4.push_back(ao_tex_sphere_4);

    /// TEXTURES SPHERE 5 ///
    Texture albedo_tex_sphere_5{0, 0, "diff_texture", "../resources/pbr_materials/diffuse_pbr/albedo/diffuse_pbr_albedo_5_wine.png"};
    textures_5.push_back(albedo_tex_sphere_5);
    Texture normals_tex_sphere_5{0, 1, "normal_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_normal.png"};
    textures_5.push_back(normals_tex_sphere_5);
    Texture metallic_tex_sphere_5{0, 2, "metallic_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_metallic.png"};
    textures_5.push_back(metallic_tex_sphere_5);
    Texture roughness_tex_sphere_5{0, 3, "roughness_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_roughness.png"};
    textures_5.push_back(roughness_tex_sphere_5);
    Texture ao_tex_sphere_5{0, 4, "ao_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_ao.png"};
    textures_5.push_back(ao_tex_sphere_5);

    /// TEXTURES SPHERE 6 ///
    Texture albedo_tex_sphere_6{0, 0, "diff_texture", "../resources/pbr_materials/diffuse_pbr/albedo/diffuse_pbr_albedo_6_navy_blue.png"};
    textures_6.push_back(albedo_tex_sphere_6);
    Texture normals_tex_sphere_6{0, 1, "normal_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_normal.png"};
    textures_6.push_back(normals_tex_sphere_6);
    Texture metallic_tex_sphere_6{0, 2, "metallic_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_metallic.png"};
    textures_6.push_back(metallic_tex_sphere_6);
    Texture roughness_tex_sphere_6{0, 3, "roughness_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_roughness.png"};
    textures_6.push_back(roughness_tex_sphere_6);
    Texture ao_tex_sphere_6{0, 4, "ao_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_ao.png"};
    textures_6.push_back(ao_tex_sphere_6);

    /// TEXTURES SPHERE 7 ///
    Texture albedo_tex_sphere_7{0, 0, "diff_texture", "../resources/pbr_materials/diffuse_pbr/albedo/diffuse_pbr_albedo_7_grass_green.png"};
    textures_7.push_back(albedo_tex_sphere_7);
    Texture normals_tex_sphere_7{0, 1, "normal_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_normal.png"};
    textures_7.push_back(normals_tex_sphere_7);
    Texture metallic_tex_sphere_7{0, 2, "metallic_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_metallic.png"};
    textures_7.push_back(metallic_tex_sphere_7);
    Texture roughness_tex_sphere_7{0, 3, "roughness_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_roughness.png"};
    textures_7.push_back(roughness_tex_sphere_7);
    Texture ao_tex_sphere_7{0, 4, "ao_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_ao.png"};
    textures_7.push_back(ao_tex_sphere_7);

    /// TEXTURES SPHERE 8 ///
    Texture albedo_tex_sphere_8{0, 0, "diff_texture", "../resources/pbr_materials/diffuse_pbr/albedo/diffuse_pbr_albedo_8_lemon_yellow.png"};
    textures_8.push_back(albedo_tex_sphere_8);
    Texture normals_tex_sphere_8{0, 1, "normal_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_normal.png"};
    textures_8.push_back(normals_tex_sphere_8);
    Texture metallic_tex_sphere_8{0, 2, "metallic_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_metallic.png"};
    textures_8.push_back(metallic_tex_sphere_8);
    Texture roughness_tex_sphere_8{0, 3, "roughness_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_roughness.png"};
    textures_8.push_back(roughness_tex_sphere_8);
    Texture ao_tex_sphere_8{0, 4, "ao_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_ao.png"};
    textures_8.push_back(ao_tex_sphere_8);

    /// TEXTURES SPHERE 9 ///
    Texture albedo_tex_sphere_9{0, 0, "diff_texture", "../resources/pbr_materials/diffuse_pbr/albedo/diffuse_pbr_albedo_9_salmon_pink.png"};
    textures_9.push_back(albedo_tex_sphere_9);
    Texture normals_tex_sphere_9{0, 1, "normal_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_normal.png"};
    textures_9.push_back(normals_tex_sphere_9);
    Texture metallic_tex_sphere_9{0, 2, "metallic_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_metallic.png"};
    textures_9.push_back(metallic_tex_sphere_9);
    Texture roughness_tex_sphere_9{0, 3, "roughness_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_roughness.png"};
    textures_9.push_back(roughness_tex_sphere_9);
    Texture ao_tex_sphere_9{0, 4, "ao_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_ao.png"};
    textures_9.push_back(ao_tex_sphere_9);

    /// TEXTURES SPHERE 10 ///
    Texture albedo_tex_sphere_10{0, 0, "diff_texture", "../resources/pbr_materials/diffuse_pbr/albedo/diffuse_pbr_albedo_10_purple.png"};
    textures_10.push_back(albedo_tex_sphere_10);
    Texture normals_tex_sphere_10{0, 1, "normal_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_normal.png"};
    textures_10.push_back(normals_tex_sphere_10);
    Texture metallic_tex_sphere_10{0, 2, "metallic_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_metallic.png"};
    textures_10.push_back(metallic_tex_sphere_10);
    Texture roughness_tex_sphere_10{0, 3, "roughness_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_roughness.png"};
    textures_10.push_back(roughness_tex_sphere_10);
    Texture ao_tex_sphere_10{0, 4, "ao_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_ao.png"};
    textures_10.push_back(ao_tex_sphere_10);

    /// TEXTURES SPHERE 11 ///
    Texture albedo_tex_sphere_11{0, 0, "diff_texture", "../resources/pbr_materials/diffuse_pbr/albedo/diffuse_pbr_albedo_11_lilac.png"};
    textures_11.push_back(albedo_tex_sphere_11);
    Texture normals_tex_sphere_11{0, 1, "normal_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_normal.png"};
    textures_11.push_back(normals_tex_sphere_11);
    Texture metallic_tex_sphere_11{0, 2, "metallic_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_metallic.png"};
    textures_11.push_back(metallic_tex_sphere_11);
    Texture roughness_tex_sphere_11{0, 3, "roughness_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_roughness.png"};
    textures_11.push_back(roughness_tex_sphere_11);
    Texture ao_tex_sphere_11{0, 4, "ao_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_ao.png"};
    textures_11.push_back(ao_tex_sphere_11);

    /// TEXTURES SPHERE 12 ///
    Texture albedo_tex_sphere_12{0, 0, "diff_texture", "../resources/pbr_materials/diffuse_pbr/albedo/diffuse_pbr_albedo_12_clear_blue.png"};
    textures_12.push_back(albedo_tex_sphere_12);
    Texture normals_tex_sphere_12{0, 1, "normal_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_normal.png"};
    textures_12.push_back(normals_tex_sphere_12);
    Texture metallic_tex_sphere_12{0, 2, "metallic_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_metallic.png"};
    textures_12.push_back(metallic_tex_sphere_12);
    Texture roughness_tex_sphere_12{0, 3, "roughness_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_roughness.png"};
    textures_12.push_back(roughness_tex_sphere_12);
    Texture ao_tex_sphere_12{0, 4, "ao_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_ao.png"};
    textures_12.push_back(ao_tex_sphere_12);

    /// TEXTURES SPHERE 13 ///
    Texture albedo_tex_sphere_13{0, 0, "diff_texture", "../resources/pbr_materials/diffuse_pbr/albedo/diffuse_pbr_albedo_13_light_green.png"};
    textures_13.push_back(albedo_tex_sphere_13);
    Texture normals_tex_sphere_13{0, 1, "normal_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_normal.png"};
    textures_13.push_back(normals_tex_sphere_13);
    Texture metallic_tex_sphere_13{0, 2, "metallic_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_metallic.png"};
    textures_13.push_back(metallic_tex_sphere_13);
    Texture roughness_tex_sphere_13{0, 3, "roughness_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_roughness.png"};
    textures_13.push_back(roughness_tex_sphere_13);
    Texture ao_tex_sphere_13{0, 4, "ao_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_ao.png"};
    textures_13.push_back(ao_tex_sphere_13);

    /// TEXTURES SPHERE 14 ///
    Texture albedo_tex_sphere_14{0, 0, "diff_texture", "../resources/pbr_materials/diffuse_pbr/albedo/diffuse_pbr_albedo_14_cream.png"};
    textures_14.push_back(albedo_tex_sphere_14);
    Texture normals_tex_sphere_14{0, 1, "normal_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_normal.png"};
    textures_14.push_back(normals_tex_sphere_14);
    Texture metallic_tex_sphere_14{0, 2, "metallic_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_metallic.png"};
    textures_14.push_back(metallic_tex_sphere_14);
    Texture roughness_tex_sphere_14{0, 3, "roughness_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_roughness.png"};
    textures_14.push_back(roughness_tex_sphere_14);
    Texture ao_tex_sphere_14{0, 4, "ao_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_ao.png"};
    textures_14.push_back(ao_tex_sphere_14);

    /// TEXTURES SPHERE 15 ///
    Texture albedo_tex_sphere_15{0, 0, "diff_texture", "../resources/pbr_materials/diffuse_pbr/albedo/diffuse_pbr_albedo_15_light_purple.png"};
    textures_15.push_back(albedo_tex_sphere_15);
    Texture normals_tex_sphere_15{0, 1, "normal_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_normal.png"};
    textures_15.push_back(normals_tex_sphere_15);
    Texture metallic_tex_sphere_15{0, 2, "metallic_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_metallic.png"};
    textures_15.push_back(metallic_tex_sphere_15);
    Texture roughness_tex_sphere_15{0, 3, "roughness_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_roughness.png"};
    textures_15.push_back(roughness_tex_sphere_15);
    Texture ao_tex_sphere_15{0, 4, "ao_texture", "../resources/pbr_materials/diffuse_pbr/diffuse_pbr_ao.png"};
    textures_15.push_back(ao_tex_sphere_15);
    

    char* path_vertex_pbr = "../src/shaders/pbr_spectral/vertex_deferred_gbuffer.glsl";
    char* path_fragment_pbr = "../src/shaders/pbr_spectral/fragment_deferred_gbuffer.glsl";

    char* path_vertex_diff = "../src/shaders/pbr_spectral/diffuse_vertex_def_gbuffer.glsl";
    char* path_fragment_diff = "../src/shaders/pbr_spectral/diffuse_fragment_def_gbuffer.glsl";

    DiffuseMaterial* mat_floor = new DiffuseMaterial(textures_floor, path_vertex_diff, path_fragment_diff);
    DiffuseMaterial* mat_wall = new DiffuseMaterial(textures_wall, path_vertex_diff, path_fragment_diff);
    DiffuseMaterial* mat_sphere_1 = new DiffuseMaterial(textures_1, path_vertex_diff, path_fragment_diff);
    DiffuseMaterial* mat_sphere_2 = new DiffuseMaterial(textures_2, path_vertex_diff, path_fragment_diff);
    DiffuseMaterial* mat_sphere_3 = new DiffuseMaterial(textures_3, path_vertex_diff, path_fragment_diff);
    DiffuseMaterial* mat_sphere_4 = new DiffuseMaterial(textures_4, path_vertex_diff, path_fragment_diff);
    DiffuseMaterial* mat_sphere_5 = new DiffuseMaterial(textures_5, path_vertex_diff, path_fragment_diff);
    DiffuseMaterial* mat_sphere_6 = new DiffuseMaterial(textures_6, path_vertex_diff, path_fragment_diff);
    DiffuseMaterial* mat_sphere_7 = new DiffuseMaterial(textures_7, path_vertex_diff, path_fragment_diff);
    DiffuseMaterial* mat_sphere_8 = new DiffuseMaterial(textures_8, path_vertex_diff, path_fragment_diff);
    DiffuseMaterial* mat_sphere_9 = new DiffuseMaterial(textures_9, path_vertex_diff, path_fragment_diff);
    DiffuseMaterial* mat_sphere_10 = new DiffuseMaterial(textures_10, path_vertex_diff, path_fragment_diff);
    DiffuseMaterial* mat_sphere_11 = new DiffuseMaterial(textures_11, path_vertex_diff, path_fragment_diff);
    DiffuseMaterial* mat_sphere_12 = new DiffuseMaterial(textures_12, path_vertex_diff, path_fragment_diff);
    DiffuseMaterial* mat_sphere_13 = new DiffuseMaterial(textures_13, path_vertex_diff, path_fragment_diff);
    DiffuseMaterial* mat_sphere_14 = new DiffuseMaterial(textures_14, path_vertex_diff, path_fragment_diff);
    DiffuseMaterial* mat_sphere_15 = new DiffuseMaterial(textures_15, path_vertex_diff, path_fragment_diff);

    Model* model_floor = new Model("../resources/objects/mitsuba_scene_spheres/meshes/floor.obj", mat_floor);
    Model* model_wall = new Model("../resources/objects/mitsuba_scene_spheres/meshes/wall.obj", mat_wall);
    Model* model_sphere_1 = new Model("../resources/objects/mitsuba_scene_spheres/meshes/Sphere1.obj", mat_sphere_1);
    Model* model_sphere_2 = new Model("../resources/objects/mitsuba_scene_spheres/meshes/Sphere2.obj", mat_sphere_2);
    Model* model_sphere_3 = new Model("../resources/objects/mitsuba_scene_spheres/meshes/Sphere3.obj", mat_sphere_3);
    Model* model_sphere_4 = new Model("../resources/objects/mitsuba_scene_spheres/meshes/Sphere4.obj", mat_sphere_4);
    Model* model_sphere_5 = new Model("../resources/objects/mitsuba_scene_spheres/meshes/Sphere5.obj", mat_sphere_5);
    Model* model_sphere_6 = new Model("../resources/objects/mitsuba_scene_spheres/meshes/Sphere6.obj", mat_sphere_6);
    Model* model_sphere_7 = new Model("../resources/objects/mitsuba_scene_spheres/meshes/Sphere7.obj", mat_sphere_7);
    Model* model_sphere_8 = new Model("../resources/objects/mitsuba_scene_spheres/meshes/Sphere8.obj", mat_sphere_8);
    Model* model_sphere_9 = new Model("../resources/objects/mitsuba_scene_spheres/meshes/Sphere9.obj", mat_sphere_9);
    Model* model_sphere_10 = new Model("../resources/objects/mitsuba_scene_spheres/meshes/Sphere10.obj", mat_sphere_10);
    Model* model_sphere_11 = new Model("../resources/objects/mitsuba_scene_spheres/meshes/Sphere11.obj", mat_sphere_11);
    Model* model_sphere_12 = new Model("../resources/objects/mitsuba_scene_spheres/meshes/Sphere12.obj", mat_sphere_12);
    Model* model_sphere_13 = new Model("../resources/objects/mitsuba_scene_spheres/meshes/Sphere13.obj", mat_sphere_13);
    Model* model_sphere_14 = new Model("../resources/objects/mitsuba_scene_spheres/meshes/Sphere14.obj", mat_sphere_14);
    Model* model_sphere_15 = new Model("../resources/objects/mitsuba_scene_spheres/meshes/Sphere15.obj", mat_sphere_15);

    m_renderables.push_back(model_floor);
    m_renderables.push_back(model_wall);
    m_renderables.push_back(model_sphere_1);
    m_renderables.push_back(model_sphere_2);
    m_renderables.push_back(model_sphere_3);
    m_renderables.push_back(model_sphere_4);
    m_renderables.push_back(model_sphere_5);
    m_renderables.push_back(model_sphere_6);
    m_renderables.push_back(model_sphere_7);
    m_renderables.push_back(model_sphere_8);
    m_renderables.push_back(model_sphere_9);
    m_renderables.push_back(model_sphere_10);
    m_renderables.push_back(model_sphere_11);
    m_renderables.push_back(model_sphere_12);
    m_renderables.push_back(model_sphere_13);
    m_renderables.push_back(model_sphere_14);
    m_renderables.push_back(model_sphere_15);

    /// SCENE LIGHTS
    float light_mult = 1.0f;
    glm::vec3 light_dir = glm::normalize(glm::vec3(1.0f, -0.08748866352f, 0.0f));   // 1, tg(2),0 === 85deg inclination
    Spectrum* spectrum_D65 = new Spectrum("../resources/emitter_curves/CIE_std_illum_D65.csv", glm::vec3(1.0f, 1.0f, 1.0f));
    DirLight* dir_light = new DirLight(light_dir, spectrum_D65, light_mult);
    /// TODO: Set correctly the dir light direction !!!! (rotation: X = 0, Y = 85, Z = 180)
    m_scene_lights.push_back(dir_light);

    // float att_c = 1.0f;
    // float att_l = 0.1f;
    // float att_q = 0.2f;
    // PointLight* pl_1 = new PointLight(glm::vec3(8, 1, 0), spectrum_D65, light_mult, att_c, att_l, att_q);
    // m_scene_lights.push_back(pl_1);

    generate_emission_tex_array();      // needs to be called after placing all the lights in the scene
    /// FINALLY, SCENE VOLUME (FOG)
    global_volume = new Volume("../resources/volume_data/waterType_JerlovI_properties.csv", glm::vec3(0.309, 0.053, 0.009), glm::vec3(0.001, 0.002, 0.004), glm::vec3(0.471, 0.068, 0.023)); // hardcoded hehehe

    return true;
}


bool Scene::reef_scene()
{
    /// SCENE CAMERA
    glm::vec3 dir_ = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0)) * glm::vec4(glm::normalize(glm::vec3(0.75f, -3.0f, -1.4f)), 0.0f));
    glm::vec3 cam_pos = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0)) * glm::vec4(glm::vec3(0.2f, -2.4f, -0.45f), 1.0f));
    float yaw_ = glm::degrees(asin(-dir_.y)) * -1.0f;
    float pitch_ = glm::degrees(atan2(dir_.x , dir_.z));
    camera = new Camera(glm::vec3(0.2f, -2.4f, -0.45f), glm::vec3(0.0f, 1.0f, 0.0f), yaw_, pitch_, 1920.0f, 1080.0f, 0.1f, 100.0f);    
    //camera->updateCameraVectors();
    //camera->Zoom = 35.98339608247006f; // 60 deg Xfov toYfov (see formula in my notebook!)
    camera->Zoom = 60.0f;

    /// SCENE MODELS (EACH NEEDS A MATERIAL WHICH NEEDS TEXTURES)
    std::vector<Texture> textures_reef;

    /// TEXTURES 1 ///
    Texture albedo_tex;
    albedo_tex.binding = 0;
    albedo_tex.id = 0;
    albedo_tex.path = "../resources/objects/reef/reef_model.jpg";
    albedo_tex.type = "diff_texture";
    textures_reef.push_back(albedo_tex);

    Texture normals_tex;
    normals_tex.binding = 1;
    normals_tex.id = 0;
    normals_tex.path = "../resources/pbr_materials/gold-scuffed-bl/gold-scuffed_normal.png";    // ???
    normals_tex.type = "normal_texture";
    textures_reef.push_back(normals_tex);

    char* path_vertex_diff = "../src/shaders/pbr_spectral/diffuse_vertex_def_gbuffer.glsl";
    char* path_fragment_diff = "../src/shaders/pbr_spectral/diffuse_fragment_def_gbuffer.glsl";

    DiffuseMaterial* reef_mat = new DiffuseMaterial(textures_reef, path_vertex_diff, path_fragment_diff);

    Model* reef_model = new Model("../resources/objects/reef/reef_model.obj", reef_mat);                  //meshes are loaded automatically

    reef_model->get_transform()->set_rot(glm::vec3(glm::radians(330.0f), glm::radians(0.0f), glm::radians(0.0f)));
    reef_model->get_transform()->set_pos(glm::vec3(0.0f, -1.99f, 0.0f));
    reef_model->get_transform()->set_scale(glm::vec3(1.0f, 1.0f, 1.0f));

    m_renderables.push_back(reef_model);


    /// SCENE LIGHTS
    float light_mult = 10.0f;
    glm::vec3 light_dir = glm::vec3(0.0f, -1.0f, 0.0f);
    Spectrum* spectrum_D65 = new Spectrum("../resources/emitter_curves/CIE_std_illum_D65.csv", glm::vec3(1.0f, 1.0f, 1.0f));
    
    DirLight* dir_light = new DirLight(light_dir, spectrum_D65, light_mult);
    m_scene_lights.push_back(dir_light);

    generate_emission_tex_array();      // needs to be called after placing all the lights in the scene

    /// FINALLY, SCENE VOLUME (FOG)
    global_volume = new Volume("../resources/volume_data/waterType_JerlovI_properties.csv", glm::vec3(0.309, 0.053, 0.009), glm::vec3(0.001, 0.002, 0.004), glm::vec3(0.517, 0.112, 0.162)); // hardcoded hehehe

    return true;
}

#endif