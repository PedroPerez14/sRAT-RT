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

		// Our T R I A N G L E S
		// Now we have C U B E S
		float vertices[] =
		{
			// positions[0-2],		texture coordinates[3-4]
			-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,	1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,	0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,
			 0.5f, -0.5f, 0.5f,		1.0f, 0.0f,
			 0.5f,  0.5f, 0.5f,		1.0f, 1.0f,
			 0.5f,  0.5f, 0.5f,		1.0f, 1.0f,
			-0.5f,  0.5f, 0.5f,		0.0f, 1.0f,
			-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,

			-0.5f,  0.5f, 0.5f,		1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,	1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,	1.0f, 0.0f,

			 0.5f,  0.5f,  0.5f,	1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,	0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,	0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,	0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,	1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,	1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,	1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,	1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,	0.0f, 1.0f,
			 0.5f, 0.5f, -0.5f,		1.0f, 1.0f,
			 0.5f, 0.5f,  0.5f,		1.0f, 0.0f,
			 0.5f, 0.5f,  0.5f,		1.0f, 0.0f,
			-0.5f, 0.5f,  0.5f,		0.0f, 0.0f,
			-0.5f, 0.5f, -0.5f,		0.0f, 1.0f
		};

		// We now want to draw 10 cubes, so we will create 10 different posiitons for them
		glm::vec3 cubePositions[] = 
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 5.0f, -15.0f),
			glm::vec3(-1.5f, -2.2f, -2.5f),
			glm::vec3(-3.8f, -2.0f, -12.3f),
			glm::vec3(2.4f, -0.4f, -3.5f),
			glm::vec3(-1.7f, 3.0f, -7.5f),
			glm::vec3(1.3f, -2.0f, -2.5f),
			glm::vec3(1.5f, 2.0f, -2.5f),
			glm::vec3(1.5f, 0.2f, -1.5f),
			glm::vec3(-1.3f, 1.0f, -1.5f)
		};


		/*
		unsigned int indices[] =
		{
			0, 1, 3,		// Triangle 1
			1, 2, 3			// Triangle 2
		};
		*/

		unsigned int VBO, VAO, EBO;

		// Vertex Array Object
		glGenVertexArrays(1, &VAO);
		// Vertex Buffer Object
		glGenBuffers(1, &VBO);
		// Element Buffer Object
		//glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		/*
			At this point we still have to tell OpenGL how to interpret the vertex data
			before doing any rendering, so we have to tell it how to do it.
			- For our vertices, we have 6*4 byte floats with other data (3 color floats)
				in the middle. Also, the first value is in the beginning of the buffer.
			We proceed as follows:
		*/
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		Shader shader("./src/shaders/vertex_camera.vert", "./src/shaders/fragment_camera.frag");

		// Texture tutorial code goes here 
		// (I didn't know where to place it so here's as good as anywhere else)
		int width, height, nChannels;
		unsigned char* data = stbi_load("./resources/textures/mario_block.png", &width, &height, &nChannels, 0);
		
		unsigned int texture1;
		glGenTextures(1, &texture1);				      // Create OpenGL texture
		glBindTexture(GL_TEXTURE_2D, texture1);		// Bind it

		// Set interpolation and wrapping options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// And generate it
		if(data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);		// Generate mipmaps too
		}
		else
		{
			std::cout << "Failed to load texture!" << std::endl;
		}
		
		stbi_image_free(data);					// We can free memory here
		
		// The second texture (Needs Y-axis flipping!)
		stbi_set_flip_vertically_on_load(true);
		data = stbi_load("./resources/textures/awesomeface.png", &width, &height, &nChannels, 0);
		unsigned int texture2;
		glGenTextures(1, &texture2);			// Create OpenGL texture
		glBindTexture(GL_TEXTURE_2D, texture2);	// Bind it

		// Set interpolation and wrapping options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// And generate it
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);		// Generate mipmaps too
		}
		else
		{
			std::cout << "Failed to load texture!" << std::endl;
		}

		stbi_image_free(data);					// We can free memory here

		// Render loop

		// Set the uniforms for the texture units
		shader.use();							// We have to activate/use before setting the uniforms!
		glActiveTexture(GL_TEXTURE0);			// Activate its unit first
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);			// Activate its unit first
		glBindTexture(GL_TEXTURE_2D, texture2);

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
			//glm::mat4 model = glm::mat4(1.0f);
			//model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

			//glm::mat4 view = glm::mat4(1.0f);
			// [OLD, NOW DOING LOOKAT FOR THE CAMERA] We want to move the camera backwards (positive Z), 
			//	so we move all the scene forward (negative Z)
			//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
			const float radius = 10.0f;
			float camX = sin(glfwGetTime()) * radius;
			float camZ = cos(glfwGetTime()) * radius;
			// We can now create a LookAt matrix (some of the steps above are done automatically here)
			glm::mat4 view = camera->GetViewMatrix();

			glm::mat4 projection = glm::mat4(1.0f);
			projection = glm::perspective(glm::radians(camera->Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
			///////////////////////////////////////////////////////////////////////////////////////////
			
			// The following ways to do it are equivalent
			glUniform1i(glGetUniformLocation(shader.ID, "texture1"), 0);
			glUniform1i(glGetUniformLocation(shader.ID, "texture2"), 0);
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
			shader.setInt("texture1", 0);
			shader.setInt("texture2", 1);
			shader.setMat4("view", view);
			shader.setMat4("projection", projection);

			glBindVertexArray(VAO);
			for (int i = 0; i < 10; i++)
			{
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, cubePositions[i]);
				float angle = 20.0f * i;
				model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
				glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
				shader.setMat4("model", model);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

			//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);		// Not now

			// Check and call events + swap buffers (show rendered stuff in screen)
			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		// De-Allocate the buffers
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);

		// Finish everything properly
		glfwTerminate();

		return 0;
}
