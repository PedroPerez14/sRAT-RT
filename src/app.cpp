#include <string>
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
    scene->get_camera()->ProcessMouseScroll(yoffset);
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

    scene->get_camera()->ProcessMouseMovement(xoffset, yoffset, false);
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
    look_up_tables = new std::unordered_map<colorspace, RGB2Spec*>();
    load_luts(settings->get_path_LUTs(), settings->get_file_extension_LUTs());
    
    // And finally, initialize GLFW and create the window (cannot be done before the initialization)
    window = init_glfw_and_create_window(settings->get_window_width(), 
            settings->get_window_height(), settings->get_window_name().c_str());
    if(!window)
        exit(1);        // Can this be recovered somehow instead of killing everything?
}

App::~App()
{
    for (auto& it: (*look_up_tables))
    {
        rgb2spec_free(it.second);
        it.second = nullptr;
    }
    free(look_up_tables);
}

void App::load_luts(const std::string& dir, const std::string& ext)
{   
    // God forgive me for this awful way to do this
    for(std::string colorspace_key : colorspace_translations)
    {
        colorspace colorspace_value = colorspace_translations_inv.at(colorspace_key);
        const char* full_path = (dir + colorspace_key + ext).c_str();
        RGB2Spec* pointer = rgb2spec_load(full_path);
        (*look_up_tables)[colorspace_value] = pointer;
        std::cout << "APP::STATUS::INIT::LOADING_LUTS::LOADED: Loaded LUT " << full_path << std::endl;
    }
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
    return true;
}

// This one will be our main rendering loop
void App::run()
{
    /// TODO:
    // 1.- Add rgb2spec             [DONE]
    // 2.- load_luts in sRAT-RT     [DONE]
    // 3.- Class Scene (see Néstor's reference code) (and defining a scene + materials format :) )
    // 4.- Finish this run() method :)
    // 5.- See if I need to activate anything else in init(), i.e my own framebuffer (See Néstor's gist for the framebuffer)
    // 6.- I forgor
}