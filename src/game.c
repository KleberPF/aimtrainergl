#include "game.h"

#include <time.h>

#include "util.h"
#include "objects.h"

Game* game_create()
{
	srand(time(0));
	Game* game = xmalloc(sizeof(*game));

	// window needs to be created first to init glfw stuff (can't use opengl functions before that)
	game->window = window_create(SCR_WIDTH, SCR_HEIGHT, "Aim Trainer GL", FULLSCREEN);

	// opengl stuff
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	game->camera = camera_create(game->window, (vec3s) { 0.0f, 1.5f, 1.0f }, (vec3s) { 0.0f, 1.0f, 0.0f }, -90.0f, 0.0f);
	game->text_manager = text_manager_create();

	game->entities = malloc(START_CAPACITY * sizeof(*(game->entities))); // start with START_CAPACITY entities max, increase in game_add_entity as needed
	game->entity_count = 0;
	game->entity_capacity = START_CAPACITY;

	game->sprites = malloc(START_CAPACITY * sizeof(*(game->sprites))); // start with START_CAPACITY entities max, increase in game_add_sprite as needed
	game->sprite_count = 0;
	game->sprite_capacity = START_CAPACITY;

	game->first_mouse = true;
	game->mouse_btns->prev = GLFW_RELEASE;
	game->mouse_btns->current = GLFW_RELEASE;
	game->last_x = game->window->width / 2.0f;
	game->last_y = game->window->height / 2.0f;
	game->delta_time = 0.0f;
	game->last_frame = 0.0f;

	game->shots_hit = 0;
	game->total_shots = 0;

	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			Entity* entity = entity_create(cube, sizeof(cube) / sizeof(*cube) / 3, NULL, 0, (vec3s) { 2 * i, 2 * j + 5.0f, -15.0f }, (vec4s) { 0.2f, 0.0f, 1.0f, 1.0f }, TARGET);
			game_add_entity(game, entity);
		}
	}

	Entity* entity = entity_create(plane, sizeof(plane) / sizeof(*plane) / 3, NULL, 0, (vec3s) { 0.0f, 0.0f, 0.0f }, (vec4s) { 0.2f, 0.2f, 0.2f, 1.0f }, STATIC);
	game_add_entity(game, entity);

	Sprite* crosshair = sprite_create(square, sizeof(square) / sizeof(*square) / 3, NULL, 0, (vec3s) { 0.0f, 0.0f, 1.0f });
	game_add_sprite(game, crosshair);

	return game;
}

void game_destroy(Game* game)
{
	for (size_t i = 0; i < game->entity_count; i++) {
		entity_destroy(game->entities[i]);
	}

	text_manager_destroy(game->text_manager);
	camera_destroy(game->camera);
	window_destroy(game->window);
	free(game->entities);
	free(game);
}

void game_process_timing(Game* game)
{
	float current_frame = (float)glfwGetTime();
	game->delta_time = current_frame - game->last_frame;
	game->last_frame = current_frame;
}

