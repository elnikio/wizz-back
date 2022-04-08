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

void entity_reflect_direction (Entity* entity) {
	switch (entity -> direction) {
					case LEFT:
						entity -> direction = RIGHT;
						break;
					case RIGHT:
						entity -> direction = LEFT;
						break;
					case UP:
						entity -> direction = DOWN;
						break;
					case DOWN:
						entity -> direction = UP;
						break;
	}
}

void step_turtles (Program* program) {
	Entity* entity;
	for (int i = 0; i < 15; i ++) {
		for (int j = 0; j < 15; j ++) {
			entity = program -> level -> cell[i][j].entity;
			if (entity != NULL) if (entity -> type == TURTLE) if (entity -> step < program -> step) {
					//entity_reflect_direction (entity);

				switch (entity -> direction) {
					case LEFT:
						if (!move_entity (program, i, j, LEFT)) {
							move_entity (program, i, j, RIGHT);
							entity -> direction = RIGHT;
						}
						break;
					case RIGHT:
						if (!move_entity (program, i, j, RIGHT)) {
							move_entity (program, i, j, LEFT);
							entity -> direction = LEFT;
						}
						break;
					case UP:
						if (!move_entity (program, i, j, UP)) {
							move_entity (program, i, j, DOWN);
							entity -> direction = DOWN;
						}
						break;
					case DOWN:
						if (!move_entity (program, i, j, DOWN)) {
							move_entity (program, i, j, UP);
							entity -> direction = UP;
						}
						break;
				}
			}
		}
	}
}

void keyboard_callback (GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action > 0) {
		switch (key) {
			case KEY_RIGHT:
				player_add_step (program, RIGHT);
				step_level (program);
				break;
			case KEY_LEFT:
				player_add_step (program, LEFT);
				step_level (program);
				break;
			case KEY_UP:
				player_add_step (program, UP);
				step_level (program);
				break;
			case KEY_DOWN:
				player_add_step (program, DOWN);
				step_level (program);
				break;
			default:
				return;
		}
	}
	/*
	if (key that moves player) {
		player_add_step ();
		step_level ();
	}
	*/
}

Entity* entityNew (Program* program, int type) {
	Entity* entity = malloc(sizeof(Entity));
	entity -> type = type;
	entity -> moves = NULL;
	entity -> interactions = 0;
	entity -> step = 0;
	entity -> direction = DOWN;
	entity -> next = NULL;
	switch (type) {
		case PLAYER:
			entity -> rank = PLAYER_RANK;
			break;
		case BLOCK:
			entity -> rank = BLOCK_RANK;
			break;
		case CRATE:
			entity -> rank = CRATE_RANK;
			break;
		case TURTLE:
			entity -> rank = TURTLE_RANK;
			break;
		case APPLE:
			entity -> rank = APPLE_RANK;
			break;

	}
	Level* level = program -> level;
	if (level -> entities == NULL)
		level -> entities = entity;
	else {
		Entity* last = level -> entities;
		while (last -> next != NULL)
			last = last -> next;
		last -> next = entity;
	}
	return entity;
}

// This is used to delete the first move in the queue after it's been processed.
void entity_pop_move (Entity* entity) {
	if (entity != NULL) {
	if (entity -> moves != NULL) {
		Move* second = NULL;
		char multiple_moves = ((entity -> moves -> next) != NULL);
		if (multiple_moves)
			second = entity -> moves -> next;
		free (entity -> moves);
		entity -> moves = second;
	}
	if (entity -> step < program -> step)
		entity -> step = program -> step;
	}
}

