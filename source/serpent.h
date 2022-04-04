#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <GL/glew.h>
#include <cglm/cglm.h>
#include <cglm/struct/cam.h>
#include <cglm/call/mat4.h>
#include <GLFW/glfw3.h>
#include "SOIL2/SOIL2.h"
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#define uniformF(x,y,z)		glProgramUniform1f(x,glGetUniformLocation(x,y),z)
#define uniformI(x,y,z)		glProgramUniform1i(x,glGetUniformLocation(x,y),z)
#define uniform4FV(x,y,z)	glProgramUniform4fv(x,glGetUniformLocation(x,y),1,z)
#define uniform3FV(x,y,z)	glProgramUniform3fv(x,glGetUniformLocation(x,y),1,z)
#define uniform4FM(x,y,z)	glProgramUniformMatrix4fv(x,glGetUniformLocation(x,y),1,GL_FALSE,z)
#define uniform3F(x,y,z,w,a)	glProgramUniform3f(x,glGetUniformLocation(x,y),z,w,a);

typedef struct _Frame Frame;
typedef struct _Program Program;

enum {
	X = 0,
	Y = 1,
	Z = 2,
	R = 0,
	G = 1,
	B = 2,
	FALSE = 0,
	TRUE = 1
};

struct Character {
	unsigned int textureID;
	int *size;
	int *bearing;
	unsigned int advance;
	float length;
};

struct CharMap {
	char c;
	struct Character Character;
};

struct Button {
	char* text;
	GLuint texture;
	char visible;
	char enabled;
	char alignment;
	int x0;
	int x1;
	int y0;
	int y1;
	float x0abs;
	float x1abs;
	float y0abs;
	float y1abs;
	float textPosX;
	float textPosY;
	float fontSize;
	float* textSize;
	void (*clickCallback)(struct Button*);
	void (*hoverCallback)(struct Button*);
	vec3 buttonColor;
	vec3 hoverColor;
	vec3 outlineColor;
	vec3 textColor;
	char hovering;
	int _frame;
};

struct _Frame {
	char visible;
	double x0;
	double x1;
	double y0;
	double y1;
};

/* form types:
string -> string
string -> int
string -> float
*/

struct Form {
	char* infoText;							// this text appears with reduced opacity when the form is empty (when text points to an empty string)
	char visible;
	char enabled;	// Toggle this off to prohibit editing this form.
	char alignment;
	int x0;
	int x1;
	int y0;
	int y1;
	float UIx0;
	float UIx1;
	float UIy0;
	float UIy1;
	float x0abs;
	float x1abs;
	float y0abs;
	float y1abs;
	float textPosX;
	float textPosY;
	float fontSize;
	float* textSize;
	vec3 formColor;
	vec3 outlineColor;
	vec3 textColor;
	char hovering;
	char hovered;
	char focused;
	char* text;								// output string.
	char* ibeam;
	int ibeam_pos;
	int limit;								// size limit of output string.
	void (*returnCallback)(char* text);		// call this when Enter is pressed. the optional argument is the string of text in the form. if this is a variable, you can alert the engine to update the objects that are using it. if it's a number variable, you can convert it from string to int/float here.
	char editing;
	int _frame;
};

// This is the base class. Everything that's needed by more than one system, that would otherwise be a global variable, is in here.
struct _Program {

	// Meta-data:
	double time;
	
	// Window system objects:
	GLFWwindow* window;
	int scrWidth;
	int scrHeight;

	// Rendering system objects:
	GLuint fontShader;
	GLuint frameShader;
	GLuint bgShader;
	GLuint arrowShader;
	GLuint* vao;
	GLuint* vbo;
	struct CharMap* Characters;
	GLuint envTex;

	// Interface system objects:
	int cursor;
	GLFWcursor* arrowCursor;
	GLFWcursor* handCursor;
	GLFWcursor* ibeamCursor;

	// Project system objects:
	Frame** frames;
	int framesI;

	// Game data:
	
};

// @Render functions:
// Functions for configuring OpenGL for the render process, moving data to the GPU, and initiating rendering on the GPU.
Program* renderInit (GLFWwindow* window);
void display (Program* program, double currentTime);
char* readShaderSource (const char *filePath);
GLuint createShaderProgram (const char* vshaderDir, const char* fshaderDir);
GLuint importTexture (const char *texImagePath);

// @Interface functions:
// Functions for constructing and managing interface objects to be rendered on the screen.
void guiInit (Program* program, char *font);
void drawText (
	Program* program,
	char* text,
	float x, float y,
	float scale,
	vec3 color,
	float alpha
);
Frame* frameNew (
	Program* program,
	double x0, double x1,
	double y0, double y1
);
void frameDraw (Program* program, Frame* frame);
void bgDraw (Program* program);

// @Utility functions:
// Functions ensuring that the window appears on the screen without any errors from GLFW or OpenGL.
void printShaderLog (GLuint shader);
void printProgramLog (int prog);
bool checkOpenGLError();
GLFWwindow* createWindow();
