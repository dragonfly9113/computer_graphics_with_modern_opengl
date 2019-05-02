// OpenGLCourseApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace std;

const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = 3.14159265f / 180.f;   // can be used to convert a degree number to a radian number

GLuint VAO, VBO, IBO, shader, uniformModel, uniformProjection;

bool direction = true;
float triOffset = 0.0f;
float triMaxoffset = 0.7f;
float triIncrement = 0.0001f;

float curAngle = 0.0f;
bool sizeDirection = true;
float currSize = 0.4f;
float maxSize = 0.8f;
float minSize = 0.1f;

// Vertex shader
static const char* vShader = "						\n\
#version 330										\n\
													\n\
layout (location = 0) in vec3 pos;					\n\
													\n\
out vec4 vCol;										\n\
													\n\
uniform mat4 model;									\n\
uniform mat4 projection;							\n\
													\n\
void main()											\n\
{													\n\
	gl_Position = projection * model * vec4(pos, 1.0); \n\
	vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);		\n\
}";

// Fragment shader
static const char* fShader = "						\n\
#version 330										\n\
													\n\
in vec4 vCol;										\n\
													\n\
out vec4 colour;									\n\
													\n\
void main()											\n\
{													\n\
	colour = vCol;									\n\
}";

void CreateTriangle()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.f,
		0.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 0.f,
		0.f, 1.0f, 0.f
	};

	glGenVertexArrays(1, &VAO);   // create an empty vertex array on GPU and returns its ID.
	glBindVertexArray(VAO);    // bind the vertex array ID: from now on, related gl operations will work on this vertex array.

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glGenBuffers(1, &VBO);   // create an empty vertex buffer on GPU and returns its ID.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);   // bind it to the target: GL_ARRAY_BUFFER.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);  // upload vertices data (on CPU) to the VBO on GPU.

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);  // unbind the VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  // unbind the IBO
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

	uniformModel = glGetUniformLocation(shader, "model");
	uniformProjection = glGetUniformLocation(shader, "projection");
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

	glEnable(GL_DEPTH_TEST);

	// Setup Viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);

	CreateTriangle();
	CompileShaders();

	glm::mat4 projection = glm::perspective(45.f, (GLfloat)bufferWidth / (GLfloat)bufferHeight, 0.1f, 100.f);

	// Loop until window closed
	while (!glfwWindowShouldClose(mainWindow))
	{
		// Get and handle user input events
		glfwPollEvents();

		if (direction)
		{
			triOffset += triIncrement;
		}
		else {
			triOffset -= triIncrement;
		}

		if (abs(triOffset) >= triMaxoffset) {
			direction = !direction;
		}

		curAngle += 0.01f;
		if (curAngle >= 360)
		{
			curAngle -= 360;
		}

		if (sizeDirection)
		{
			currSize += 0.001f;
		}
		else
		{
			currSize -= 0.001f;
		}

		if (currSize >= maxSize || currSize <= minSize)
		{
			sizeDirection = !sizeDirection;
		}

		// clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader);

		glm::mat4 model = glm::mat4(1.0);

		model = glm::translate(model, glm::vec3(triOffset, 0.f, -2.5f));
		//model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
	}

	return 0;
}
