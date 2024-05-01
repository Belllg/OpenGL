#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include "stb_image/stb_image.h"
#include "shader.h"
#include "Renderer.h"
#include "VBO.h"
#include "VAO.h"
#include "EBO.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void drawFPS(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 900;
const unsigned int SCR_HEIGHT = 600;
//fps calculation variables
double prevTime = 0.0;
double crntTime = 0.0;
double timeDiff;
unsigned int counter = 0;
float mixValue = 0.2f;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSwapInterval(1);//Turn ON VSinc

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// build and compile our shader program
	// ------------------------------------
	Shader ourShader("src/res/shaders/vBasic.glsl", "src/res/shaders/fBasic.glsl"); // you can name your shader files however you like

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	GLfloat vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};

	GLuint indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	VAO VAO;
	VAO.Bind();
	VBO VBO(vertices, sizeof(vertices));
	EBO EBO(indices, sizeof(indices));

	VAO.LinkVBO(VBO, 0, 3, 8, 0);
	VAO.LinkVBO(VBO, 1, 3, 8, 3);
	VAO.LinkVBO(VBO, 2, 2, 8, 6);


	//load textures
	//-------------------------------------
	unsigned int texture1, texture2;
	//first texture
	GLCALL(glGenTextures(1, &texture1));
	GLCALL(glBindTexture(GL_TEXTURE_2D, texture1));
	// set the texture wrapping/filtering options (on the currently bound texture object)
	GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	//load and genereate texture
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	stbi_uc* data = stbi_load("assets/textures/container.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
		GLCALL(glGenerateMipmap(GL_TEXTURE_2D));
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	//second texture
	GLCALL(glGenTextures(1, &texture2));
	GLCALL(glBindTexture(GL_TEXTURE_2D, texture2));
	GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	data = stbi_load("assets/textures/awesomeface.png", &width, &height, &nrChannels, 0);

	if (data)
	{
		GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
		GLCALL(glGenerateMipmap(GL_TEXTURE_2D));
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	// render loop
	// -----------
	ourShader.use();
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);

	while (!glfwWindowShouldClose(window))
	{
		drawFPS(window);

		// input
		// -----
		processInput(window);

		// set the texture mix value in the shader
		ourShader.setFloat("mixValue", mixValue);

		// render
		// ------
		GLCALL(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
		GLCALL(glClear(GL_COLOR_BUFFER_BIT));

		// render the triangle

		GLCALL(glActiveTexture(GL_TEXTURE0));
		GLCALL(glBindTexture(GL_TEXTURE_2D, texture1));
		GLCALL(glActiveTexture(GL_TEXTURE1));
		GLCALL(glBindTexture(GL_TEXTURE_2D, texture2));

		VAO.Bind();
		GLCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	VAO.Delete();
	VBO.Delete();
	EBO.Delete();
	ourShader.deleteProgram();

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		mixValue += 0.001f; // change this value accordingly (might be too slow or too fast based on system hardware)
		if (mixValue >= 1.0f)
			mixValue = 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		mixValue -= 0.001f; // change this value accordingly (might be too slow or too fast based on system hardware)
		if (mixValue <= 0.0f)
			mixValue = 0.0f;
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	GLCALL(glViewport(0, 0, width, height));
}

void drawFPS(GLFWwindow* window)
{
	crntTime = glfwGetTime();
	timeDiff = crntTime - prevTime;
	counter++;
	if (timeDiff >= 1.0 / 30)
	{
		int fps = trunc((1.0 / timeDiff) * counter);
		std::string FPS = std::to_string(fps);
		std::string ms = std::to_string((timeDiff / counter) / 1000);
		std::string newTitle = "LearnOpenGL - " + FPS + "FPS / " + ms + "ms";
		glfwSetWindowTitle(window, newTitle.c_str());
		prevTime = crntTime;
		counter = 0;
	}
}