// Recursive function to push an object one tile in any of the four directions.
// Since moves are handled recursively, there has to be some way of marking that an entity has already been in moved in step X, so that it doesn't get moved twice.
char move_entity (Program* program, char X, char Y, char dir) {
	Level* level = program -> level;
	Entity* entity = level -> cell[X][Y].entity;
	
	int dX = 0;
	int dY = 0;

	switch (dir) {
		case LEFT:
			if (X <= 0)
				return 0;
			dX = -1;
			break;
		case RIGHT:
			if (X >= 14)
				return 0;
			dX = +1;
			break;
		case UP:
			if (Y >= 14)
				return 0;
			dY = +1;
			break;
		case DOWN:
			if (Y <= 0)
				return 0;
			dY = -1;
			break;
		default:
			return 0;
	}
	
	// 1. There is nothing in the target cell:
	if (level -> cell[X + dX][Y + dY].entity == NULL) {

		// Fulfill the highest ranking entity (the alpha), reject all the others.
		Entity* alpha = entity;
		int alpha_X = X;
		int alpha_Y = Y;
		int alpha_rank = entity -> rank;
		int i;
		int j;
		// Find the highest ranking contender, aka the alpha:
		for (int k = 0; k < 4; k ++) {
			switch (k) {
				case 0:
					i = +1;
					j = 0;
					break;
				case 1:
					i = -1;
					j = 0;
					break;
				case 2:
					i = 0;
					j = +1;
					break;
				case 3:
					i = 0;
					j = -1;
					break;
			}
			// Does entity exist:
			if (
				(X + dX + i >= 0) &&
				(X + dX + i <= 14) &&
				(Y + dY + j >= 0) &&
				(Y + dY + j <= 14) &&
				(dX + i != 0 || dY + j != 0)
			)
			if (level -> cell[X + dX + i][Y + dY + j].entity != NULL) {
				Entity* opponent = level -> cell[X + dX + i][Y + dY + j].entity;
				// Get next move of entity:
				Move* move = opponent -> moves;
				// If opponent is moving in and has a higher rank, promote it alpha:
				char competitor = FALSE;
				if (move != NULL) if (move -> type == STEP) {
					switch (move -> dir) {
						case LEFT:
							if (!(i + 1) && !j) competitor = TRUE;
							break;
						case RIGHT:
							if (!(i - 1) && !j) competitor = TRUE;
							break;
						case UP:
							if (!(j - 1) && !i) competitor = TRUE;
							break;
						case DOWN:
							if (!(j + 1) && !i) competitor = TRUE;
							break;
					}
					if (competitor) if (opponent -> rank > alpha_rank) {
						alpha_rank = opponent -> rank;
						alpha_X = X + dX + i;
						alpha_Y = Y + dY + j;
						alpha = opponent;
					}

				}
			}
		}
		// Reject all potential contenders who aren't the alpha:
		for (int k = 0; k < 4; k ++) {
			switch (k) {
				case 0:
					i = +1;
					j = 0;
					break;
				case 1:
					i = -1;
					j = 0;
					break;
				case 2:
					i = 0;
					j = +1;
					break;
				case 3:
					i = 0;
					j = -1;
					break;
			}
			if (
				(X + dX + i >= 0) &&
				(X + dX + i <= 14) &&
				(Y + dY + j >= 0) &&
				(Y + dY + j <= 14) &&
				(dX + i != 0 || dY + j != 0)
			) {
				Entity* opponent = level -> cell[X + dX + i][Y + dY + j].entity;
				// Does entity exist:
				if (opponent != NULL) {
					Move* move = opponent -> moves;
					// If opponent is moving in and has a higher rank, promote it alpha:
					char competitor = FALSE;
					if (move != NULL) if (move -> type == STEP) {
						switch (move -> dir) {
							case LEFT:
								if (!(i + 1) && !j) competitor = TRUE;
								break;
							case RIGHT:
								if (!(i - 1) && !j) competitor = TRUE;
								break;
							case UP:
								if (!(j - 1) && !i) competitor = TRUE;
								break;
							case DOWN:
								if (!(j + 1) && !i) competitor = TRUE;
								break;
						}
	
						// Is entity the current designated alpha:
						if ((X + dX + i != alpha_X) || (Y + dY + j != alpha_Y)) {
							entity_pop_move (opponent);
							if (opponent -> type == TURTLE)
								entity_reflect_direction (opponent);
						}
					}
				}
			}
		}
		
		// Advance the designated alpha to the target cell:
		level -> cell[X + dX][Y + dY].entity = alpha;
		level -> cell[X][Y].entity = NULL;
		entity_pop_move (alpha);

		return 1;
	}
	else return 0;

	// If there's something in the way:
	// 1. if the thing doesn't move this step, check if its rank:
	//  1.1. if it has a higher rank, do nothing.
	//  1.2. if it has a lower rank, call move_entity on it:
	// 	 1.2.1. if move succeeds, proceed with this move.
	// 	 1.2.2. if move fails, cancel this move.
	// 2. if the thing moves in any direction other than towards this entity, proceed with move as if there was nothing there.
	// 3. if the thing moves in the direction of this entity, check its rank:
	//  3.1. if it has a higher rank, call move on this entity in the opposite direction:
	// 	 3.1.1. if move succeeds, call move on other entity to this cell.
	// 	 3.1.2. if move fails, nothing moves.
	//  3.2. if it has a lower rank, call move on the entity:
	//   3.2.1. if move succeeds, proceed with move on this.
	//   3.2.2. if move fails, cancel move on this.
	// 4. if there is nothing in the way:
	//  4.1. if something is moving into the cell that this is trying to move to. check its rank:
	//   4.1.1. if it has a higher rank, let it move and cancel this move.
	//   4.2.2. if it has a lower rank, cancel its move and let this move.
	//  4.2. if nothing is moving in the cell that this is trying to move to:
	//   4.3. proceed with move.
}

