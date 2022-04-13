#include "header.h"
extern Program* program;

// Level stepping:
// Every entity in the scene points to a linked list, containing future steps of that entity. A step can contain a move in one of four directions, a teleportation to a specific location, or a call to a change of state function. On every global step, the next step of every entity is evaluated in some hierarchical order. The step is removed from the list and the the entity moves to the next step. If there is no next step in the entity, it keeps state.

// This needs to be inmplemented some time before the singularity.
void entityFree (Entity* entity) {
	
}

// Rewind time to 0. This is called when you take the stone.
void rewind_level (Program* program) {
	program -> rewinded = TRUE;
	printf ("REWINDING!\n");
	Entity* entity = program -> level -> entities;
	Entity* ghost = program -> level -> ghosts;
	Entity* player_ghost;

	// Replace every entity with its ghost:
	while (entity != NULL) {
		if (entity -> type != PLAYER) {
			entityFree (entity);
			entity = ghost;
			entity -> step = program -> step;
		}
		else
			player_ghost = ghost;
		entity = entity -> next;
		ghost = ghost -> next;
	}
	program -> level -> cell[program -> player -> spawnX][program -> player -> spawnY].entity = entityNew (program, PLAYER, program -> player -> spawnX, program -> player -> spawnY);
	program -> level -> cell[program -> player -> spawnX][program -> player -> spawnY].entity -> moves = player_ghost -> moves;

	// Move every entity other than the player to its' spawn location:
	for (int i = 0; i < 15; i ++) {
		for (int j = 0; j < 15; j ++) {
			entity = program -> level -> cell [i][j].entity;
			if (entity != NULL) if (entity -> id != program -> player -> id) {
				if (entity -> spawnX != i || entity -> spawnY != j) {
					program -> level -> cell [entity -> spawnX][entity -> spawnY].entity = entity;
					program -> level -> cell [i][j].entity = NULL;
				}
			}
		}
	}
}

void clear_moves (Move* move) {
	if (move == NULL)
		return;
	if (move -> next != NULL)
		clear_moves (move -> next);
	free (move);
}

void player_add_step (Program* program, char direction) {

	// Clear backlog and add move:
	Entity* player = program -> player;
	clear_moves (player -> moves);
	player -> moves = malloc(sizeof(Move));
	player -> moves -> type = STEP;
	player -> moves -> dir = direction;
	player -> moves -> next = NULL;

	// Append move to ghost:
	if (!(program -> rewinded)) {
		Entity* ghost = program -> player_ghost;
		Move* move = malloc (sizeof(Move));
		if (ghost -> moves == NULL)
			ghost -> moves = move;
		else {
			Move* last = ghost -> moves;
			while (last -> next != NULL)
				last = last -> next;
			last -> next = move;
		}
		move -> type = STEP;
		move -> dir = direction;
		move -> next = NULL;
	}
}

void entity_add_step (Program* program, Entity* entity, char direction) {

	// Add move to entity:
	Move* move = malloc(sizeof(Move));
	move -> type = STEP;
	move -> dir = direction;
	move -> next = NULL;

	if (entity -> moves == NULL)
		entity -> moves = move;
	else {
		Move* last = entity -> moves;
		while (last -> next != NULL)
			last = last -> next;
		last -> next = move;
	}
	
	// Add move to ghost:
	Entity* ghost = program -> level -> ghosts;
	move = malloc (sizeof(Move));
	move -> type = STEP;
	move -> dir = direction;
	move -> next = NULL;

	while (ghost -> id != entity -> id) ghost = ghost -> next;

	if (ghost -> moves == NULL)
		ghost -> moves = move;
	else {
		Move* last = ghost -> moves;
		while (last -> next != NULL)
			last = last -> next;
		last -> next = move;
	}
}

