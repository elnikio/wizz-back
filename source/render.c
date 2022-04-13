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
	program -> maskShader = createShaderProgram ("../shaders/vsprite.glsl", "../shaders/fmask.glsl");
	program -> editorShader = createShaderProgram ("../shaders/vsprite.glsl", "../shaders/fimage.glsl");
	program -> shadowShader = createShaderProgram ("../shaders/vsprite.glsl", "../shaders/fshadow.glsl");
	program -> vao = vao;
	program -> vbo = vbo;
	program -> frames = malloc(sizeof(Frame*) * 100);
	program -> framesI = 0;
	program -> time = 0.0;
	program -> anim_time = 0.0;
	//program -> chapter = TITLE;
	program -> chapter = FOREST;
	program -> level = NULL;
	program -> level_id = FOREST_2;
	program -> step = 0;
	program -> screen = LEVEL_SCREEN;
	//program -> screen = TITLE_SCREEN;
	program -> sparcles = NULL;
	program -> titleMenuOption = NEW_GAME;
	program -> optionsMenuOption = FULLSCREEN;
	program -> option_fullscreen = FALSE;
	program -> option_sound_fx = FALSE;
	program -> option_music = FALSE;
	program -> running = TRUE;
	program -> pusher = NULL;
	program -> ingredients = 0;
	program -> mixed = FALSE;
	program -> time_stone = FALSE;
	program -> entity_id_last = 0;
	program -> rewinded = FALSE;
	program -> rewind_time = 0.0;
	program -> dev_menu = FALSE;
	program -> dev_menu_selected = FALSE;
	program -> editor_menu_chapter = FALSE;
	program -> editor_menu_chapter_selected = FALSE;
	program -> entity_menu_generic = FALSE;
	program -> entity_menu_generic_selected = FALSE;
	program -> input_buffer = malloc(64);
	program -> input_buffer[0] = '\0';
	program -> input_buffer_i = 0;
	program -> selector_x = 0;
	program -> selector_y = 0;

	load_level (program, program -> level_id);
	glfwGetWindowSize (program -> window, &(program -> scrWidth), &(program -> scrHeight));

	glfwSetKeyCallback(program -> window, keyboard_callback);
	glfwSetMouseButtonCallback(program -> window, mouse_callback);
	guiInit (program, "../fonts/dongle.ttf");

	// Pass data to shader programs:
	//vec4 sunDir = {-0.2, -0.4, 0.9};
	vec4 sunDir = {0.6, -0.4, 0.4};
	uniform4FV (program -> frameShader, "sunDir", sunDir);

	// Import textures:
	program -> selectorTex = importTexture ("../sprites/selector.png");
	program -> grassTex = importTexture ("../sprites/grass.png");
	program -> glyphGrassTex = importTexture ("../sprites/glyph_grass.png");
	program -> frameTex = importTexture ("../sprites/frame.png");
	program -> treeTex = importTexture ("../sprites/tree.png");
	program -> barkTex = importTexture ("../sprites/trunk.png");
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
	program -> sparcleTex = importTexture ("../sprites/sparcle.png");
	program -> gliffTex = importTexture ("../sprites/gliff.png");
	program -> boxTex = importTexture ("../sprites/box.png");
	program -> checkboxTex = importTexture ("../sprites/check_box.png");
	program -> cauldronTex = importTexture ("../sprites/cauldron.png");
	program -> carLeftTex = importTexture ("../sprites/car_left.png");
	program -> carRightTex = importTexture ("../sprites/car_right.png");
	program -> planksTex = importTexture ("../sprites/planks.png");
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

	//printf ("program -> time = %f\n", program -> time);
	//printf ("program -> rewind_time = %f\n", program -> rewind_time);

	program -> canvasY0 = 0;
	program -> canvasY1 = program -> scrHeight;
	program -> canvasX0 = program -> scrWidth / 2 - program -> scrHeight / 2;
	program -> canvasX1 = program -> scrWidth / 2 + program -> scrHeight / 2;
	
	vec3 textColor = {1.0, 0.0, 0.0};
	switch (program -> chapter) {
		case TITLE:
			glClearColor ((float)TITLE_BG_R / 255, (float)TITLE_BG_G / 255, (float)TITLE_BG_B / 255, 1.0);
			textColor[X] = sin(program -> time * 100);
			textColor[Y] = sin(program -> time * 1000);
			textColor[Z] = 1.0 - (textColor[X] + textColor[Y]);

			break;

		case FOREST:
			glClearColor ((float)FOREST_BG_R / 255, (float)FOREST_BG_G / 255, (float)FOREST_BG_B / 255, 1.0);
			textColor[X] = (float)FOREST_R / 255;
			textColor[Y] = (float)FOREST_G / 255;
			textColor[Z] = (float)FOREST_B / 255;
			break;
	}

	glClear (GL_DEPTH_BUFFER_BIT);
	glClear (GL_COLOR_BUFFER_BIT);
	
	glfwGetWindowSize (program -> window, &(program -> scrWidth), &(program -> scrHeight));

	glBindFramebuffer (GL_FRAMEBUFFER, 0);
	glDrawBuffer (GL_FRONT);
	glEnable(GL_MULTISAMPLE);


	// Menu:
	if (program -> screen == TITLE_SCREEN || program -> screen == OPTIONS_SCREEN) {
		imageDraw (program, program -> titleTex1,
			0,
			0,
			512 * program -> scrHeight / 1080,
			512 * program -> scrHeight / 1080,
			FALSE
		);
		if ((int)(program -> time * 10000) % 1 == 0) {
			sparcle_new (program);
		}
		draw_sparcles (program);
		step_sparcles(program);
		drawTextCentered (program, "MAGICAL HAND OF", 700, 720, 3.0, textColor, 1.0);
		drawTextCentered (program, "MAGIC", 1270, 720, 6.0, textColor, 1.0);
	}

	// Title Screen:
	if (program -> screen == TITLE_SCREEN) {
		//drawTextCentered (program, "continue", 950, 360, 1.6, textColor, 1.0);
		drawTextCentered (program, "new game", 950, 315, 1.6, textColor, 1.0);
		drawTextCentered (program, "options", 950, 270, 1.6, textColor, 1.0);
		drawTextCentered (program, "quit", 950, 225, 1.6, textColor, 1.0);
		switch (program -> titleMenuOption) {
			case CONTINUE:
				imageDraw (program, program -> gliffTex,
					- (program -> scrWidth) * 0.12,
					- (program -> scrHeight) * 0.32,
					(int) (0.08 * program -> scrHeight),
					(int) (0.08 * program -> scrHeight),
					FALSE
				);
				break;
			case NEW_GAME:
				imageDraw (program, program -> gliffTex,
					- (program -> scrWidth) * 0.135,
					- (program -> scrHeight) * 0.405,
					(int) (0.08 * program -> scrHeight),
					(int) (0.08 * program -> scrHeight),
					FALSE
				);
				break;
			case OPTIONS:
				imageDraw (program, program -> gliffTex,
					- (program -> scrWidth) * 0.11,
					- (program -> scrHeight) * 0.49,
					(int) (0.08 * program -> scrHeight),
					(int) (0.08 * program -> scrHeight),
					FALSE
				);
				break;
			case QUIT:
				imageDraw (program, program -> gliffTex,
					- (program -> scrWidth) * 0.08,
					- (program -> scrHeight) * 0.575,
					(int) (0.08 * program -> scrHeight),
					(int) (0.08 * program -> scrHeight),
					FALSE
				);
				break;
		}
	}

	// Options Screen:
	if (program -> screen == OPTIONS_SCREEN) {
		
		drawTextCentered (program, "fullscreen", 950, 360, 1.6, textColor, 1.0);
		GLuint box;
		box = program -> boxTex;
		if (program -> option_fullscreen)
			box = program -> checkboxTex;
		imageDraw (program, box,
			+ (program -> scrWidth) * 0.14,
			- (program -> scrHeight) * 0.32,
			(int) (0.08 * program -> scrHeight),
			(int) (0.08 * program -> scrHeight),
			FALSE
		);
		box = program -> boxTex;
		if (program -> option_sound_fx)
			box = program -> checkboxTex;
		imageDraw (program, box,
			+ (program -> scrWidth) * 0.135,
			- (program -> scrHeight) * 0.405,
			(int) (0.08 * program -> scrHeight),
			(int) (0.08 * program -> scrHeight),
			FALSE
		);
		box = program -> boxTex;
		if (program -> option_music)
			box = program -> checkboxTex;
		imageDraw (program, box,
			+ (program -> scrWidth) * 0.095,
			- (program -> scrHeight) * 0.49,
			(int) (0.08 * program -> scrHeight),
			(int) (0.08 * program -> scrHeight),
			FALSE
		);

		drawTextCentered (program, "sound fx", 950, 315, 1.6, textColor, 1.0);
		drawTextCentered (program, "music", 950, 270, 1.6, textColor, 1.0);

		switch (program -> optionsMenuOption) {
			case FULLSCREEN:
				imageDraw (program, program -> gliffTex,
					- (program -> scrWidth) * 0.13,
					- (program -> scrHeight) * 0.32,
					(int) (0.08 * program -> scrHeight),
					(int) (0.08 * program -> scrHeight),
					FALSE
				);
				break;
			case SOUND_FX:
				imageDraw (program, program -> gliffTex,
					- (program -> scrWidth) * 0.118,
					- (program -> scrHeight) * 0.405,
					(int) (0.08 * program -> scrHeight),
					(int) (0.08 * program -> scrHeight),
					FALSE
				);
				break;
			case MUSIC:
				imageDraw (program, program -> gliffTex,
					- (program -> scrWidth) * 0.10,
					- (program -> scrHeight) * 0.49,
					(int) (0.08 * program -> scrHeight),
					(int) (0.08 * program -> scrHeight),
					FALSE
				);
				break;
		}
	}

	// Level Screen:
	if (program -> screen == LEVEL_SCREEN) {
		draw_level (program, FOREST_1);
		drawText (program, "Forest - The Hut.", program -> scrWidth / 50, program -> scrHeight / 50, 1.6, textColor, 1.0);
		char* step_str = malloc(32);
		snprintf (step_str, 32, "Step: %d", program -> step);
		drawText (program, step_str, program -> scrWidth / 50, program -> scrHeight * 49 / 50 - 16, 1.6, textColor, 1.0);

		// Ingredients:
		float spriteHeight = 128.0 * (program -> scrHeight) / SCR_HEIGHT_DEFAULT;
		for (int i = 0; i < 12; i ++) {
			imageDraw (program, program -> frameTex,
				- (program -> scrWidth) * 0.95,
				spriteHeight * (i - 6),
				spriteHeight,
				spriteHeight,
				FALSE
			);
		}
		if (program -> ingredients & APPLE) {
			imageDraw (program, program -> appleTex,
				- (program -> scrWidth) * 0.95,
				spriteHeight * (0 - 6),
				spriteHeight,
				spriteHeight,
				FALSE
			);
		}
	}

	if ((program -> ingredients == APPLE)) {
		program -> mixed = TRUE;
		if ((int)(program -> time * 10000) % 1 == 0) {
			sparcle_new_at (program, 0.0, -0.12, 0.1, 32, 0.1);
		}
		draw_sparcles (program);
		step_sparcles(program);
	}
	
	// Rewind shader:
	if (program -> time_stone == TRUE && program -> rewind_time >= 0.99) {
		rewind_level (program);
		program -> time_stone = FALSE;
		program -> rewind_time = 0.0;
	}
	if (program -> time_stone && program -> rewind_time < 1.0) {
		program -> rewind_time += 0.01;
	}

	// Editor:
	float size = 128.0 * (program -> scrHeight) / SCR_HEIGHT_DEFAULT;
	float white[3] = {1.0, 1.0, 1.0};
	float gray[3] = {0.4, 0.4, 0.4};
	float yellow[3] = {1.0, 1.0, 0.0};

	if (program -> dev_menu)
		editorImageDraw (program, program -> selectorTex, program -> selector_x * size, program -> selector_y * size, size, size);
	if (program -> dev_menu > 0) {
		drawText (program, "[1] edit level", 0, 0, 0.6, gray, 1.0);
		drawText (program, "[2] reload level", 0, 0 + 12.0, 0.6, gray, 1.0);
		drawText (program, "[3] save changes", 0, 0 + 24.0, 0.6, gray, 1.0);
		drawText (program, "[4] save as", 0, 0 + 36.0, 0.6, gray, 1.0);
		switch (program -> dev_menu) {
			case 1:
				drawText (program, "[1] edit level", 0, 0, 0.6, white, 1.0);
				break;
			case 2:
				drawText (program, "[2] reload level", 0, 0 + 12.0, 0.6, white, 1.0);
				break;
			case 3:
				drawText (program, "[3] save changes", 0, 0 + 24.0, 0.6, white, 1.0);
				break;
			case 4:
				drawText (program, "[4] save as", 0, 0 + 36.0, 0.6, white, 1.0);
				break;
		}
		switch (program -> dev_menu_selected) {
			case 1:
				drawText (program, "[1] edit level", 0, 0, 0.6, yellow, 1.0);
				break;
			case 2:
				drawText (program, "[2] reload level", 0, 0 + 12.0, 0.6, yellow, 1.0);
				break;
			case 3:
				drawText (program, "[3] save changes", 0, 0 + 24.0, 0.6, yellow, 1.0);
				break;
			case 4:
				drawText (program, "[4] save as", 0, 0 + 36.0, 0.6, yellow, 1.0);
				break;
		}
	}
	if (program -> dev_menu_selected == 1) {
		drawText (program, "[1] generic", 160.0, 0 + 0.0, 0.6, gray, 1.0);
		drawText (program, "[2] woods", 160.0, 0 + 12.0, 0.6, gray, 1.0);
		drawText (program, "[3] desert", 160.0, 0 + 24.0, 0.6, gray, 1.0);
		drawText (program, "[4] ocean", 160.0, 0 + 36.0, 0.6, gray, 1.0);
		switch (program -> editor_menu_chapter) {
			case 1:
				drawText (program, "[1] generic", 160.0, 0 + 0.0, 0.6, white, 1.0);
				break;
			case 2:
				drawText (program, "[2] woods", 160.0, 0 + 12.0, 0.6, white, 1.0);
				break;
			case 3:
				drawText (program, "[3] desert", 160.0, 0 + 24.0, 0.6, white, 1.0);
				break;
			case 4:
				drawText (program, "[4] ocean", 160.0, 0 + 36.0, 0.6, white, 1.0);
				break;
		}
		switch (program -> editor_menu_chapter_selected) {
			case 1:
				drawText (program, "[1] generic", 160.0, 0 + 0.0, 0.6, yellow, 1.0);
				break;
			case 2:
				drawText (program, "[2] woods", 160.0, 0 + 12.0, 0.6, yellow, 1.0);
				break;
			case 3:
				drawText (program, "[3] desert", 160.0, 0 + 24.0, 0.6, yellow, 1.0);
				break;
			case 4:
				drawText (program, "[4] ocean", 160.0, 0 + 36.0, 0.6, yellow, 1.0);
				break;
		}
		if (program -> editor_menu_chapter_selected == 1) {
			drawText (program, "[1] player", 320.0, 0 + 0.0, 0.6, gray, 1.0);
			drawText (program, "[2] block", 320.0, 0 + 12.0, 0.6, gray, 1.0);
			drawText (program, "[3] crate", 320.0, 0 + 24.0, 0.6, gray, 1.0);
			drawText (program, "[4] cauldron", 320.0, 0 + 36.0, 0.6, gray, 1.0);
			switch (program -> entity_menu_generic) {
				case 1:
					drawText (program, "[1] player", 320.0, 0 + 0.0, 0.6, white, 1.0);
					break;
				case 2:
					drawText (program, "[2] block", 320.0, 0 + 12.0, 0.6, white, 1.0);
					break;
				case 3:
					drawText (program, "[3] crate", 320.0, 0 + 24.0, 0.6, white, 1.0);
					break;
				case 4:
					drawText (program, "[4] cauldron", 320.0, 0 + 36.0, 0.6, white, 1.0);
					break;
			}
			switch (program -> entity_menu_generic_selected) {
				case 1:
					drawText (program, "[1] player", 320.0, 0 + 0.0, 0.6, yellow, 1.0);
					break;
				case 2:
					drawText (program, "[2] block", 320.0, 0 + 12.0, 0.6, yellow, 1.0);
					break;
				case 3:
					drawText (program, "[3] crate", 320.0, 0 + 24.0, 0.6, yellow, 1.0);
					break;
				case 4:
					drawText (program, "[4] cauldron", 320.0, 0 + 36.0, 0.6, yellow, 1.0);
					break;
			}
		}
	}
	if (program -> dev_menu_selected == 4) {
		drawTextCentered (program, "[input]", program -> scrWidth / 2, program -> scrHeight / 2, 0.6, white, 1.0);
		drawTextCentered (program, program -> input_buffer, program -> scrWidth / 2, program -> scrHeight / 2 - 12.0, 0.6, yellow, 1.0);
	}

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
