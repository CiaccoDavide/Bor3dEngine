#include <iostream>
#include <string>
#include <unordered_map>
// SDL + GLEW
#include <SDL.h>
#include <SDL_keyboard.h>
#include <SDL_keycode.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
// SOIL
#include "SOIL2\SOIL2.h" // for texture loading
// glm
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
// other includes
#include "Shader.h"
#include "Camera.h"

//const GLint WIDTH = 980, HEIGHT = 640;
const GLint WIDTH = 1600, HEIGHT = 900;
const GLchar *vertexShaderSource;
const GLchar *fragmentShaderSource;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0f;
GLfloat lastY = HEIGHT / 2.0f;
GLfloat currentFrame = 0.0f, lastFrame = 0.0f, deltaTime = 0.0f;

bool running = true, is_active_spotlight = true;
void ProcessInput(GLfloat dt);

glm::vec3 lightPosition(1.2f, 1.0f, 2.0f);

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



	//it includes SDL_ShowCursor(SDL_DISABLE); and you dont need it anymore, + you need it for relative mouse mode
	SDL_SetRelativeMouseMode(SDL_TRUE);

	/*Uint32 flags(SDL_GetWindowFlags(window));
	flags ^= SDL_WINDOW_FULLSCREEN_DESKTOP;
	SDL_SetWindowFullscreen(window, flags);*/

	// enable experimental features
	glewExperimental = GL_TRUE;

	if (GLEW_OK != glewInit()) // if not initialized
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// setup opengl viewport
	glViewport(0, 0, WIDTH, HEIGHT);

	// enable depth
	glEnable(GL_DEPTH_TEST);

	// image blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader shader("Resources/Shaders/vertex.hlsl", "Resources/Shaders/fragment.hlsl");
	Shader lightingShader("Resources/Shaders/lighting.vs", "Resources/Shaders/lighting.frag");
	Shader lightSourceShader("Resources/Shaders/light_source.vs", "Resources/Shaders/light_source.frag");
	/*
	// QUAD
	// vertices. [0,0] is the middle
	GLfloat quadVertices[] =
	{ // position[x,y,z],		color[r,g,b],			texture coords
		5.0f, 5.5f, 0.0f,		0.0f, 0.0f, 0.8f,		1.0f, 1.0f, // top right
		0.5f, -5.5f, 0.0f,		0.8f, 0.0f, 0.0f,		1.0f, 0.0f, // bottom right
		-5.5f, -5.5f, 0.0f,		0.0f, 0.8f, 0.0f,		0.0f, 0.0f, // bottom left
		-5.5f, 5.5f, 0.0f,		0.0f, 0.0f, 0.8f,		0.0f, 1.0f  // top left
	};
	// to draw a quad we got to draw two triangles
	GLuint quadIndices[] =
	{
		0, 1, 3, // first triangle
		1, 2, 3  // first triangle
	};
	*/


	// cube with normals and texture coordinates
	GLfloat vertices[] =
	{
		// Positions            // Normals              // Texture Coords
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  	1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f
	};
	float dstnc = 1.6f;
	glm::vec3 cubesPositions[] =
	{
		glm::vec3(0.0f, -1.8f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, dstnc, 0.0f),
		glm::vec3(dstnc, 0.0f, 0.0f),
		glm::vec3(-dstnc, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, dstnc),
		glm::vec3(0.0f, 0.0f, -dstnc),
		glm::vec3(dstnc, 0.0f, dstnc),
		glm::vec3(dstnc, 0.0f, -dstnc),
		glm::vec3(-dstnc, 0.0f, dstnc),
		glm::vec3(-dstnc, 0.0f, -dstnc),
		/*
		glm::vec3(2.0f, 5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f, 3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f, 2.0f, -2.5f),
		glm::vec3(1.5f, 0.2f, -1.5f),
		glm::vec3(-1.3f, 1.0f, -1.5f)*/
	};

	glm::vec3 pointLightPositions[100];
	glm::vec3 pointLightColors[] =
	{
		glm::vec3(1.0f,0.2f,0.2f),
		glm::vec3(0.2f,1.0f,0.2f),
		glm::vec3(0.2f,0.2f,1.0f)
	};

	GLuint VBO, VAO_box/*, EBO*/; // vertex buffer object and vertex array object and element buffer object
	glGenVertexArrays(1, &VAO_box);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO_box);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// vertex position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);
	// normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// texture coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	//unbind VAO
	glBindVertexArray(0);


	// for the light source
	GLuint VAO_light; // vertex buffer object and vertex array object and element buffer object
	glGenVertexArrays(1, &VAO_light);
	glBindVertexArray(VAO_light);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// vertex position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	//unbind VAO
	glBindVertexArray(0);

	GLuint diffuseMap, specularMap, emissionMap;
	glGenTextures(1, &diffuseMap);
	glGenTextures(1, &specularMap);
	glGenTextures(1, &emissionMap);
	int textureWidth, textureHeight;
	unsigned char *image;
	// diffuse map
	image = SOIL_load_image("Resources/Textures/texture_framed_wood.png", &textureWidth, &textureHeight, 0, SOIL_LOAD_RGB);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// specular map
	image = SOIL_load_image("Resources/Textures/texture_framed_wood_specular_detailed.png", &textureWidth, &textureHeight, 0, SOIL_LOAD_RGB);
	glBindTexture(GL_TEXTURE_2D, specularMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	lightingShader.Use();
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.specular"), 1);

	// create the projection matrix
	glm::mat4 projection;

	// game loop
	while (running)
	{
		// calculate deltaTime
		currentFrame = SDL_GetTicks();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		ProcessInput(deltaTime);

		// move the light source
		//lightPosition = glm::vec3(4 * sinf(currentFrame / 1600.0f), 2 * sinf(currentFrame / 2000.0f), 3 * sinf(currentFrame / 2600.0f));
		//lightPosition = glm::vec3(cosf(currentFrame / 1000.0f), 1.1f, sinf(currentFrame / 1000.0f));
		lightPosition = glm::vec3(0.0f);

		// use your shader
		lightingShader.Use();

		// MATERIAL STUFF
		GLint lightPositionLocation = glGetUniformLocation(lightingShader.Program, "light.position");
		GLint viewPositionLocation = glGetUniformLocation(lightingShader.Program, "viewPosition");
		glUniform3f(lightPositionLocation, lightPosition.x, lightPosition.y, lightPosition.z);
		glUniform3f(viewPositionLocation, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);

		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "directionalLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "directionalLight.ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "directionalLight.diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "directionalLight.specular"), 0.01f, 0.01f, 0.01f);

		// Point lights
		std::string shaderPropString;
		for (GLuint i = 0; i < 10; i++)
		{
			shaderPropString = ("pointLights[" + std::to_string(i) + "].position");
			glUniform3f(glGetUniformLocation(lightingShader.Program, shaderPropString.c_str()), pointLightPositions[i].x, pointLightPositions[i].y, pointLightPositions[i].z);
			shaderPropString = ("pointLights[" + std::to_string(i) + "].ambient");
			glUniform3f(glGetUniformLocation(lightingShader.Program, shaderPropString.c_str()), 0.05f, 0.05f, 0.05f);
			shaderPropString = ("pointLights[" + std::to_string(i) + "].diffuse");
			glUniform3f(glGetUniformLocation(lightingShader.Program, shaderPropString.c_str()), pointLightColors[i % 3].r, pointLightColors[i % 3].g, pointLightColors[i % 3].b);
			shaderPropString = ("pointLights[" + std::to_string(i) + "].specular");
			glUniform3f(glGetUniformLocation(lightingShader.Program, shaderPropString.c_str()), 2.6f*pointLightColors[i % 3].r, 2.6f*pointLightColors[i % 3].g, 2.6f*pointLightColors[i % 3].b);
			shaderPropString = ("pointLights[" + std::to_string(i) + "].constant");
			glUniform1f(glGetUniformLocation(lightingShader.Program, shaderPropString.c_str()), 1.0f);
			shaderPropString = ("pointLights[" + std::to_string(i) + "].linear");
			glUniform1f(glGetUniformLocation(lightingShader.Program, shaderPropString.c_str()), 0.09f);
			shaderPropString = ("pointLights[" + std::to_string(i) + "].quadratic");
			glUniform1f(glGetUniformLocation(lightingShader.Program, shaderPropString.c_str()), 0.032f);
		}
		if (is_active_spotlight) {
			// SpotLight
			glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
			glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
			glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.09f);
			glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.032f);
			glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutoff"), glm::cos(glm::radians(12.5f)));
			glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutoff"), glm::cos(glm::radians(15.0f)));
		}








		// generate model and view matrices
		glm::mat4 view;
		view = camera.GetViewMatrix();

		GLint modelLocation = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLocation = glGetUniformLocation(lightingShader.Program, "view");
		GLint projectionLocation = glGetUniformLocation(lightingShader.Program, "projection");
		// pass the matrices to the shader
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

		// clear the screen
		//glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
		glClearColor(0.14f, 0.14f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer

		// TEXTURE STUFF
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		//glUniform1i(glGetUniformLocation(shader.Program, "texture"), 0);

		// glm::perspective(field of view, ratio, near clipping plane, far clipping plane)
		projection = glm::perspective(camera.GetZoom(), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 1000.0f);
		//projection = glm::ortho(0.0f, (GLfloat)WIDTH, 0.0f, (GLfloat)HEIGHT, 0.1f, 1000.0f);


		// draw our object
		glBindVertexArray(VAO_box);

		// bind vertex array
		glm::mat4 model;

		model = glm::mat4();
		model = glm::translate(model, cubesPositions[0]);
		model = glm::scale(model, glm::vec3(15.0f, 0.2f, 15.0f));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		for (GLuint i = 1; i < 11; i++)
		{
			model = glm::mat4();
			model = glm::translate(model, cubesPositions[i]);
			GLfloat angle = 20.0f *(i + 1);
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// unbind
		glBindVertexArray(0);





		// light source
		lightSourceShader.Use();

		modelLocation = glGetUniformLocation(lightSourceShader.Program, "model");
		viewLocation = glGetUniformLocation(lightSourceShader.Program, "view");
		projectionLocation = glGetUniformLocation(lightSourceShader.Program, "projection");

		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4();
		model = glm::translate(model, lightPosition);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO_light);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glBindVertexArray(VAO_light);
		for (GLuint i = 0; i < 10; i++)
		{
			glUniform3f(glGetUniformLocation(lightSourceShader.Program, "color"), pointLightColors[i % 3].r, pointLightColors[i % 3].g, pointLightColors[i % 3].b);

			pointLightPositions[i] = glm::vec3(6 * cosf((1 + i) * currentFrame / 6000.0f), 0.5f, 6 * sinf((1 + i) * currentFrame / 6000.0f));
			model = glm::mat4();
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.1f));
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);

		// if double buffering is present this will swap, otherwise not
		SDL_GL_SwapWindow(window);
	}

	//delete
	glDeleteVertexArrays(1, &VAO_box);
	glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);

	// delete&destroy before exiting
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}

