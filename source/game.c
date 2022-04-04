#include "header.h"
extern Program* program;

// Level stepping:
// Every entity in the scene points to a linked list, containing future steps of that entity. A step can contain a move in one of four directions, a teleportation to a specific location, or a call to a change of state function. On every global step, the next step of every entity is evaluated in some hierarchical order. The step is removed from the list and the the entity moves to the next step. If there is no next step in the entity, it keeps state.

void clear_moves (Move* move) {
	if (move == NULL)
		return;
	if (move -> next != NULL)
		clear_moves (move -> next);
	free (move);
}

void player_add_step (Program* program, char direction) {

	Entity* player = program -> player;
	clear_moves (player -> moves);
	player -> moves = malloc(sizeof(Move));
	player -> moves -> type = STEP;
	player -> moves -> dir = direction;
	player -> moves -> next = NULL;
}

void keyboard_callback (GLFWwindow* window, int key, int scancode, int action, int mods) {
	switch (key) {
		case KEY_RIGHT:
			player_add_step (program, RIGHT);
			break;
		case KEY_LEFT:
			player_add_step (program, LEFT);
			break;
		case KEY_UP:
			player_add_step (program, UP);
			break;
		case KEY_DOWN:
			player_add_step (program, DOWN);
			break;
		default:
			return;
	}
	/*
	if (key that moves player) {
		player_add_step ();
		step_level ();
	}
	*/
	//printf("key: %d\naction: %d\n", key, action);
}

Entity* entityNew (char type) {
	Entity* entity = malloc(sizeof(Entity));
	entity -> type = type;
	entity -> moves = NULL;
	entity -> interactions = 0;
	switch (type) {
		case PLAYER:
			entity -> rank = PLAYER_RANK;
			break;
	}
	return entity;
}

void step_level (Program* program, int level) {
	
}

void load_level (Program* program, int level) {
	Level* this = malloc(sizeof(Level));

	for (int i = 0; i < 15; i ++) {
		for (int j = 0; j < 15; j ++) {
			this -> cell [i][j].occupant_type = EMPTY;
			this -> cell [i][j].background_type = EMPTY;
			this -> cell [i][j].entity = NULL;
		}
	}
	switch (level) {
		case FOREST_1:
			for (int i = 0; i < 15; i ++) {
				for (int j = 0; j < 15; j ++) {
					this -> cell [i][j].occupant_type = EMPTY;
					this -> cell [i][j].background_type = GRASS;
				}
			}
			this -> cell [4][4].entity = entityNew (PLAYER);
			program -> player = this -> cell [4][4].entity;
			this -> cell [2][4].occupant_type = CRATE;
			this -> cell [6][6].occupant_type = TREE;
			for (int i = 4; i < 10; i ++) {
				this -> cell [i][10].occupant_type = BLOCK;
			}

			program -> levels -> forest_1 = this;
			
			break;
	}
}

void tileDraw (Program* program, int x, int y, int tile_type, char background) {
	float spriteWidth = 84.0 * (program -> scrWidth) / SCR_HEIGHT_DEFAULT;
	float spriteHeight = 128.0 * (program -> scrHeight) / SCR_HEIGHT_DEFAULT;
	spriteWidth = spriteHeight;

	GLuint tex = 0;
	switch (tile_type) {
		case EMPTY:
			return;
		case GRASS:
			tex = program -> grassTex;
			break;
		case PLAYER:
			tex = program -> doieTex;
			break;
		case TREE:
			tex = program -> treeTex;
			break;
		case BLOCK:
			tex = program -> blockTex;
			break;
		case CRATE:
			tex = program -> crateTex;
			break;

		default:
			return;

	}
	imageDraw (program, tex,
		x * spriteWidth,
		y * spriteHeight,
		spriteWidth,
		spriteHeight,
		background
	);
}

void draw_cell (Program* program, Cell* cell, int X, int Y) {
	Entity* entity = cell -> entity;
	if (entity != NULL) {
		if (entity -> type == PLAYER) {
			tileDraw (program, X, Y, PLAYER, FALSE);
		}
	}
}

void draw_level (Program* program, int level) {
	//printf ("screen size = %d, %d\n", program -> scrWidth, program -> scrHeight);

	Level* this = NULL;
	switch (level) {
		case FOREST_1:
			this = program -> levels -> forest_1;
	}
	for (int i = -7; i <= 7; i ++) {
		for (int j = -7; j <= 7; j ++) {
			tileDraw (program, i, j, this -> cell [i + 7][j + 7].background_type, TRUE);
			draw_cell (program, &(this -> cell [i + 7][j + 7]), i, j);
		}
	}
}
