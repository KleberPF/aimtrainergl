#include "globject.h"

#include "util.h"

GLObject* globject_create(float* vertices, size_t vertices_count, GLuint* indices, GLuint indices_count)
{
	GLObject* globject = xmalloc(sizeof(*globject));

	globject->vertices = vertices;
	globject->vertices_count = vertices_count;
	globject->indices = indices;
	globject->indices_count = indices_count;

	glGenVertexArrays(1, &globject->vao);
	glGenBuffers(1, &globject->vbo);
	glGenBuffers(1, &globject->ebo);
	glBindVertexArray(globject->vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, globject->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*indices) * indices_count, indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, globject->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(*vertices) * vertices_count * 3, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(*vertices) * 3, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return globject;
}

void globject_destroy(GLObject* globject)
{
	glDeleteVertexArrays(1, &globject->vao);
	glDeleteBuffers(1, &globject->vbo);
	glDeleteBuffers(1, &globject->ebo);

	free(globject);
}

void globject_render(GLObject* globject)
{
	// shader_use is done in game render for now at least
	glBindVertexArray(globject->vao);
	if (globject->indices_count > 0) {
		glDrawElements(GL_TRIANGLES, globject->indices_count, GL_UNSIGNED_INT, 0);
	} else {
		glDrawArrays(GL_TRIANGLES, 0, globject->vertices_count);
	}
}
