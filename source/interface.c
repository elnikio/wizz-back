#include "header.h"


void guiInit (Program* program, char *font) {

	program -> Characters = malloc (sizeof (struct CharMap) * 128);

	// IMPORT FONT
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		printf ("freetype init failed");
	FT_Face face;
			// Okay fonts: astronboywonder, gondrini, gunplay.
	if (FT_New_Face (ft, font, 0, &face))
		printf ("loading font failed");
	FT_Set_Pixel_Sizes (face, 0, 36);
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

	for (unsigned char i = 0; i < 128; i ++) {
		/*
		if (FT_Load_Char (face, i, FT_LOAD_RENDER))
			error ("failed to load glyph");
		*/
		FT_Load_Glyph (face, FT_Get_Char_Index (face, i), FT_LOAD_RENDER);
		//FT_Load_Glyph (face, i, FT_LOAD_RENDER);

		unsigned int texture;
		glGenTextures (1, &texture);
		glBindTexture (GL_TEXTURE_2D, texture);
		glTexImage2D (
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face -> glyph -> bitmap.width,
			face -> glyph -> bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face -> glyph -> bitmap.buffer
		);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		struct Character character;
		character.size = malloc (2*sizeof (int));
		character.size [0] = face -> glyph -> bitmap.width;
		character.size [1] = face -> glyph -> bitmap.rows;
		character.textureID = texture;
		character.bearing = malloc (2*sizeof (int));
		character.bearing [0] = face -> glyph -> bitmap_left;
		character.bearing [1] = face -> glyph -> bitmap_top;
		character.advance = face -> glyph -> advance.x;
		character.length = (face -> glyph -> advance.x >> 6);
		
		program -> Characters [i].c = i;
		program -> Characters [i].Character = character;
	}

	FT_Done_Face (face);
	FT_Done_FreeType (ft);

	// cursors
	program -> arrowCursor = glfwCreateStandardCursor (GLFW_ARROW_CURSOR);
	program -> handCursor = glfwCreateStandardCursor (GLFW_HAND_CURSOR);
	program -> ibeamCursor = glfwCreateStandardCursor (GLFW_IBEAM_CURSOR);
	program -> cursor = GLFW_ARROW_CURSOR;
}

Frame* frameNew (
	Program* program,
	double x0, double x1,
	double y0, double y1
	) {

	/*
	float UIx0 = -1 + 2 * ((float)x0 / program -> scrWidth);
	float UIx1 = -1 + 2 * ((float)x1 / program -> scrWidth);
	float UIy0 = -1 + 2 * ((float)y0 / program -> scrHeight);
	float UIy1 = -1 + 2 * ((float)y1 / program -> scrHeight);
	*/
	
	// Build button object:
	Frame *frame = malloc (sizeof (struct Button));
	frame -> visible = TRUE;
	frame -> x0 = x0;
	frame -> x1 = x1;
	frame -> y0 = y0;
	frame -> y1 = y1;

	// push frame to frames stack:
	program -> frames[(program -> framesI) ++] = frame;

	return frame;
}

// I used to handle frames by 'id', but frames do not persist across sessions, so handling them by pointers is fine.
void frameDraw (Program* program, Frame* frame) {
	// draw button
	glUseProgram (program -> frameShader);

	float x0 = frame -> x0;
	float x1 = frame -> x1;
	float y0 = frame -> y0;
	float y1 = frame -> y1;
	
	float UIx0 = -1 + 2 * (x0 / program -> scrWidth);
	float UIx1 = -1 + 2 * (x1 / program -> scrWidth);
	float UIy0 = -1 + 2 * (y0 / program -> scrHeight);
	float UIy1 = -1 + 2 * (y1 / program -> scrHeight);

	//printf ("scrWidth = %d, scrHeight = %d", program -> scrWidth, program -> scrHeight);
	//printf ("UIx0 = %f, UIx1 = %f, UIy0 = %f, UIy1 = %f\n", UIx0, UIx1, UIy0, UIy1);

	uniformF (program -> frameShader, "x0", UIx0);
	uniformF (program -> frameShader, "x1", UIx1);
	uniformF (program -> frameShader, "y0", UIy0);
	uniformF (program -> frameShader, "y1", UIy1);

	uniformF (program -> frameShader, "x0win", (const GLfloat) frame -> x0);
	uniformF (program -> frameShader, "x1win", (const GLfloat) frame -> x1);
	uniformF (program -> frameShader, "y0win", (const GLfloat) frame -> y0);
	uniformF (program -> frameShader, "y1win", (const GLfloat) frame -> y1);

	/*
	uniformF (program -> frameShader, "colorR", 1.0);
	uniformF (program -> frameShader, "colorG", 0.1);
	uniformF (program -> frameShader, "colorB", 0.4);
	*/

	glBindBuffer (GL_ARRAY_BUFFER, program -> vbo[0]);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray (0);

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);
	glDisable (GL_CULL_FACE);
	
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
	glActiveTexture (GL_TEXTURE0);

	// do the actual drawing if button is set to visible:
	if (frame -> visible) {
		glDrawArrays (GL_TRIANGLES, 0, 6);
	}
}

