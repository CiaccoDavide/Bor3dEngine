#include <iostream>
#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>

const GLint WIDTH = 980, HEIGHT = 640;

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
	SDL_Window *window = SDL_CreateWindow("Bor3D", 0, 0, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);

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


		// if double buffering is present this will swap, otherwise not
		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}