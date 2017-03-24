#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#define WINDOW_TITLE_PREFIX "Bor3d Engine"

int CurrentWidth = 800;
int CurrentHeight = 600;
int WindowHandle = 0; // used to store the handle to the window created by FreeGLUT

unsigned FrameCount = 0;

void Initialize(int, char*[]);
void InitWindow(int, char*[]);
void ResizeFunction(int, int);
void RenderFunction(void);
void TimerFunction(int);
void IdleFunction(void);

int main(int argc, char* argv[])
{
	Initialize(argc, argv); // initialize the window using FreeGLUT

	glutMainLoop(); // the main loop, where the magic happens!

	exit(EXIT_SUCCESS); // exit(0);
}

void Initialize(int argc, char* argv[])
{
	GLenum GlewInitResult;

	InitWindow(argc, argv); // glutInit() setup

	GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult) {
		fprintf(
			stderr,
			"ERROR: %s\n",
			glewGetErrorString(GlewInitResult)
			);
		exit(EXIT_FAILURE);
	}

	fprintf(
		stdout,
		"INFO: OpenGL Version: %s\n",
		glGetString(GL_VERSION)
		);

	glClearColor(0.1f, 0.1f, 0.1f, 0.0f); // background color
}

void InitWindow(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitContextVersion(4, 0);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutSetOption(
		GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS
		);

	glutInitWindowSize(CurrentWidth, CurrentHeight);

	glutInitDisplayMode(
		GLUT_DEPTH |  // enable z-buffer usage
		GLUT_DOUBLE | // enable double-buffering usage (to reduce image flickering)
		GLUT_RGBA	  // color composition (r,g,b,a)
		);

	WindowHandle = glutCreateWindow(WINDOW_TITLE_PREFIX);

	if (WindowHandle < 1) {
		fprintf(
			stderr,
			"ERROR: Could not create a new rendering window.\n"
			);
		exit(EXIT_FAILURE);
	}

	glutReshapeFunc(ResizeFunction); // links the function that will manage the window resizing events
	glutDisplayFunc(RenderFunction); // links the function that will manage the scene rendering
	glutIdleFunc(IdleFunction);
	glutTimerFunc(0, TimerFunction, 0); // remember that v-sync depends on the gpu driver settings!
}

void ResizeFunction(int Width, int Height)
{
	CurrentWidth = Width;
	CurrentHeight = Height;
	glViewport(0, 0, CurrentWidth, CurrentHeight); // viewport(x, y, width, height)
}

void RenderFunction(void)
{
	++FrameCount; // increase the frame counter each time a frame is rendered

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // reset some buffers: back-buffer (to glClearColor) and z-buffer

	// here we can draw on the back-buffer

	glutSwapBuffers(); // swaps the back-buffer with the front-buffer, rendering the drawn scene to our window!
}

void IdleFunction(void)
{
	glutPostRedisplay(); //tells FreeGLUT to redraw as soon as possible
}

void TimerFunction(int Value)
{
	if (0 != Value) {
		char* TempString = (char*)
			malloc(512 + strlen(WINDOW_TITLE_PREFIX));

		sprintf(
			TempString,
			"%s: %d Frames Per Second @ %d x %d",
			WINDOW_TITLE_PREFIX,
			FrameCount * 4,
			CurrentWidth,
			CurrentHeight
			);

		glutSetWindowTitle(TempString);
		free(TempString);
	}

	FrameCount = 0;
	glutTimerFunc(250, TimerFunction, 1);
}