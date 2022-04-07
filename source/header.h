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
#define	SCR_WIDTH_DEFAULT	1920
#define SCR_HEIGHT_DEFAULT	1080

typedef struct _Frame Frame;
typedef struct _Program Program;
typedef struct _Level Level;
typedef struct _Cell Cell;
typedef struct _Entity Entity;
typedef struct _Move Move;

enum _keys {
	KEY_RIGHT = 262,
	KEY_LEFT = 263,
	KEY_UP = 265,
	KEY_DOWN = 264
};

enum _directions {
	LEFT,
	RIGHT,
	UP,
	DOWN
};

enum _chapters {
	FOREST,
	OCEAN
};

enum _levels {
	FOREST_1,
	FOREST_2
};

enum _entities {
	// Generic:
	EMPTY,
	PLAYER	= 0000000000000001,
	BLOCK	= 0000000000000010,
	CRATE	= 0000000000000100,

	// Forest:
	GRASS	= 0000000000001000,
	TREE	= 0000000000010000,
	TURTLE	= 0000000000100000,
	APPLE	= 0000000001000000
};

enum _entity_ranks {
	// Generic:
	PLAYER_RANK = 10,
	BLOCK_RANK = 20,
	CRATE_RANK = 5,
	TURTLE_RANK = 12,
	APPLE_RANK = 2,

	// Forest:
	GRASS_RANK,
	TREE_RANK
};

enum _moveTypes {
	STEP,
	JUMP,
	SPECIAL
};

struct _Move {
	int type;

	// Step type:
	char dir;
	Move* next;
};

struct _Entity {
	int type;

	Move* moves;
	int step;
	char direction;

	// If two entities are fighting to move into the same spot and they're non-interactive with one another, then the one with a higher rank moves in.
	int rank;

	// A bit mask of the entities this interacts with, if they collide on the same tile.
	int interactions;

	// MAKE A LINKED LIST OF ENTITIES SO THEY CAN BE ACCESSED MORE EFFICIENTLY.
	Entity* next;
};

struct _Cell {
	void* occupant;
	int occupant_type;
	int background_type;
	Entity* entity;
};

struct _Level {
	Cell cell[15][15];
	Entity* entities;
};

enum _colors {
	FOREST_BG_R = 0,
	FOREST_BG_G = 8,
	FOREST_BG_B = 0,
	FOREST_R = 0,
	FOREST_G = 100,
	FOREST_B = 0
};

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
	int canvasX0;
	int canvasX1;
	int canvasY0;
	int canvasY1;

	// Rendering system objects:
	GLuint fontShader;
	GLuint frameShader;
	GLuint bgShader;
	GLuint arrowShader;
	GLuint spriteShader;
	GLuint shadowShader;
	GLuint* vao;
	GLuint* vbo;
	struct CharMap* Characters;

	// Interface system objects:
	int cursor;
	GLFWcursor* arrowCursor;
	GLFWcursor* handCursor;
	GLFWcursor* ibeamCursor;

	// Project system objects:
	Frame** frames;
	int framesI;

	// Game data:
	char chapter;
	Level* level;
	int step;
	Entity* player;
	
	// Generic textures:
	char playerDir;
	GLuint playerUpTex;
	GLuint playerDownTex;
	GLuint playerLeftTex;
	GLuint playerRightTex;
	GLuint blockTex;
	GLuint crateTex;
	GLuint appleTex;
	GLuint timeStoneTex;

	// Forest textures:
	GLuint grassTex;
	GLuint treeTex;
	GLuint turtleUpTex;
	GLuint turtleDownTex;
	GLuint turtleLeftTex;
	GLuint turtleRightTex;
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
void imageDraw (Program* program, GLuint texture, int x, int y, int width, int height, char background);
void bgDraw (Program* program);

// @Utility functions:
// Functions ensuring that the window appears on the screen without any errors from GLFW or OpenGL.
void printShaderLog (GLuint shader);
void printProgramLog (int prog);
bool checkOpenGLError();
GLFWwindow* createWindow();
void keyboard_callback (GLFWwindow* window, int key, int scancode, int action, int mods);

// @Game functions:
void load_level (Program* program, int level);
void draw_level (Program* program, int level);
char move_entity (Program* program, char X, char Y, char dir);
Entity* entityNew (Program* program, int type);
void step_level (Program* program);
void step_turtles (Program* program);
