// OpenGLCourseApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

const GLint WIDTH = 800, HEIGHT = 600;

GLuint VAO, VBO, shader;

// Vertex shader
static const char* vShader = "						\n\
#version 330										\n\
													\n\
layout (location = 0) in vec3 pos;					\n\
													\n\
void main()											\n\
{													\n\
	gl_Position = vec4(0.4*pos.x, 0.4*pos.y, pos.z, 1.0);	\n\
}";

// Fragment shader
static const char* fShader = "						\n\
#version 330										\n\
													\n\
out vec4 colour;									\n\
													\n\
void main()											\n\
{													\n\
	colour = vec4(1.0, 0.0, 0.0, 1.0);				\n\
}";

void CreateTriangle()
{
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.f,
		1.0f, -1.0f, 0.f,
		0.f, 1.0f, 0.f
	};

	glGenVertexArrays(1, &VAO);   // create an empty vertex array on GPU and returns its ID.
	glBindVertexArray(VAO);    // bind the vertex array ID: from now on, related gl operations will work on this vertex array.

	glGenBuffers(1, &VBO);   // create an empty vertex buffer on GPU and returns its ID.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);   // bind it to the target: GL_ARRAY_BUFFER.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);  // upload vertices data (on CPU) to the VBO on GPU.

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);  // unbind the VBO
	glBindVertexArray(0);   // unbind the VAO
}

void AddShader(GLuint theProgram, const GLchar* shaderCode, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);

	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);
	
	GLint result = 0;
	GLchar eLog[1024]{};

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
		//cout << "Error compiling " << shaderType << " shader: " << eLog << endl;
		printf("Error compiling %d shader: %s\n", shaderType, eLog);
		return;
	}

	glAttachShader(theProgram, theShader);
}

void CompileShaders()
{
	shader = glCreateProgram();

	if (!shader) {
		cout << "Error creating shader program!" << endl;
		return;
	}

	AddShader(shader, vShader, GL_VERTEX_SHADER);
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024]{};

	glLinkProgram(shader);   // generate executable in GPU
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		cout << "Error linking program: " << eLog << endl;
		return;
	}

	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		cout << "Error validating program: " << eLog << endl;
		return;
	}
}

int main()
{
	// Initialize GLFW
	if (!glfwInit())
	{
		cout << "GLFW initialization failed!" << endl;
		glfwTerminate();
		return 1;
	}

	// Setup GLFW window properties
	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Core profile = No backwards compatibility
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Allow forward compatibility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow *mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", nullptr, nullptr);
	if (!mainWindow)
	{
		cout << "GLFW window creation failed!" << endl;
		glfwTerminate();
		return 1;
	}

	// Get buffer size info
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Set context for GLEW to use
	glfwMakeContextCurrent(mainWindow);

	// Allow modern extension features
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		cout << "GLEW initialization failed!" << endl;
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	// Setup Viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);

	CreateTriangle();
	CompileShaders();


	// Loop until window closed
	while (!glfwWindowShouldClose(mainWindow))
	{
		// Get and handle user input events
		glfwPollEvents();

		// clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
	}

	return 0;
}
