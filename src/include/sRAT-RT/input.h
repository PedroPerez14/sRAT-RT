/// Handle input related stuff, like assigning all the callbacks :)
#ifndef INPUT_H
#define INPUT_H

#include <sRAT-RT/app.h>

static void handle_framebuffer_resize(GLFWwindow* window, int width, int height) 
{
    if(App* app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window)))
    {
        app->framebuffer_size_callback(window, width, height);
    }
}

static void handle_mouse_movement(GLFWwindow* window, double xpos, double ypos) 
{
    if(App* app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window)))
    {
        app->mouse_callback(window, xpos, ypos);
    }
}

static void handle_mouse_scroll(GLFWwindow* window, double xoffset, double yoffset)
{
    if(App* app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window)))
    {
        app->scroll_callback(window, xoffset, yoffset);
    }
}

#endif