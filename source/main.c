#include "header.h"
Program* program;

void init (Program* program) {
	srand(time(NULL));
}

int main (void) {
	GLFWwindow* window = createWindow ();

	program = renderInit (window);
	glfwGetWindowSize (program -> window, &(program -> scrWidth), &(program -> scrHeight));

	init (program);

	program -> titleTex1 = importTexture ("../sprites/hand.png");

	while (!glfwWindowShouldClose (window)) {
		//printf ("screen size = %d, %d\n", program -> scrWidth, program -> scrHeight);
		display (program, glfwGetTime ());
		glfwSwapBuffers (window);
		glfwPollEvents ();
		program -> time += 0.0001;
		program -> anim_time += 0.002;
		//printf ("anim_time = %f\n", program -> anim_time);
		if (program -> anim_time > 1.0)
			program -> anim_time = 0.0;
		if (!(program -> running)) break;
	}

	glfwDestroyWindow (window);
	glfwTerminate ();
	exit (EXIT_SUCCESS);
}
