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
    Settings* settings;         // Create settings here
    Scene* scene;
    std::unordered_map<colorspace, RGB2Spec*>* look_up_tables;
    // change this to glfwwindow or however it is written lmao
    GLFWwindow* window;

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


public:
    App();
    App(std::string path_settings);
    ~App();

    bool init();                        // Configure GLAD, callbacks,etc. before running
    void run();                         // This method will have the render loop, like our old main functions

    void framebuffer_size_callback(GLFWwindow* window, int width, int height) const;
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) const;
    void mouse_callback(GLFWwindow* window, double xpos, double ypos);
};

#endif