void entity_add_jump (Program* program, Entity* entity, int x, int y) {

	Move* move = malloc(sizeof(Move));
	move -> type = JUMP;
	move -> x = x;
	move -> y = y;
	move -> next = NULL;

	// Add jump to entity:
	if (entity -> moves == NULL)
		entity -> moves = move;
	else {
		Move* last = entity -> moves;
		while (last -> next != NULL)
			last = last -> next;
		last -> next = move;
	}

	// Add jump to ghost:
	Entity* ghost = program -> level -> ghosts;
	while (ghost -> id != entity -> id) ghost = ghost -> next;

	if (ghost -> moves == NULL)
		ghost -> moves = move;
	else {
		Move* last = ghost -> moves;
		while (last -> next != NULL)
			last = last -> next;
		last -> next = move;
	}
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

Sparcle* sparcle_new_at (Program* program, float posX, float posY, float range, int life, float velZ) {
	Sparcle* this;
	char found_corpse = FALSE;
	if (program -> sparcles == NULL) {
		this = malloc(sizeof(Sparcle));
		program -> sparcles = this;
	}
	else {
		this = program -> sparcles;
		while (this -> next != NULL) {
			if (this -> alive == FALSE) {
				found_corpse = TRUE;
				break;
			}
			this = this -> next;
		}
		if (!found_corpse) {
			this -> next = malloc(sizeof(Sparcle));
			this = this -> next;
		}
	}

	this -> posX = posX * program -> scrWidth;
	this -> posY = posY * program -> scrHeight;
	this -> life = life;
	float rangeX = program -> scrWidth / 10.0 * range;
	float rangeY = program -> scrHeight / 10.0 * range;
	this -> velX = 0.0;
	this -> velY = 0.0;
	this -> velZ = 0.0;
	while (this -> velX == 0.0)
		this -> velX = ((float)rand()) / RAND_MAX * rangeX - rangeX/2;
	while (this -> velY == 0.0)
		this -> velY = ((float)rand()) / RAND_MAX * rangeY - rangeY/2;
	while (this -> velZ == 0.0)
		this -> velZ = ((float)rand()) / RAND_MAX * 128.0 * velZ;
	this -> height = 0.0;
	if (!found_corpse)
		this -> next = NULL;
	this -> alive = TRUE;
}


Sparcle* sparcle_new (Program* program) {
	Sparcle* this;
	char found_corpse = FALSE;
	if (program -> sparcles == NULL) {
		this = malloc(sizeof(Sparcle));
		program -> sparcles = this;
	}
	else {
		this = program -> sparcles;
		while (this -> next != NULL) {
			if (this -> alive == FALSE) {
				found_corpse = TRUE;
				break;
			}
			this = this -> next;
		}
		if (!found_corpse) {
			this -> next = malloc(sizeof(Sparcle));
			this = this -> next;
		}
	}

	this -> posX = 0.0;
	this -> posY = 0.0;
	this -> life = 200;
	float rangeX = program -> scrWidth / 10.0;
	float rangeY = program -> scrHeight / 10.0;
	this -> velX = 0.0;
	this -> velY = 0.0;
	this -> velZ = 0.0;
	while (this -> velX == 0.0)
		this -> velX = ((float)rand()) / RAND_MAX * rangeX - rangeX/2;
	while (this -> velY == 0.0)
		this -> velY = ((float)rand()) / RAND_MAX * rangeY - rangeY/2;
	while (this -> velZ == 0.0)
		this -> velZ = ((float)rand()) / RAND_MAX * 128.0;
	this -> height = 0.0;
	if (!found_corpse)
		this -> next = NULL;
	this -> alive = TRUE;
}

void step_sparcles (Program* program) {
	Sparcle* this = program -> sparcles;
	//printf ("sparcles:\n");
	while (this != NULL) {
	
		if (this -> alive) {
			// Momentum:
			this -> posX += this -> velX;
			this -> posY += this -> velY;
			this -> posY += this -> velZ;
			this -> height += this -> velZ;
			this -> velZ -= 0.98;
			if (this -> height <= 0.0)
				this -> velZ = - this -> velZ;

			// Resistance:
			this -> velX *= 0.95;
			this -> velY *= 0.95;
			this -> velZ *= 0.95;
	
			/*
			if (this -> velZ < 2.0 && this -> velZ > -2.0 && this -> height < 10)
				this -> alive = FALSE;
				*/
			if (this -> life == 0)
				this -> alive = FALSE;


			this -> life -= 1;
			//printf ("life: %d\n", this -> life);
	
		}
		this = this -> next;
	}
}

void draw_sparcles (Program* program) {
	Sparcle* this = program -> sparcles;
	while (this != NULL) {
		if (this -> alive) {
			imageDraw (program, program -> sparcleTex,
				this -> posX,
				this -> posY,
				(int) (0.02 * program -> scrHeight),
				(int) (0.02 * program -> scrHeight),
				FALSE
			);
		}
		this = this -> next;
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
							program -> pusher = NULL;
							move_entity (program, i, j, RIGHT);
							entity -> direction = RIGHT;
						}
						break;
					case RIGHT:
						if (!move_entity (program, i, j, RIGHT)) {
							program -> pusher = NULL;
							move_entity (program, i, j, LEFT);
							entity -> direction = LEFT;
						}
						break;
					case UP:
						if (!move_entity (program, i, j, UP)) {
							program -> pusher = NULL;
							move_entity (program, i, j, DOWN);
							entity -> direction = DOWN;
						}
						break;
					case DOWN:
						if (!move_entity (program, i, j, DOWN)) {
							program -> pusher = NULL;
							move_entity (program, i, j, UP);
							entity -> direction = UP;
						}
						break;
				}
			}
		}
	}
}

