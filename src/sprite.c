#include "sprite.h"

#include "util.h"

Sprite* sprite_create(float* vertices, size_t vertices_count, GLuint* indices, GLuint indices_count, vec3s pos)
{
	Sprite* sprite = xmalloc(sizeof(*sprite));
	sprite->obj = globject_create(vertices, vertices_count, indices, indices_count);

	sprite->shader = shader_create("./resources/shaders/shader.vert", "./resources/shaders/shader.frag");
	sprite->pos = pos;

	return sprite;
}

void sprite_destroy(Sprite* sprite)
{
	shader_destroy(sprite->shader);
	globject_destroy(sprite->obj);
	free(sprite);
}

void sprite_render(Sprite* sprite)
{
	// shader_use is done in scene render for now at least
	globject_render(sprite->obj);
}
