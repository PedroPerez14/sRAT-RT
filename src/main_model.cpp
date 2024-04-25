#define GLFW_INCLUDE_NONE

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <glm/glm.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <thread>

#include <sRAT-RT/shader.h>
#include <sRAT-RT/stb_image.h>
#include <sRAT-RT/camera.h>
#include <sRAT-RT/model.h>
#include <glm/gtc/type_ptr.hpp>

//////////////////////////////////////
	// Camera initialization
	float lastX = 400;
	float lastY = 300;
	bool firstMouse = true;
	Camera* camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	//////////////////////////////////////
	// Deltatime stuff
	float deltaTime = 0.0f;				// Time between last and current frame
	float lastFrame = 0.0f;				// Time of last frame
	//////////////////////////////////////

	void mouse_callback(GLFWwindow* window, double xpos, double ypos)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;

		camera->ProcessMouseMovement(xoffset, yoffset, false);
	}

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		camera->ProcessMouseScroll(yoffset);
	}

	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

void processInput(GLFWwindow* window, Camera* camera, float deltaTime)
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

int main()
{
  	std::cout << "HELLO THERE" << std::endl;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "sRAT-RT", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create a GLFW Window!" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// Now, GLAD
	/*
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD!" << std::endl;
			return -1;
		}
    */

    if (!gladLoaderLoadGL())
    {
      throw std::runtime_error("Error initializing glad");
      return -1;
    }

	// Set viewport size
	glViewport(0, 0, 800, 600);

	// Set flip for stbi textures
	stbi_set_flip_vertically_on_load(true);

	// Enable depth Z-Buffer
	glEnable(GL_DEPTH_TEST);

	// Set cursor to be locked in hte middle of the window
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Register callback function for every resize event
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Register mouse callback function
	glfwSetCursorPosCallback(window, mouse_callback);

	// Register Scroll zoom callback
	glfwSetScrollCallback(window, scroll_callback);


	Shader shader("./src/shaders/model_test.vert", "./src/shaders/model_test.frag");
	Model ourModel("./resources/objects/backpack/backpack.obj");

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		//Delta time calculations
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		// Input processing
		processInput(window, camera, deltaTime);

		// Render commands will go here
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();

		///////////////////////////////////////////////////////////////////////////////////////////
		// GOING 3D!
		///////////////////////////////////////////////////////////////////////////////////////////
		// We can now create a LookAt matrix (some of the steps above are done automatically here)
		glm::mat4 view = camera->GetViewMatrix();

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(camera->Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
		///////////////////////////////////////////////////////////////////////////////////////////

		// Set the matrix data in the shader
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		shader.setMat4("model", model);
		ourModel.draw(shader);

		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);		// Not now

		// Check and call events + swap buffers (show rendered stuff in screen)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// Finish everything properly
	glfwTerminate();

	return 0;
}