/* INPUT MANAGING */

enum InputDevice { KEYBOARD = 0x00000000, MOUSE = 0x22222222 };
std::unordered_map<uint32_t, uint8_t> keyMapStatus, keyMapPreStatus;

void KeyPress(uint32_t keyID, InputDevice device)
{
	keyMapStatus[keyID + device] = true;
}

void KeyRelease(uint32_t keyID, InputDevice device)
{
	keyMapStatus[keyID + device] = false;
}

uint8_t IsKeyDown(uint32_t keyID, InputDevice device)
{
	auto it = keyMapStatus.find(keyID + device);
	if (it != keyMapStatus.end())
	{
		return it->second;
	}
	else
	{
		return false;
	}
}

uint8_t WasKeyDown(uint32_t keyID, InputDevice device) {
	// We dont want to use the associative array approach here
	// because we don't want to create a key if it doesnt exist.
	// So we do it manually
	auto it = keyMapPreStatus.find(keyID + device);
	if (it != keyMapPreStatus.end()) {
		// Found the key
		return it->second;
	}
	else {
		// Didn't find the key
		return false;
	}
}

uint8_t IsKeyPressed(uint32_t keyID, InputDevice device) {
	// Check if it is pressed this frame, and wasn't pressed last frame
	if (IsKeyDown(keyID, device) == true && WasKeyDown(keyID, device) == false) {
		return true;
	}
	return false;
}