void save_level (Program* program, char* name) {
	char* dir = malloc(64);
	snprintf (dir, 64, "%s", name);
	FILE *fp = fopen (name, "w+");

	Level* level = program -> level;
	Entity* entity = level -> entities;
	Entity* ghost = level -> ghosts;

	// Meta-data:
	fprintf (fp, "id: %d\n", level -> id);
	fprintf (fp, "name: %s\n", level -> name);

	// Cells:
	fprintf (fp, "cells:\n");
	for (int i = 0; i < 15; i ++) {
		for (int j = 0; j < 15; j ++) {
			if (level -> cell[i][j].entity != NULL)
				fprintf (fp, " entity id: %d\n", level -> cell[i][j].entity -> id);
			else
				fprintf (fp, " entity id: NULL\n");
		}
	}

	// Entities:
	fprintf (fp, "entities:\n");
	while (entity != NULL) {

		// Meta-data:
		fprintf (fp, " id: %d\n", entity -> id);
		fprintf (fp, " type: %d\n", entity -> type);
		fprintf (fp, " spawnX: %d\n", entity -> spawnX);
		fprintf (fp, " spawnY: %d\n", entity -> spawnY);

		Move* move = entity -> moves;
		fprintf (fp, " entities:\n");
		while (move != NULL) {
			fprintf (fp, "  type: %d\n", move -> type);
			fprintf (fp, "  dir: %d\n", move -> dir);
			fprintf (fp, "  x: %d\n", move -> x);
			fprintf (fp, "  y: %d\n", move -> y);

			move = move -> next;
		}

		// Moves:
		fprintf (fp, " rank: %d\n", entity -> rank);

		entity = entity -> next;
	}
	fclose (fp);
}


