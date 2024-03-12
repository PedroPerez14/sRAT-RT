#include <iostream>
#include <sRAT-RT/app.h>
#include <sRAT-RT/input.h>

GLFWwindow* init_glfw_and_create_window(const unsigned int& width, 
const unsigned int& height, const char* window_name)
{
    std::cout << "HELLO THERE" << std::endl;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, window_name, NULL, NULL);
	if (window == NULL)
	{
		std::cout << "APP::STATUS::INIT_ERROR::WINDOW_INIT_ERROR: Cannot create GLFW Window successfully! Aborting..." << std::endl;
		glfwTerminate();
		return NULL;
	}
    // else
    glfwMakeContextCurrent(window);
    return window;
}

// Callbacks //TODO: Add more flexibility to assign callbacks for different camera behaviors?

void App::framebuffer_size_callback(GLFWwindow* window, int width, int height) const
{
    glViewport(0, 0, width, height);
}

void App::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) const
{
    scene->camera->ProcessMouseScroll(yoffset);
}

void App::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (mouse_data.firstMouse)
    {
        mouse_data.lastX = xpos;
        mouse_data.lastY = ypos;
        mouse_data.firstMouse = false;
    }

    float xoffset = xpos - mouse_data.lastX;
    float yoffset = mouse_data.lastY - ypos;
    mouse_data.lastX = xpos;
    mouse_data.lastY = ypos;

    scene->camera->ProcessMouseMovement(xoffset, yoffset, false);
}

// Class functions and methods

App::App()
{
    settings = new Settings();
    scene = new Scene(settings->get_scene());
    window = init_glfw_and_create_window(settings->get_window_width(), 
            settings->get_window_height(), settings->get_window_name().c_str());
    if(!window)
        exit(1);
}

App::App(std::string path_settings)
{
    // Load the settings
    settings = new Settings(path_settings);

    // Load the scene
    scene = new Scene(settings->get_scene());

    // Load the uplifting Look Up Tables
    look_up_tables = load_luts(settings->get_path_LUTs(), settings->get_file_extension_LUTs());
    
    // And finally, initialize GLFW and create the window (cannot be done before the initialization)
    window = init_glfw_and_create_window(settings->get_window_width(), 
            settings->get_window_height(), settings->get_window_name().c_str());
    if(!window)
        exit(1);        // Can this be recovered somehow instead of killing everything?
}

void App::load_luts(const std::string& dir, const std::string& ext)
{
    // TODO: Do stuff here once you've added RGB2Spec
}

// Configure GLAD, callbacks and such, before going into the rendering loop in run()
bool App::init()
{
    // Init GLAD
    if (!gladLoaderLoadGL())
    {
      throw std::runtime_error("Error initializing glad");
      return false;
    }

    // Set viewport size
	glViewport(0, 0, settings->get_window_width(), settings->get_window_height());

	// Enable depth Z-Buffer
	glEnable(GL_DEPTH_TEST);

    // Set cursor to be locked in hte middle of the window
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set user pointer to this class so that we can register the callbacks properly
    glfwSetWindowUserPointer(window, reinterpret_cast<void *>(this));

	// Register callback function for every resize event
	glfwSetFramebufferSizeCallback(window, handle_framebuffer_resize);

	// Register mouse callback function
	glfwSetCursorPosCallback(window, handle_mouse_movement);

	// Register Scroll zoom callback
	glfwSetScrollCallback(window, handle_mouse_scroll);
}

// This one will be our main rendering loop
void App::run()
{
    /// TODO:
    // 1.- Add rgb2spec
    // 2.- load_luts in sRAT-RT
    // 3.- Class Scene (see Néstor's reference code) (and defining a scene + materials format :) )
    // 4.- Finish this method :)
    // 5.- See if I need to activate anything else in init(), i.e my own framebuffer (?)
}