void editorImageDraw (Program* program, GLuint texture, int x, int y, int width, int height) {
	
	float x0 = x - width / 2;
	float x1 = x + width / 2;
	float y0 = y - height / 2;
	float y1 = y + height / 2;

	float UIx0;
	float UIx1;
	float UIy0;
	float UIy1;
	
	// Draw Object:
	glUseProgram (program -> editorShader);
	
	UIx0 = (x0 / program -> scrWidth);
	UIx1 = (x1 / program -> scrWidth);
	UIy0 = (y0 / program -> scrHeight);
	UIy1 = (y1 / program -> scrHeight);

	uniformF (program -> editorShader, "x0", UIx0);
	uniformF (program -> editorShader, "x1", UIx1);
	uniformF (program -> editorShader, "y0", UIy0);
	uniformF (program -> editorShader, "y1", UIy1);

	uniformI (program -> editorShader, "chapter", program -> chapter);
	uniformI (program -> editorShader, "background", FALSE);
	uniformF (program -> editorShader, "realtime", (const GLfloat) glfwGetTime ());
	uniformF (program -> editorShader, "time", program -> time);
	uniformF (program -> editorShader, "rewind_time", program -> rewind_time);

	glBindBuffer (GL_ARRAY_BUFFER, program -> vbo[0]);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray (0);

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);
	glDisable (GL_CULL_FACE);
	
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
	glActiveTexture (GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	glDrawArrays (GL_TRIANGLES, 0, 6);
}


void imageDraw (Program* program, GLuint texture, int x, int y, int width, int height, char background) {
	
	float x0 = x - width / 2;
	float x1 = x + width / 2;
	float y0 = y - height / 2;
	float y1 = y + height / 2;

	float UIx0;
	float UIx1;
	float UIy0;
	float UIy1;
	
	// Draw Object:
	glUseProgram (program -> spriteShader);

	
	UIx0 = (x0 / program -> scrWidth);
	UIx1 = (x1 / program -> scrWidth);
	UIy0 = (y0 / program -> scrHeight);
	UIy1 = (y1 / program -> scrHeight);

	uniformF (program -> spriteShader, "x0", UIx0);
	uniformF (program -> spriteShader, "x1", UIx1);
	uniformF (program -> spriteShader, "y0", UIy0);
	uniformF (program -> spriteShader, "y1", UIy1);

	uniformI (program -> spriteShader, "chapter", program -> chapter);
	uniformI (program -> spriteShader, "background", background);
	uniformF (program -> spriteShader, "realtime", (const GLfloat) glfwGetTime ());
	uniformF (program -> spriteShader, "time", program -> time);
	uniformF (program -> spriteShader, "rewind_time", program -> rewind_time);

	glBindBuffer (GL_ARRAY_BUFFER, program -> vbo[0]);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray (0);

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);
	glDisable (GL_CULL_FACE);
	
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
	glActiveTexture (GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	glDrawArrays (GL_TRIANGLES, 0, 6);


}

