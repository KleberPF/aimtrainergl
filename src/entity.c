#include "entity.h"

#include <string.h>
#include <assert.h>
#include <float.h>

#include "util.h"

Entity* entity_create(float* vertices, size_t vertices_count, GLuint* indices, GLuint indices_count, vec3s pos, vec4s color, EntityType type)
{
	Entity* entity = xmalloc(sizeof(*entity));
	entity->obj = globject_create(vertices, vertices_count, indices, indices_count);

	entity->shader = shader_create("./resources/shaders/shader.vert", "./resources/shaders/shader.frag");
	entity->pos = pos;
	entity->color = color;
	entity->has_aabb = type == TARGET;

	if (entity->has_aabb) {
		entity->aabb[0] = glms_vec3_sub(pos, (vec3s) { 0.5f, 0.5f, 0.5f });
		entity->aabb[1] = glms_vec3_add(pos, (vec3s) { 0.5f, 0.5f, 0.5f });
		entity->on_update = &target_on_update;
	} else {
		entity->on_update = &default_on_update;
	}

	return entity;
}

void entity_destroy(Entity* entity)
{
	shader_destroy(entity->shader);
	globject_destroy(entity->obj);
	free(entity);
}

bool is_point_in_plane_section(vec3s point, vec3s aabb[2])
{
	return (
		point.x <= aabb[1].x &&
		point.x >= aabb[0].x &&
		point.y <= aabb[1].y &&
		point.y >= aabb[0].y &&
		point.z <= aabb[1].z &&
		point.z >= aabb[0].z
		);
}

void default_on_update(Entity* entity, bool hit)
{
	// do nothing (at least for now)
}

void target_on_update(Entity* entity, bool hit)
{
	if (hit) {
		vec3s new_coord = {0};
		new_coord.x = (((float)rand() / RAND_MAX) * 2 - 1) * 10; // [-10, 10]
		new_coord.y = ((float)rand() / RAND_MAX) * 8 + 2; // [2, 10]
		new_coord.z = -15.0f;
		entity_move(entity, new_coord);
	}
}

void entity_move(Entity* entity, vec3s new_pos)
{
	entity->pos = new_pos;
	entity->aabb[0] = glms_vec3_sub(new_pos, (vec3s) { 0.5f, 0.5f, 0.5f });
	entity->aabb[1] = glms_vec3_add(new_pos, (vec3s) { 0.5f, 0.5f, 0.5f });
}

// TODO: work with scaling and translations when implemented in the entity
vec3s entity_get_line_aabb_intersection(Entity* entity, vec3s eye_pos, vec3s eye_dir, bool* intersects)
{
	vec3s closest_intersection = {0};
	float closest_dist = FLT_MAX;
	*intersects = false;

	if (!entity->has_aabb) {
		return closest_intersection;
	}

	for (size_t i = 0; i < 2; i++) {
		for (size_t j = 0; j < 3; j++) {
			float t = (entity->aabb[i].raw[j] - eye_pos.raw[j]) / eye_dir.raw[j];
			if (t < 0) {
				continue;
			}

			vec3s intersection = glms_vec3_add(eye_pos, glms_vec3_scale(eye_dir, t));
			bool intersects_now = is_point_in_plane_section(intersection, entity->aabb);

			if (!intersects_now) {
				continue;
			} else {
				*intersects = true;
			}

			float dist = glms_vec3_distance(intersection, eye_pos);
			if (dist < closest_dist) {
				closest_dist = dist;
				closest_intersection = intersection;
			}
		}
	}

	return closest_intersection;
}

void entity_render(Entity* entity)
{
	// shader_use is done in scene render for now at least
	globject_render(entity->obj);
}
