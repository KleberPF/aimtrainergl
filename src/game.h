#pragma once

#include <GLFW/glfw3.h>

#include "entity.h"
#include "camera.h"
#include "window.h"
#include "sprite.h"
#include "text_manager.h"

// settings
#define SCR_WIDTH 1920
#define SCR_HEIGHT 1080
#define FULLSCREEN true

#define START_CAPACITY 4
#define MAX_ENTITY_COUNT 4096
#define MAX_SPRITE_COUNT 16

typedef struct mouse_button {
	int prev;
	int current;
} MouseButton;

typedef struct game {
	Window* window;
	Camera* camera;
	TextManager* text_manager;
	Entity** entities; // will maybe turn this into a linked list in the future to have fast insert/remove
	size_t entity_count;
	size_t entity_capacity;
	Sprite** sprites; // 2d elements rendered on top of everything
	size_t sprite_count;
	size_t sprite_capacity;

	// mouse input
	bool first_mouse;
	MouseButton mouse_btns[2]; // store either GLFW_RELEASE or GLFW_PRESS
	float last_x;
	float last_y;

	// timing
	float delta_time;
	float last_frame;

	// objective related stuff
	int shots_hit;
	int total_shots;
} Game;

Game* game_create();
void game_destroy(Game* game);

void game_process_timing(Game* game);
void game_process_input(Game* game);
void game_main_loop(Game* game);
void game_add_entity(Game* game, Entity* entity);
void game_update_entities(Game* game);
void game_add_sprite(Game* game, Sprite* sprite);
void game_render(Game* game);
void game_update_glfw(Game* game);