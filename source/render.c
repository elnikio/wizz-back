#include "header.h"

char windowResized = FALSE;

GLuint importTexture (const char *texImagePath) {
	GLuint textureID;
	textureID = SOIL_load_OGL_texture (texImagePath, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (textureID == 0) printf ("\n[] [] [] Could not find texture file %s. [] [] []\n", texImagePath);
	return textureID;
}

Program* renderInit (GLFWwindow* window) {
	Program* program = malloc(sizeof(Program));
	GLuint* vao = malloc(sizeof(GLuint));
	GLuint* vbo = malloc(sizeof(GLuint));

	glGenVertexArrays (1, vao);
	glBindVertexArray (vao[0]);

	float squareVerts [18] = {
		0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0, 0.0,
		1.0, -1.0, 0.0, 1.0, 0.0, 0.0, 0.0, -1.0, 0.0
	};

	glGenBuffers (1, vbo);
	
	glBindBuffer (GL_ARRAY_BUFFER, vbo[0]);
	glBufferData (GL_ARRAY_BUFFER, sizeof (squareVerts), squareVerts, GL_STATIC_DRAW);

	program -> window = window;
	program -> fontShader = createShaderProgram ("../shaders/vfont.glsl", "../shaders/ffont.glsl");
	program -> frameShader = createShaderProgram ("../shaders/vframe.glsl", "../shaders/fframe.glsl");
	program -> bgShader = createShaderProgram ("../shaders/vbg.glsl", "../shaders/fbg.glsl");
	program -> arrowShader = createShaderProgram ("../shaders/varrow.glsl", "../shaders/farrow.glsl");
	program -> spriteShader = createShaderProgram ("../shaders/vsprite.glsl", "../shaders/fsprite.glsl");
	program -> shadowShader = createShaderProgram ("../shaders/vsprite.glsl", "../shaders/fshadow.glsl");
	program -> vao = vao;
	program -> vbo = vbo;
	program -> frames = malloc(sizeof(Frame*) * 100);
	program -> framesI = 0;
	program -> time = 0.0;
	program -> chapter = FOREST;
	program -> level = NULL;

	load_level (program, FOREST_1);
	glfwGetWindowSize (program -> window, &(program -> scrWidth), &(program -> scrHeight));

	glfwSetKeyCallback(program -> window, keyboard_callback);
	guiInit (program, "../fonts/dongle.ttf");

	// Pass data to shader programs:
	//vec4 sunDir = {-0.2, -0.4, 0.9};
	vec4 sunDir = {0.6, -0.4, 0.4};
	uniform4FV (program -> frameShader, "sunDir", sunDir);

	// Import textures:
	program -> grassTex = importTexture ("../sprites/grass.png");
	program -> treeTex = importTexture ("../sprites/trunk.png");
	program -> playerUpTex = importTexture ("../sprites/hand_up.png");
	program -> playerDownTex = importTexture ("../sprites/hand_down.png");
	program -> playerLeftTex = importTexture ("../sprites/hand_left.png");
	program -> playerRightTex = importTexture ("../sprites/hand_right.png");
	program -> turtleUpTex = importTexture ("../sprites/turtle_up.png");
	program -> turtleDownTex = importTexture ("../sprites/turtle_down.png");
	program -> turtleLeftTex = importTexture ("../sprites/turtle_left.png");
	program -> turtleRightTex = importTexture ("../sprites/turtle_right.png");
	program -> appleTex = importTexture ("../sprites/apple.png");
	program -> blockTex = importTexture ("../sprites/block.png");
	program -> crateTex = importTexture ("../sprites/crate.png");
	program -> timeStoneTex = importTexture ("../sprites/time_stone.png");
	program -> playerDir = UP;

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);
	glEnable (GL_CULL_FACE);
	glDepthFunc (GL_ALWAYS);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return program;
}

void display (Program* program, double currentTime) {

	program -> canvasY0 = 0;
	program -> canvasY1 = program -> scrHeight;
	program -> canvasX0 = program -> scrWidth / 2 - program -> scrHeight / 2;
	program -> canvasX1 = program -> scrWidth / 2 + program -> scrHeight / 2;
	
	vec3 textColor = {1.0, 0.0, 0.0};
	switch (program -> chapter) {
		case FOREST:
			glClearColor ((float)FOREST_BG_R / 255, (float)FOREST_BG_G / 255, (float)FOREST_BG_B / 255, 1.0);
			textColor[X] = (float)FOREST_R / 255;
			textColor[Y] = (float)FOREST_G / 255;
			textColor[Z] = (float)FOREST_B / 255;
			break;
	}
	glClear (GL_DEPTH_BUFFER_BIT);
	glClear (GL_COLOR_BUFFER_BIT);

	draw_level (program, FOREST_1);
	
	glfwGetWindowSize (program -> window, &(program -> scrWidth), &(program -> scrHeight));

	glBindFramebuffer (GL_FRAMEBUFFER, 0);
	glDrawBuffer (GL_FRONT);
	glEnable(GL_MULTISAMPLE);

	drawText (program, "Chapter 1 - The Hut.", program -> scrWidth / 50, program -> scrHeight / 50, 1.6, textColor, 1.0);
	windowResized = FALSE;
}

char* readShaderSource (const char *filePath) {
	char *content = malloc(5000);
	char c;
	int i = 0;
	FILE* fileStream = fopen (filePath, "r");
	while ((c = getc (fileStream)) != EOF)
		content [i ++] = c;

	content [i] = '\0';
	fscanf (fileStream, "%s", content);
	fclose (fileStream);
	return content;
}

GLuint createShaderProgram (const char* vshaderDir, const char* fshaderDir) {
	GLint vertCompiled;
	GLint fragCompiled;
	GLint linked;

	char* vshaderSource = readShaderSource (vshaderDir);
	char* fshaderSource = readShaderSource (fshaderDir);

	//printShaderSource ("Vertex Shader", vshaderSource);
	//printShaderSource ("Fragment Shader", fshaderSource);
	
	GLuint vShader = glCreateShader (GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader (GL_FRAGMENT_SHADER);

	glShaderSource (vShader, 1, (const char* const*)&vshaderSource, NULL);
	glShaderSource (fShader, 1, (const char* const*)&fshaderSource, NULL);
	free (vshaderSource);

	// compile Vertex shader
	glCompileShader (vShader);
	checkOpenGLError();
	glGetShaderiv (vShader, GL_COMPILE_STATUS, &vertCompiled);
	if (vertCompiled != 1) {
		printf ("\e[0;31m[ ERROR ]\e[0m vertex compilation failed.\n");
		printShaderLog (vShader);
	}

	// compile Fragment shader
	glCompileShader (fShader);
	checkOpenGLError();
	glGetShaderiv (fShader, GL_COMPILE_STATUS, &fragCompiled);
	if (fragCompiled != 1) {
		printf ("\e[0;31m[ ERROR ]\e[0m fragment compilation failed.\n");
		printShaderLog (fShader);
	}

	// link shaders into shader program
	GLuint vfProgram = glCreateProgram ();
	glAttachShader (vfProgram, vShader);
	glAttachShader (vfProgram, fShader);
	glLinkProgram (vfProgram);
	checkOpenGLError ();
	glGetProgramiv (vfProgram, GL_LINK_STATUS, &linked);
	if (linked != 1) {
		printf ("\e[0;31m[ ERROR ]\e[0m program linking failed.\n");

		printProgramLog (vfProgram);
	}
	return vfProgram;
}