void bgDraw (Program* program) {
	// draw button
	glUseProgram (program -> bgShader);

	float x0 = 0.0;
	float x1 = program -> scrWidth;
	float y0 = 0.0;
	float y1 = program -> scrHeight;
	
	float UIx0 = -1 + 2 * (x0 / program -> scrWidth);
	float UIx1 = -1 + 2 * (x1 / program -> scrWidth);
	float UIy0 = -1 + 2 * (y0 / program -> scrHeight);
	float UIy1 = -1 + 2 * (y1 / program -> scrHeight);

	uniformF(program -> bgShader, "x0", UIx0);
	uniformF(program -> bgShader, "x1", UIx1);
	uniformF(program -> bgShader, "y0", UIy0);
	uniformF(program -> bgShader, "y1", UIy1);

	uniformF(program -> bgShader, "x0win", x0);
	uniformF(program -> bgShader, "x1win", x1);
	uniformF(program -> bgShader, "y0win", y0);
	uniformF(program -> bgShader, "y1win", y1);

	glBindBuffer (GL_ARRAY_BUFFER, program -> vbo[0]);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray (0);

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);
	glEnable (GL_CULL_FACE);
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);

	// do the actual drawing if button is set to visible:
	glDrawArrays (GL_TRIANGLES, 0, 6);
}


/*
int uiButtonNew (
	int x0, int x1,
	int y0, int y1,
	char alignment,
	char *text,
	void (*clickCallback)(struct Button*),
	void (*hoverCallback)(struct Button*),
	float fontSize,
	int _frame,
	char enabled
	) {

	float xhp = 0;
	float yhp = 0;

	// convert from corner-alignment space to window space
	if (alignment == ALIGN_BOTTOM_RIGHT || alignment == ALIGN_TOP_RIGHT) {
		x0abs = (float)GLFWwidth - (float)x1;
		x1abs = (float)GLFWwidth - (float)x0;
		UIx1 = 1 - 2 * ((float)x0 / GLFWwidth);
		UIx0 = 1 - 2 * ((float)x1 / GLFWwidth);
	} else {
		x0abs = (float)x0;
		x1abs = (float)x1;
		UIx0 = -1 + 2 * ((float)x0 / GLFWwidth);
		UIx1 = -1 + 2 * ((float)x1 / GLFWwidth);
	}
	if (alignment == ALIGN_TOP_LEFT || alignment == ALIGN_TOP_RIGHT) {
		y0abs = (float)GLFWheight - (float)y1;
		y1abs = (float)GLFWheight - (float)y0;
		UIy1 = 1 - 2 * ((float)y0 / GLFWheight);
		UIy0 = 1 - 2 * ((float)y1 / GLFWheight);
	} else {
		y0abs = (float)y0;
		y1abs = (float)y1;
		UIy0 = -1 + 2 * ((float)y0 / GLFWheight);
		UIy1 = -1 + 2 * ((float)y1 / GLFWheight);
	}
	
	float* size = uiTextSize (text, 0.0, 0.0, fontSize);
	int xmid;
	int ymid;

	if (alignment == ALIGN_BOTTOM_LEFT) {
		xmid = (x1 + x0) / 2;
		ymid = (y1 + y0) / 2;
	}
	else if (alignment == ALIGN_BOTTOM_RIGHT) {
		xmid = GLFWwidth - (x1 + x0) / 2;
		ymid = (y1 + y0) / 2;
	}
	else if (alignment == ALIGN_TOP_LEFT) {
		xmid = (x1 + x0) / 2;
		ymid = GLFWheight - (y1 + y0) / 2;
	}
	else if (alignment == ALIGN_TOP_RIGHT) {
		xmid = GLFWwidth - (x1 + x0) / 2;
		ymid = GLFWheight - (y1 + y0) / 2;
	}

	float textPosX = xmid - size[X]/2;
	float textPosY = ymid - size[Y]/2;
	
	// build button object:
	struct Button *button = malloc (sizeof (struct Button));
	button -> visible = 1;
	button -> enabled = enabled;
	button -> alignment = alignment;
	button -> x0 = x0;
	button -> x1 = x1;
	button -> y0 = y0;
	button -> y1 = y1;
	button -> x0abs = x0abs;
	button -> x1abs = x1abs;
	button -> y0abs = y0abs;
	button -> y1abs = y1abs;
	button -> UIx0 = UIx0;
	button -> UIx1 = UIx1;
	button -> UIy0 = UIy0;
	button -> UIy1 = UIy1;
	button -> clickCallback = clickCallback;
	button -> hoverCallback = hoverCallback;
	button -> text = text;
	button -> texture = 0;
	button -> textPosX = textPosX;
	button -> textPosY = textPosY;
	button -> fontSize = fontSize;
	button -> textSize = size;
	button -> buttonColor[R] = buttonColor[R];
	button -> buttonColor[G] = buttonColor[G];
	button -> buttonColor[B] = buttonColor[B];
	button -> outlineColor[R] = outlineColor[R];
	button -> outlineColor[G] = outlineColor[G];
	button -> outlineColor[B] = outlineColor[B];
	button -> hoverColor[R] = hoverColor[R];
	button -> hoverColor[G] = hoverColor[G];
	button -> hoverColor[B] = hoverColor[B];
	button -> textColor[R] = textColor[R];
	button -> textColor[G] = textColor[G];
	button -> textColor[B] = textColor[B];
	button -> hovering = FALSE;
	button -> _frame = _frame;	// underscore prefix means the variable is an in engine object handle (a pointer to a stack)

	// push button to buttons stack:
	buttons[buttoni ++] = button;

	return buttoni - 1;
}
*/

