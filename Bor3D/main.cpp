#include <iostream>
#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include "SOIL2\SOIL2.h" // for texture loading

#include "Shader.h"

const GLint WIDTH = 980, HEIGHT = 640;
const GLchar *vertexShaderSource;
const GLchar *fragmentShaderSource;

int main(int argc, char *argv[])
{
	// init SDL with
	SDL_Init(SDL_INIT_EVERYTHING);

	// opengl core profile (not compatibility)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	// select opengl version [4.4]
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);

	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	// create a new window
	SDL_Window *window = SDL_CreateWindow("Bor3D", 30, 60, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);

	// create a new context
	SDL_GLContext context = SDL_GL_CreateContext(window);

	// enable experimental features
	glewExperimental = GL_TRUE;

	if (GLEW_OK != glewInit()) // if not initialized
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// setup opengl viewport
	glViewport(0, 0, WIDTH, HEIGHT);

	SDL_Event windowEvent;

	// image blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader shader("Resources/Shaders/vertex.hlsl", "Resources/Shaders/fragment.hlsl");

	// vertices. [0,0] is the middle
	GLfloat vertices[] =
	{ // position[x,y,z],		color[r,g,b],			texture coords
		0.5f, 0.5f, 0.0f,		0.0f, 0.0f, 0.8f,		1.0f, 1.0f, // top right
		0.5f, -0.5f, 0.0f,		0.8f, 0.0f, 0.0f,		1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,		0.0f, 0.8f, 0.0f,		0.0f, 0.0f, // bottom left
		-0.5f, 0.5f, 0.0f,		0.0f, 0.0f, 0.8f,		0.0f, 1.0f  // top left
	};
	// to draw a quad we got to draw two triangles
	GLuint indices[] =
	{
		0, 1, 3, // first triangle
		1, 2, 3  // first triangle
	};

	GLuint VBO, VAO, EBO; // vertex buffer object and vertex array object and element buffer object
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// vertex position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);
	// color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// texture coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	//unbind VAO
	glBindVertexArray(0);

	// texture creation and binding
	GLuint texture;
	int width, height;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char *image = SOIL_load_image("Resources/Textures/texture_wood_painted.jpg", &width, &height, 0, SOIL_LOAD_RGBA);
	if (image == nullptr)
	{
		std::cout << SOIL_last_result() << std::endl;
	}
	else
	{
		std::cout << "TEXTURE: " << *image << std::endl;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
	

	// game loop
	while (true)
	{
		if (SDL_PollEvent(&windowEvent))
		{
			if (SDL_QUIT == windowEvent.type) {
				break;
			}
		}

		// clear the screen
		glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		// DRAW OPENGL HERE
		shader.Use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(shader.Program, "texture"), 0);

		// bind vertex array
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		// unbind
		glBindVertexArray(0);


		// if double buffering is present this will swap, otherwise not
		SDL_GL_SwapWindow(window);
	}

	//delete
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	// delete&destroy before exiting
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}