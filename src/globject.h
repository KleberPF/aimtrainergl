#pragma once

#include <glad/glad.h>

typedef struct globject {
	float* vertices;
	size_t vertices_count;
	GLuint* indices;
	GLuint indices_count;
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
} GLObject;

GLObject* globject_create(float* vertices, size_t vertices_count, GLuint* indices, GLuint indices_count);
void globject_destroy(GLObject* globject);

void globject_render(GLObject* globject);