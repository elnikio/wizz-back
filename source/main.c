#include "header.h"

void init (Program* program) {
	srand(time(NULL));
}

int main (void) {
	GLFWwindow* window = createWindow ();

	Program* program = renderInit (window);
	glfwGetWindowSize (program -> window, &(program -> scrWidth), &(program -> scrHeight));

	init (program);

	while (!glfwWindowShouldClose (window)) {
		display (program, glfwGetTime ());
		glfwSwapBuffers (window);
		glfwPollEvents ();
		program -> time += 0.0001;
	}

	glfwDestroyWindow (window);
	glfwTerminate ();
	exit (EXIT_SUCCESS);
}