void keyboard_callback (GLFWwindow* window, int key, int scancode, int action, int mods) {
	printf ("key = %d\n", key);
	//if (program -> time_stone == TRUE && program -> rewind_time >= 0.99) {
	if (program -> screen == LEVEL_SCREEN) {
	if (program -> rewind_time == 0.0) if (action > 0) {

		printf ("dev_menu_selected = %d\n", program -> dev_menu_selected);

		// Dev Menu Disabled:
		if (!(program -> dev_menu)) {
			printf ("ONE\n");
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
				case KEY_SPACE:
					step_level (program);
					break;
				case KEY_R:
					load_level (program, program -> level_id);
					break;
				case KEY_K:
					entity_add_step (program, program -> player, DOWN);
					break;
				case KEY_DEV:
					program -> dev_menu = !(program -> dev_menu);
					break;
				default:
					return;
			}
		}
		// Dev Menu:
		else if (!(program -> dev_menu_selected) && (program -> editor_menu_chapter == 0)) {
			printf ("TWO\n");
			switch (key) {
				case KEY_UP:
					if (program -> dev_menu < 4)
						program -> dev_menu ++;
					break;
				case KEY_DOWN:
					if (program -> dev_menu > 1)
						program -> dev_menu --;
					break;
				case KEY_SPACE:
					program -> dev_menu_selected = program -> dev_menu;
					if (program -> dev_menu_selected == 1) {
						program -> editor_menu_chapter = 1;
					}
					break;
				case KEY_ENTER:
					program -> dev_menu_selected = program -> dev_menu;
					if (program -> dev_menu_selected == 1) {
						program -> editor_menu_chapter = 1;
					}
					break;
				case KEY_DEV:
					program -> dev_menu = !(program -> dev_menu);
					break;
				case KEY_ESC:
					program -> dev_menu = 0;
					break;
				default:
					return;
			}
		}
		// Editor Chapter Menu:
		else if ((program -> dev_menu_selected == 1) && (program -> editor_menu_chapter != 0)) {
			printf ("THREE\n");
			switch (key) {
				case KEY_UP:
					if (program -> editor_menu_chapter < 4)
						program -> editor_menu_chapter ++;
					break;
				case KEY_DOWN:
					if (program -> editor_menu_chapter > 1)
						program -> editor_menu_chapter --;
					break;
				case KEY_SPACE:
					program -> editor_menu_chapter_selected = program -> editor_menu_chapter;
					break;
				case KEY_ENTER:
					program -> editor_menu_chapter_selected = program -> editor_menu_chapter;
					break;
				case KEY_ESC:
					program -> editor_menu_chapter = 0;
					program -> dev_menu_selected = 0;
					break;
				default:
					return;
			}
		}
		else if (program -> dev_menu_selected == 4) {
			printf ("FOUR\n");
			switch (key) {
				case KEY_ESC:
					program -> editor_menu_chapter = 0;
					program -> dev_menu_selected = 0;
					break;
				case KEY_ENTER:
					program -> editor_menu_chapter = 0;
					program -> dev_menu_selected = 0;
					save_level (program, program -> input_buffer);
					break;
				case KEY_DELETE:
					if (program -> input_buffer > 0)
						program -> input_buffer[program -> input_buffer_i - 1] = '\0';
					program -> input_buffer_i --;
					break;
				default:
					program -> input_buffer[program -> input_buffer_i ++] = key;
					program -> input_buffer[program -> input_buffer_i] = '\0';
					return;
			}
		}
		else {
			printf ("FIVE\n");
			switch (key) {
				case KEY_ESC:
					program -> dev_menu_selected = 0;
					break;
			}
		}
	}
	}
	else if (program -> screen == TITLE_SCREEN && action == 1) {
		switch (key) {
			case KEY_UP:
				if (program -> titleMenuOption > 1)
					program -> titleMenuOption --;
				break;
			case KEY_DOWN:
				if (program -> titleMenuOption < 3)
					program -> titleMenuOption ++;
				break;
			case KEY_ENTER:
				if (program -> titleMenuOption == NEW_GAME) {
					program -> screen = LEVEL_SCREEN;
					program -> chapter = FOREST;
				}
				if (program -> titleMenuOption == OPTIONS)
					program -> screen = OPTIONS_SCREEN;
				if (program -> titleMenuOption == QUIT)
					program -> running = FALSE;
				break;
		}
	}
	else if (program -> screen == OPTIONS_SCREEN && action == 1) {
		switch (key) {
			case KEY_UP:
				if (program -> optionsMenuOption > 0)
					program -> optionsMenuOption --;
				break;
			case KEY_DOWN:
				if (program -> optionsMenuOption < 2)
					program -> optionsMenuOption ++;
				break;
			case KEY_ENTER:
				switch (program -> optionsMenuOption) {
					case FULLSCREEN:
						program -> option_fullscreen = !(program -> option_fullscreen);
						break;
					case SOUND_FX:
						program -> option_sound_fx = !(program -> option_sound_fx);
						break;
					case MUSIC:
						program -> option_music = !(program -> option_music);
						break;
				}
				break;
			case KEY_ESC:
					program -> screen = TITLE_SCREEN;
				break;

		}
	}
	/*
	else if (program -> screen == TITLE_SCREEN && action == 1) {
		switch (key) {
			case KEY_UP:
				if (program -> titleMenuOption > 0)
					program -> titleMenuOption --;
				break;
			case KEY_DOWN:
				if (program -> titleMenuOption < 3)
					program -> titleMenuOption ++;
				break;
		}
	}*/

	/*
	if (key that moves player) {
		player_add_step ();
		step_level ();
	}
	*/
}

