#pragma once

#include <cglm/struct.h>

#include "shader.h"
#include "globject.h"

// this struct is for sprites that are going to be rendered with an ortho projection
// useful for crosshairs, HUD, etc
typedef struct sprite {
	GLObject* obj;
	vec3s pos;
	Shader* shader;
} Sprite;

Sprite* sprite_create(float* vertices, size_t vertices_count, GLuint* indices, GLuint indices_count, vec3s pos);
void sprite_destroy(Sprite* sprite);

void sprite_render(Sprite* sprite);