#pragma once

#include <glad/glad.h>
#include <cglm/struct.h>

#include "shader.h"
#include "globject.h"

typedef enum entity_type {
	TARGET,
	STATIC,
} EntityType;

typedef struct entity {
	Shader* shader;
	GLObject* obj;
	bool has_aabb;
	vec3s aabb[2];
	vec3s pos;
	vec4s color;
	void (*on_update)(void*, bool);
} Entity;

Entity* entity_create(float* vertices, size_t vertices_count, GLuint* indices, GLuint indices_count, vec3s pos, vec4s color, EntityType type);
void entity_destroy(Entity* entity);

bool is_point_in_plane_section(vec3s point, vec3s aabb[2]);
void default_on_update(Entity* entity, bool hit);
void target_on_update(Entity* entity, bool hit);

void entity_move(Entity* entity, vec3s new_pos);
vec3s entity_get_line_aabb_intersection(Entity* entity, vec3s eye_pos, vec3s eye_dir, bool* intersects);
void entity_render(Entity* entity);