void game_process_input(Game* game)
{
	// mouse input
	double xpos_in;
	double ypos_in;
	glfwGetCursorPos(game->window->ptr, &xpos_in, &ypos_in);

	float xpos = (float)xpos_in;
	float ypos = (float)ypos_in;

	if (game->first_mouse) {
		game->last_x = xpos;
		game->last_y = ypos;
		game->first_mouse = false;
	}

	float xoffset = xpos - game->last_x;
	float yoffset = game->last_y - ypos; // reversed since y-coordinates go from bottom to top

	game->last_x = xpos;
	game->last_y = ypos;

	game->mouse_btns[0].current = glfwGetMouseButton(game->window->ptr, GLFW_MOUSE_BUTTON_LEFT);
	game->mouse_btns[1].current = glfwGetMouseButton(game->window->ptr, GLFW_MOUSE_BUTTON_RIGHT);

	// notify entities when hit
	game_update_entities(game);

	game->mouse_btns[0].prev = game->mouse_btns[0].current;
	game->mouse_btns[1].prev = game->mouse_btns[1].current;

	// camera mouse processing
	camera_process_mouse_movement(game->camera, (float)xoffset, (float)yoffset);

	// generic processing
	if (glfwGetKey(game->window->ptr, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(game->window->ptr, true);

	// camera keyboard processing
	// uncomment this to allow flying around
	//if (glfwGetKey(game->window->ptr, GLFW_KEY_W) == GLFW_PRESS)
	//    camera_process_keyboard(game->camera, FORWARD, game->delta_time);
	//if (glfwGetKey(game->window->ptr, GLFW_KEY_S) == GLFW_PRESS)
	//    camera_process_keyboard(game->camera, BACKWARD, game->delta_time);
	//if (glfwGetKey(game->window->ptr, GLFW_KEY_A) == GLFW_PRESS)
	//    camera_process_keyboard(game->camera, LEFT, game->delta_time);
	//if (glfwGetKey(game->window->ptr, GLFW_KEY_D) == GLFW_PRESS)
	//    camera_process_keyboard(game->camera, RIGHT, game->delta_time);
}

void game_main_loop(Game* game)
{
	while (!glfwWindowShouldClose(game->window->ptr)) {
		game_process_timing(game);
		game_process_input(game);
		game_render(game);
		game_update_glfw(game);
	}
}

void game_add_entity(Game* game, Entity* entity)
{
	if (game->entity_count == MAX_ENTITY_COUNT) {
		return;
	}

	if (game->entity_count == game->entity_capacity) {
		game->entity_capacity *= 2;
		void* ptr_aux = realloc(game->entities, game->entity_capacity * sizeof(*game->entities));
		if (ptr_aux == NULL) {
			return;
		}

		game->entities = ptr_aux;
	}
	game->entities[game->entity_count++] = entity;
}

void game_update_entities(Game* game)
{
	if (game->mouse_btns[0].current == GLFW_PRESS && game->mouse_btns[0].prev == GLFW_RELEASE) {
		Entity* closest_entity = NULL;
		float closest_dist = FLT_MAX;

		for (size_t i = 0; i < game->entity_count; i++) {
			Entity* entity = game->entities[i];
			bool intersects = false;
			vec3s intersection = entity_get_line_aabb_intersection(entity, game->camera->position, game->camera->front, &intersects);

			if (intersects) {
				float dist = glms_vec3_distance2(game->camera->position, intersection);
				if (dist < closest_dist) {
					closest_dist = dist;
					closest_entity = entity;
				}
			}
		}

		if (closest_entity != NULL) {
			closest_entity->on_update(closest_entity, true);
			game->shots_hit++;
		}

		game->total_shots++;
	}
}

void game_add_sprite(Game* game, Sprite* sprite)
{
	if (game->sprite_count == MAX_SPRITE_COUNT) {
		return;
	}

	if (game->sprite_count == game->sprite_capacity) {
		game->sprite_capacity *= 2;
		void* ptr_aux = realloc(game->sprites, game->sprite_capacity * sizeof(*game->sprites));
		if (ptr_aux == NULL) {
			return;
		}

		game->sprites = ptr_aux;
	}
	game->sprites[game->sprite_count++] = sprite;
}

void game_render(Game* game)
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (size_t i = 0; i < game->entity_count; i++) {
		Entity* entity = game->entities[i];
		shader_use(entity->shader);

		mat4s model = glms_translate(glms_mat4_identity(), entity->pos);
		shader_set_mat4(entity->shader, "model", &model);

		mat4s projection = glms_perspective(glm_rad(game->camera->zoom), (float)game->window->width / game->window->height, 0.1f, 100.0f);
		shader_set_mat4(entity->shader, "projection", &projection);

		mat4s view = camera_get_view_matrix(game->camera);
		shader_set_mat4(entity->shader, "view", &view);

		shader_set_vec4(entity->shader, "color", &entity->color);

		entity_render(entity);
	}

	// Draw ortho sprites (e.g. crosshair)
	for (size_t i = 0; i < game->sprite_count; i++) {
		Sprite* sprite = game->sprites[i];
		shader_use(sprite->shader);

		mat4s model = glms_scale(glms_mat4_identity(), (vec3s) { 0.015f, 0.015f, 1.0f });
		shader_set_mat4(sprite->shader, "model", &model);

		float aspect_ratio = (float)game->window->width / game->window->height;
		mat4s projection = glms_ortho(-aspect_ratio, aspect_ratio, -1.0f, 1.0f, 0.1f, 100.0f);
		shader_set_mat4(sprite->shader, "projection", &projection);

		mat4s view = glms_mat4_identity();
		shader_set_mat4(sprite->shader, "view", &view);

		vec4s color = {0.0f, 1.0f, 0.0f, 1.0f};
		shader_set_vec4(sprite->shader, "color", &color);

		sprite_render(sprite);
	}

	// Draw text (also ortho)
	shader_use(game->text_manager->shader);
	mat4s projection = glms_ortho(0.0f, game->window->width, 0.0f, game->window->height, 0.0f, 100.0f);
	shader_set_mat4(game->text_manager->shader, "projection", &projection);

	vec3s color = {0.5f, 0.8f, 0.2f};
	shader_set_vec3(game->text_manager->shader, "color", &color);

	char score_text[64];
	sprintf(score_text, "Shots hit: %d (%.2f%% accuracy)", game->shots_hit, game->total_shots > 0 ? (float)game->shots_hit * 100 / game->total_shots : 0);
	text_manager_render_text(game->text_manager, score_text, 25.0f, 70.0f, 1.0f);
	text_manager_render_text(game->text_manager, "Press ESC to quit", 25.0f, 25.0f, 1.0f);
}

void game_update_glfw(Game* game) // terrible name
{
	glfwSwapBuffers(game->window->ptr);
	glfwPollEvents();
}
