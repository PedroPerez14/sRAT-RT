#ifndef APP_CLASS_H
#define APP_CLASS_H

#include <sRAT-RT/scene.h>
#include <sRAT-RT/settings.h>
#include <sRAT-RT/renderer.h>
#include <rgb2spec/rgb2spec.h>
#include <sRAT-RT/colorspace.h>
#include <sRAT-RT/framebuffer.h>
#include <sRAT-RT/response_curve.h>

#define GLFW_INCLUDE_NONE

#include <glad/gl.h>
#include <GLFW/glfw3.h>

class App
{

private:
    Scene* scene;
    Settings* settings;         // Create settings here
    GLFWwindow* window;
    Renderer* renderer;
    std::unordered_map<colorspace, RGB2Spec*>* look_up_tables;
    std::unordered_map<std::string, ResponseCurve*>* response_curves;

    float m_deltatime;
    float m_lastframe_time;

    bool sleep_rendering;       // If we minimized the window, don't render or it'll crash

    struct Mouse_data
    {
        float lastX = 400;
        float lastY = 300;
        bool firstMouse = true;
    } mouse_data;

    void load_luts(const std::string& dir, const std::string& ext);
    void load_response_curves(const std::string& path_responses);

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