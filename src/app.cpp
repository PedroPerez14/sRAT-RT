#include <string>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <sRAT-RT/app.h>
#include <sRAT-RT/input.h>
#include <sRAT-RT/stb_image.h>

// RENDERERS
#include <sRAT-RT/renderer_def_fwd.h>
#include <sRAT-RT/renderer_test_uplifting.h>

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

void init_imgui(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450 core");
    ImGui::StyleColorsClassic();
}

// Callbacks //TODO: Add more flexibility to assign callbacks for different camera behaviors?
void App::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    renderer->handle_resize(width, height);
}

void App::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
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
    window = init_glfw_and_create_window(settings->get_window_width(),
        settings->get_window_height(), settings->get_window_name().c_str());
    
    if(!window)
        exit(1);
}

App::App(std::string path_settings)
{
    // Load the settings
    settings = new Settings(path_settings);

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
    for(auto colorspace_key : colorspace_translations)
    {
        colorspace colorspace_value = colorspace_translations_inv.at(colorspace_key);
        std::string aux = dir + colorspace_key + ext;
        const char* full_path = aux.c_str();
        std::cout << "APP::STATUS::INIT::LOADING_LUTS: Loading " << full_path << std::endl;
        RGB2Spec* pointer = rgb2spec_load(full_path);
        if (pointer == NULL)
        {
            std::cout << "APP::STATUS::INIT::LOADING_LUTS::ERROR_NOT_FOUND: LUT " << full_path << " could not be found!" << std::endl;
        }
        else
        {
            (*look_up_tables)[colorspace_value] = pointer;
            std::cout << "APP::STATUS::INIT::LOADING_LUTS::LOADED: Loaded LUT " << full_path << std::endl;
        }
    }
}

void App::deltatime_frame_tick()
{
    float curr_frame_time = glfwGetTime();
    m_deltatime = curr_frame_time - m_lastframe_time;
    m_lastframe_time = curr_frame_time;
}

void App::process_input(GLFWwindow* window, Camera* camera, float deltaTime)
{
    // Camera movement
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera->ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera->ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera->ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera->ProcessKeyboard(RIGHT, deltaTime);

		//exit application
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, true);
		}
}

/// Configure GLAD, callbacks and such, before going into the rendering loop in run()
/// NOTE: We're going for a mix of deferred and forward pipeline, so we'll render in 2 passes
///         even if one of them does no effective work.
bool App::init()
{
    // Init GLAD
    if (!gladLoaderLoadGL())
    {
        throw std::runtime_error("Error initializing glad");
    }

    // Set viewport size
	glViewport(0, 0, settings->get_window_width(), settings->get_window_height());

    // stbi flip Y axis in textures
    stbi_set_flip_vertically_on_load(true);

	// Enable depth Z-Buffer
	glEnable(GL_DEPTH_TEST);

    // Set cursor to be locked in hte middle of the window
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);

    // Set user pointer to this class so that we can register the callbacks properly
    glfwSetWindowUserPointer(window, this);

	// Register callback function for every resize event
	glfwSetFramebufferSizeCallback(window, handle_framebuffer_resize);

	// Register mouse callback function
	glfwSetCursorPosCallback(window, handle_mouse_movement);

	// Register Scroll zoom callback
	glfwSetScrollCallback(window, handle_mouse_scroll);

    //renderer = new RendererDeferredAndForward(settings->get_window_width(), settings->get_window_height());
    renderer = new RendererTestUplifting(settings->get_window_width(), settings->get_window_height(), 
                                        look_up_tables, settings->get_colorspace(), settings->get_num_wavelengths(), 
                                        settings->get_wl_min(), settings->get_wl_max());
    m_deltatime = 0.0f;
    m_lastframe_time = 0.0f;

    init_imgui(window);

    return true;
}

void App::load_scene()
{
    scene = new Scene(settings->get_scene());
}

// This will be our main rendering loop
void App::run()
{
    while(!glfwWindowShouldClose(window))
    {
        deltatime_frame_tick();
        process_input(window, scene->get_camera(), m_deltatime);

        // Renderer virtual class added to allow easy implementation 
        //      of more renderers with different passes each
        renderer->render_scene(scene);

        renderer->render_ui();

        glfwSwapBuffers(window);
		glfwPollEvents();
    }
}

void App::cleanup()
{
    /// TODO: Should I free memory here? Or maybe once we end execution in the destructors?
    glfwTerminate();
    // Terminate ImGui execution properly and cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}