Entity* entityNew (Program* program, long type, int spawnX, int spawnY) {
	Entity* entity = malloc(sizeof(Entity));
	Entity* ghost = malloc(sizeof(Entity));
	entity -> id = program -> entity_id_last + 1;
	ghost -> id = program -> entity_id_last + 1;
	program -> entity_id_last ++;
	entity -> type = type;
	ghost -> type = type;
	entity -> moves = NULL;
	ghost -> moves = NULL;
	entity -> interactions = 0;
	ghost -> interactions = 0;
	entity -> step = 0;
	ghost -> step = 0;
	entity -> direction = DOWN;
	ghost -> direction = DOWN;
	entity -> next = NULL;
	ghost -> next = NULL;
	entity -> spawnX = spawnX;
	entity -> spawnY = spawnY;
	ghost -> spawnX = spawnX;
	ghost -> spawnY = spawnY;
	switch (type) {
		case PLAYER:
			entity -> rank = PLAYER_RANK;
			ghost -> rank = PLAYER_RANK;
			program -> player_ghost = ghost;
			break;
		case BLOCK:
			entity -> rank = BLOCK_RANK;
			ghost -> rank = BLOCK_RANK;
			break;
		case TREE:
			entity -> rank = TREE_RANK;
			ghost -> rank = TREE_RANK;
			break;
		case BARK:
			entity -> rank = BARK_RANK;
			ghost -> rank = BARK_RANK;
			break;
		case CRATE:
			entity -> rank = CRATE_RANK;
			ghost -> rank = CRATE_RANK;
			break;
		case TURTLE:
			entity -> rank = TURTLE_RANK;
			ghost -> rank = TURTLE_RANK;
			break;
		case APPLE:
			entity -> rank = APPLE_RANK;
			ghost -> rank = APPLE_RANK;
			break;
		case CAULDRON:
			entity -> rank = CAULDRON_RANK;
			ghost -> rank = CAULDRON_RANK;
			break;
		case CAR_LEFT:
			entity -> rank = CAR_RANK;
			ghost -> rank = CAR_RANK;
			break;
		case CAR_RIGHT:
			entity -> rank = CAR_RANK;
			ghost -> rank = CAR_RANK;
			break;
		case TIME_STONE:
			entity -> rank = TIME_STONE_RANK;
			ghost -> rank = TIME_STONE_RANK;
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
	if (level -> ghosts  == NULL)
		level -> ghosts = ghost;
	else {
		Entity* last = level -> ghosts;
		while (last -> next != NULL)
			last = last -> next;
		last -> next = ghost;
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
	// There is something in the way:
	else {
		// Is an ingredient or hand being pushed into cauldron?
		if (level -> cell[X + dX][Y + dY].entity -> type == CAULDRON) switch (entity -> type) {
			case APPLE:
				level -> cell[X][Y].entity = NULL;
				program -> ingredients += APPLE;
				return 1;
				break;
			case PLAYER:
				if (program -> mixed) {
					program -> screen = TITLE_SCREEN;
					program -> chapter = TITLE;
				}
				break;
		}

		// Is hand taking an item?
		if (level -> cell[X][Y].entity -> type = PLAYER) {
			if (level -> cell[X + dX][Y + dY].entity -> type == TIME_STONE) {
				level -> cell[X + dX][Y + dY].entity = entity;
				level -> cell[X][Y].entity = NULL;
				entity_pop_move (entity);
				program -> time_stone = TRUE;
				return 1;
			}
		}

		if (program -> pusher == NULL)
			program -> pusher = entity;
		// See if you can push it:
		if ((level -> cell[X + dX][Y + dY].entity -> rank) <= (program -> pusher -> rank)) {
			// If you succeed in pushing it, move in on it's spot.
			if (move_entity (program, X + dX, Y + dY, dir)) {
				level -> cell[X + dX][Y + dY].entity = entity;
				level -> cell[X][Y].entity = NULL;
				entity_pop_move (entity);
				return 1;
			}
			// If not, cancel move:
			else
				return 0;
		}
		// It's too heavy:
		else {
			return 0;
		}
	}

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
		program -> pusher = NULL;
		for (int j = 0; j < 15; j ++) {
			entity = program -> level -> cell [i][j].entity;
			if (entity != NULL) {
				if (entity -> moves != NULL) {
					Move* move = entity -> moves;
					switch (move -> type) {
						case STEP:
							switch (move -> dir) {
								case LEFT:
									if (entity -> step < program -> step) {
										program -> pusher = NULL;
										move_entity (program, i, j, move -> dir);
									}
									program -> playerDir = LEFT;
									break;
								case RIGHT:
									if (entity -> step < program -> step) {
										program -> pusher = NULL;
										move_entity (program, i, j, move -> dir);
									}
									program -> playerDir = RIGHT;
									break;
								case UP:
									if (entity -> step < program -> step) {
										program -> pusher = NULL;
										move_entity (program, i, j, move -> dir);
									}
									program -> playerDir = UP;
									break;
								case DOWN:
									if (entity -> step < program -> step) {
										program -> pusher = NULL;
										move_entity (program, i, j, move -> dir);
									}
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
	this -> ghosts = NULL;
	program -> ingredients = 0;
	program -> mixed = FALSE;
	program -> step = 0;
	program -> rewinded = FALSE;
	program -> rewind_time = 0.0;

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

					if (i < 4 || i > 10) {
						float dice = ((float)rand()) / RAND_MAX;
						if (i > 10 && j >= 5 && j <= 7);
						else {
						if (dice < 0.9) {
							this -> cell [i][j].entity = entityNew (program, TREE, i, j);
						}
						else {
							this -> cell [i][j].entity = entityNew (program, BARK, i, j);
						}
						}
					}
					if (
						((j == 12) && (i >= 5 && i <= 9)) ||
						((j == 8) && (i >= 5 && i <= 9 && i != 6)) ||
						((i == 5 || i == 9) && j < 12 && j > 8)
					) {
						if (!(i == 8 && j == 12))
							this -> cell [i][j].entity = entityNew (program, BLOCK, i, j);
					}
					if (i > 5 && i < 9 && j > 8 && j < 12)
						this -> cell [i][j].background_type = PLANKS;
				}
			}
			this -> cell [8][12].entity = entityNew (program, CRATE, 8, 12);
			this -> cell [4][9].entity = entityNew (program, BARK, 4, 9);


			this -> cell [7][4].entity = entityNew (program, PLAYER, 7, 4);
			program -> player = this -> cell [7][4].entity;
			// Turtle:
			/*
			this -> cell [2][2].occupant_type = TURTLE;
			this -> cell [2][2].entity = entityNew (program, TURTLE, 2, 2);
			this -> cell[2][2].entity -> direction = RIGHT;
			*/
			
			// Apple:
			this -> cell [10][9].occupant_type = APPLE;
			this -> cell [10][9].entity = entityNew (program, APPLE, 10, 9);
			
			// Cauldron:
			this -> cell [7][6].occupant_type = CAULDRON;
			this -> cell [7][6].entity = entityNew (program, CAULDRON, 7, 6);

			// Car:
			this -> cell [11][6].occupant_type = CAR_LEFT;
			this -> cell [11][6].entity = entityNew (program, CAR_LEFT, 11, 6);
			this -> cell [12][6].occupant_type = CAR_RIGHT;
			this -> cell [12][6].entity = entityNew (program, CAR_RIGHT, 12, 6);

			break;

		case FOREST_2:
			for (int i = 0; i < 15; i ++) {
				for (int j = 0; j < 15; j ++) {
					this -> cell [i][j].occupant_type = EMPTY;
					this -> cell [i][j].background_type = GRASS;

					if (pow(i - 7, 2)+ pow(j - 7, 2) > 32) {
						float dice = ((float)rand()) / RAND_MAX;
						if (dice < 0.9) {
							this -> cell [i][j].entity = entityNew (program, TREE, i, j);
						}
						else {
							this -> cell [i][j].entity = entityNew (program, BARK, i, j);
						}
					}
				}
			}

			this -> cell [7][4].entity = entityNew (program, PLAYER, 7, 4);
			program -> player = this -> cell [7][4].entity;
			
			// Apple:
			this -> cell [10][9].occupant_type = APPLE;
			this -> cell [10][9].entity = entityNew (program, APPLE, 10, 9);
			
			// Cauldron:
			this -> cell [7][6].occupant_type = CAULDRON;
			this -> cell [7][6].entity = entityNew (program, CAULDRON, 7, 6);

			this -> cell [7][11].entity = entityNew (program, TIME_STONE, 7, 11);

			break;

	}
}


void tileDraw (Program* program, int x, int y, long tile_type, char background) {
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
		case BARK:
			tex = program -> barkTex;
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
		case TIME_STONE:
			tex = program -> timeStoneTex;
			break;
		case PLANKS:
			tex = program -> planksTex;
			break;
		case APPLE:
			tex = program -> appleTex;
			break;
		case CAULDRON:
			tex = program -> cauldronTex;
			break;
		case CAR_LEFT:
			tex = program -> carLeftTex;
			break;
		case CAR_RIGHT:
			tex = program -> carRightTex;
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

	Level* this = NULL;
	switch (level) {
		case FOREST_1:
			/*
			drawTextCentered (program, "Movement: arrow keys", 936, 320, 1.0, textColor, 1.0);
			drawTextCentered (program, "Wait: SPACE", 936, 280, 1.0, textColor, 1.0);
			drawTextCentered (program, "Restart: R", 936, 240, 1.0, textColor, 1.0);
			drawTextCentered (program, "Time moves when you move.", 936, 200, 1.0, textColor, 1.0);
			drawTextCentered (program, "Some object are light enough to move.", 916, 160, 1.0, textColor, 1.0);
			drawTextCentered (program, "Cook a spell using apple.", 926, 120, 1.0, textColor, 1.0);
			drawTextCentered (program, "Enter cauldron to consume spell.", 926, 80, 1.0, textColor, 1.0);
			*/
			this = program -> level;
			break;
	}
	for (int i = -7; i <= 7; i ++) {
		for (int j = -7; j <= 7; j ++) {
			draw_cell (program, &(this -> cell [i + 7][j + 7]), i, j);
		}
	}
}