void step_level (Program* program) {
	(program -> step) ++;

	Entity* entity;
	// Iterate through all cells:
	for (int i = 0; i < 15; i ++) {
		for (int j = 0; j < 15; j ++) {
			entity = program -> level -> cell [i][j].entity;
			if (entity != NULL) {
				if (entity -> moves != NULL) {
					Move* move = entity -> moves;
					switch (move -> type) {
						case STEP:
							switch (move -> dir) {
								case LEFT:
									if (entity -> step < program -> step)
									move_entity (program, i, j, move -> dir);
									program -> playerDir = LEFT;
									break;
								case RIGHT:
									if (entity -> step < program -> step)
									move_entity (program, i, j, move -> dir);
									program -> playerDir = RIGHT;
									break;
								case UP:
									if (entity -> step < program -> step)
									move_entity (program, i, j, move -> dir);
									program -> playerDir = UP;
									break;
								case DOWN:
									if (entity -> step < program -> step)
									move_entity (program, i, j, move -> dir);
									program -> playerDir = DOWN;
									break;
							}
							break;
					}
				}
			}
		}
	}
	step_turtles (program);
}

void load_level (Program* program, int level) {
	Level* this = malloc(sizeof(Level));
	program -> level = this;
	this -> entities = NULL;

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
			this -> cell [4][4].entity = entityNew (program, PLAYER);
			program -> player = this -> cell [4][4].entity;
			for (int i = 4; i < 10; i ++) {
				this -> cell [i][10].occupant_type = CRATE;
				this -> cell [i][10].entity = entityNew (program, CRATE);
			}
			this -> cell [2][2].occupant_type = TURTLE;
			this -> cell [2][2].entity = entityNew (program, TURTLE);
			this -> cell[2][2].entity -> direction = RIGHT;
			this -> cell [11][9].occupant_type = APPLE;
			this -> cell [11][9].entity = entityNew (program, APPLE);
			
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
			switch (program -> playerDir) {
				case UP:
					tex = program -> playerUpTex;
					break;
				case DOWN:
					tex = program -> playerDownTex;
					break;
				case LEFT:
					tex = program -> playerLeftTex;
					break;
				case RIGHT:
					tex = program -> playerRightTex;
					break;
			}
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
		case TURTLE:
			tex = program -> turtleUpTex;
			break;
		case APPLE:
			tex = program -> appleTex;
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
	tileDraw (program, X, Y, cell->background_type, TRUE);
	if (entity != NULL)
		tileDraw (program, X, Y, entity -> type, FALSE);
}

void draw_level (Program* program, int level) {

	Level* this = NULL;
	switch (level) {
		case FOREST_1:
			this = program -> level;
	}
	for (int i = -7; i <= 7; i ++) {
		for (int j = -7; j <= 7; j ++) {
			//tileDraw (program, i, j, this -> cell [i + 7][j + 7].background_type, TRUE);
			draw_cell (program, &(this -> cell [i + 7][j + 7]), i, j);
		}
	}
}