uint8_t IsKeyReleased(uint32_t keyID, InputDevice device) {
	if (IsKeyDown(keyID, device) == false && WasKeyDown(keyID, device) == true) {
		return true;
	}
	return false;
}

SDL_Event event;
GLfloat xOffset, yOffset;
void ProcessInput(GLfloat dt)
{
	//Will keep looping until there are no more events to process
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_KEYDOWN:
			KeyPress(event.key.keysym.sym, InputDevice::KEYBOARD);
			break;
		case SDL_KEYUP:
			KeyRelease(event.key.keysym.sym, InputDevice::KEYBOARD);
			break;
		case SDL_MOUSEBUTTONDOWN:
			KeyPress(event.button.button, InputDevice::MOUSE);
			break;
		case SDL_MOUSEBUTTONUP:
			KeyRelease(event.button.button, InputDevice::MOUSE);
			break;
		case SDL_MOUSEMOTION:
			if (IsKeyDown(SDL_BUTTON_RIGHT, MOUSE))
				camera.ProcessMouseMovement(event.motion.xrel, -event.motion.yrel);
			//std::cout << "mouse:" << xOffset << "," << yOffset << std::endl;
			break;
		}

	}

	if (IsKeyPressed(SDLK_LSHIFT, InputDevice::KEYBOARD))
	{
		camera.MoveFaster();
	}
	else if (IsKeyReleased(SDLK_LSHIFT, InputDevice::KEYBOARD))
	{
		camera.MoveNormal();
	}

	if (IsKeyPressed(SDLK_LCTRL, InputDevice::KEYBOARD))
	{
		camera.MoveSlower();
	}
	else if (IsKeyReleased(SDLK_LCTRL, InputDevice::KEYBOARD))
	{
		camera.MoveNormal();
	}

	if (IsKeyPressed(SDLK_f, InputDevice::KEYBOARD))
	{
		is_active_spotlight = !is_active_spotlight;
	}

	if (IsKeyDown(SDLK_w, InputDevice::KEYBOARD) || IsKeyDown(SDLK_UP, InputDevice::KEYBOARD))
	{
		camera.ProcessKeyboard(FORWARD, dt);
	}
	if (IsKeyDown(SDLK_a, InputDevice::KEYBOARD) || IsKeyDown(SDLK_LEFT, InputDevice::KEYBOARD))
	{
		camera.ProcessKeyboard(LEFT, dt);
	}
	if (IsKeyDown(SDLK_s, InputDevice::KEYBOARD) || IsKeyDown(SDLK_DOWN, InputDevice::KEYBOARD))
	{
		camera.ProcessKeyboard(BACKWARD, dt);
	}
	if (IsKeyDown(SDLK_d, InputDevice::KEYBOARD) || IsKeyDown(SDLK_RIGHT, InputDevice::KEYBOARD))
	{
		camera.ProcessKeyboard(RIGHT, dt);
	}
	if (IsKeyDown(SDLK_ESCAPE, InputDevice::KEYBOARD))
	{
		std::cout << "quit!" << std::endl;
		running = false;
	}

	// copy the keyMapStatus inside the keyMapPreStatus for the next loop
	for (auto& it : keyMapStatus) {
		keyMapPreStatus[it.first] = it.second;
	}
}


