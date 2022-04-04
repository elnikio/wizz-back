#include "header.h"

extern char windowResized;

// catch GLSL errors
void printShaderLog (GLuint shader) {
	int len = 0;
	int chWrittn = 0;
	char *log;
	glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char *) malloc (len);
		glGetShaderInfoLog (shader, len, &chWrittn, log);
		printf ("Shader Info Log:\n%s\n", log);
		free (log);
	}
}

// catch shader program errors
void printProgramLog (int prog) {
	int len = 0;
	int chWrittn = 0;
	char *log;
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char *)malloc(len);
		glGetProgramInfoLog(prog, len, &chWrittn, log);
		printf ("Program Info Log:\n%s\n", log);
		free(log);
	} 
}

// catch OpenGL errors
bool checkOpenGLError() {
	bool foundError = false;
	int glErr = glGetError();
	while (glErr != GL_NO_ERROR) {
		printf ("glError:\n%d\n", glErr);
		foundError = true;
		glErr = glGetError();
	}
	return foundError;
}

void resizeWindow (GLFWwindow* window, int width, int height) {
	windowResized = TRUE;
}

GLFWwindow* createWindow () {
	if (!glfwInit ()) { exit (EXIT_FAILURE); }
	glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow (1920, 1080, "Thallus", NULL, NULL);
	glfwSetWindowSizeCallback (window, resizeWindow);
	glfwMakeContextCurrent (window);
	if (glewInit () != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval (1);

	return window;
}