void drawText (Program* program, char* text, float x, float y, float scale, vec3 color, float alpha) {
	glUseProgram (program -> fontShader);

	glActiveTexture (GL_TEXTURE0);
	glBindVertexArray (program -> vao[0]);
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);

	for (int i = 0; text[i] != '\0'; i ++) {

		// draw shadow:
		uniform3F (program -> fontShader, "textColor", 0.0, 0.0, 0.0);
		struct Character ch = program -> Characters [text[i]].Character;

		float xpos = x + ch.bearing[X] * scale;
		float ypos = y - (ch.size[Y] - ch.bearing[Y]) * scale;
		float w = ch.size[X] * scale;
		float h = ch.size[Y] * scale;

		uniformF (program -> fontShader, "xpos", (const GLfloat) xpos);
		uniformF (program -> fontShader, "ypos", (const GLfloat) ypos);
		uniformF (program -> fontShader, "w", (const GLfloat) w);
		uniformF (program -> fontShader, "h", (const GLfloat) h);
		uniformF (program -> fontShader, "alpha", (const GLfloat) alpha);

		glBindTexture (GL_TEXTURE_2D, ch.textureID);
		glBindBuffer (GL_ARRAY_BUFFER, program -> vbo[0]);
		glDrawArrays (GL_TRIANGLES, 0, 6);
		x += (ch.advance >> 6) * scale;
		
		// draw text:
		uniform3F (program -> fontShader, "textColor", color[X], color[Y], color[Z]);
		uniformF (program -> fontShader, "xpos", (const GLfloat) xpos + 1);
		uniformF (program -> fontShader, "ypos", (const GLfloat) ypos + 1);


		glDrawArrays (GL_TRIANGLES, 0, 6);

	}
}

void drawTextCentered (Program* program, char* text, float x, float y, float scale, vec3 color, float alpha) {
	glUseProgram (program -> fontShader);

	glActiveTexture (GL_TEXTURE0);
	glBindVertexArray (program -> vao[0]);
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);

	float width = 0.0;

	for (int i = 0; text[i] != '\0'; i ++) {
		struct Character ch = program -> Characters [text[i]].Character;
		width += ch.size[X] * scale;
	}

	x -= width / 2;

	for (int i = 0; text[i] != '\0'; i ++) {

		// draw shadow:
		uniform3F (program -> fontShader, "textColor", 0.0, 0.0, 0.0);
		struct Character ch = program -> Characters [text[i]].Character;

		float xpos = x + ch.bearing[X] * scale;
		float ypos = y - (ch.size[Y] - ch.bearing[Y]) * scale;
		float w = ch.size[X] * scale;
		float h = ch.size[Y] * scale;

		uniformF (program -> fontShader, "xpos", (const GLfloat) xpos);
		uniformF (program -> fontShader, "ypos", (const GLfloat) ypos);
		uniformF (program -> fontShader, "w", (const GLfloat) w);
		uniformF (program -> fontShader, "h", (const GLfloat) h);
		uniformF (program -> fontShader, "alpha", (const GLfloat) alpha);

		glBindTexture (GL_TEXTURE_2D, ch.textureID);
		glBindBuffer (GL_ARRAY_BUFFER, program -> vbo[0]);
		glDrawArrays (GL_TRIANGLES, 0, 6);
		x += (ch.advance >> 6) * scale;
		
		// draw text:
		uniform3F (program -> fontShader, "textColor", color[X], color[Y], color[Z]);
		uniformF (program -> fontShader, "xpos", (const GLfloat) xpos + 1);
		uniformF (program -> fontShader, "ypos", (const GLfloat) ypos + 1);


		glDrawArrays (GL_TRIANGLES, 0, 6);

	}
}

