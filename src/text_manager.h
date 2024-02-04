#pragma once

#include <glad/glad.h>
#include <cglm/struct.h>

#include "shader.h"

typedef struct caracter {
	GLuint texture_id;
	ivec2s size;
	ivec2s bearing;
	unsigned int advance;
} Caracter;

typedef struct TextManager_ {
	Caracter caracters[128];
	Shader* shader;
	GLuint vao;
	GLuint vbo; // we reuse the same vbo for every character drawn
} TextManager;

TextManager* text_manager_create();
void text_manager_destroy(TextManager* manager);

void text_manager_render_text(TextManager* manager, const char* text, float x, float y, float scale);