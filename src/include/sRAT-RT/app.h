#ifndef APP_CLASS_H
#define APP_CLASS_H

#include <sRAT-RT/scene.h>
#include <sRAT-RT/settings.h>
#include <rgb2spec/rgb2spec.h>
#include <sRAT-RT/colorspace.h>
#include <sRAT-RT/framebuffer.h>

#define GLFW_INCLUDE_NONE

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#define FRAMEBUFFER_TEX_NUM 5   /// CHANGE THIS LATER, ONCE I KNOW HOW TO DO SHIT!

class App
{

private:
    Scene* scene;
    Settings* settings;         // Create settings here
    GLFWwindow* window;
    std::unordered_map<colorspace, RGB2Spec*>* look_up_tables;

    float m_deltatime;
    float m_lastframe_time;

    /// TODO: I don't know how to declare this framebuffer
    //      For now I'm using rgba and 5 textures, CHANGE LATER IF NEEDED!
    GLFrameBufferRGBA<FRAMEBUFFER_TEX_NUM>* gl_deferred_framebuffer;

    struct Mouse_data
    {
        float lastX = 400;
        float lastY = 300;
        bool firstMouse = true;
    } mouse_data;

    void load_luts(const std::string& dir, const std::string& ext);

    void deltatime_frame_tick();    // Update deltatime and lastframe time values
    void process_input(GLFWwindow* window, Camera* camera, float deltaTime);

public:
    App();
    App(std::string path_settings);
    ~App();

    bool init();                        // Configure GLAD, callbacks,etc. before running
    void load_scene();                  // Since this creates shaders, it must be called AFTER initializing the app (which initializes glad)
    void run();                         // This method will have the render loop, like our old main functions
    void cleanup();                     // To be called once we exit the main loop in run()

    void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    void mouse_callback(GLFWwindow* window, double xpos, double ypos);
};

#endif