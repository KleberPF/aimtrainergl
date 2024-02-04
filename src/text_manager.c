#include "text_manager.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "util.h"

TextManager* text_manager_create()
{
	TextManager* manager = xmalloc(sizeof(*manager));
	manager->shader = shader_create("./resources/shaders/text.vert", "./resources/shaders/text.frag");

	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		exit(EXIT_FAILURE);
	}

	FT_Face face;
	if (FT_New_Face(ft, "./resources/fonts/Roboto-Regular.ttf", 0, &face)) {
		exit(EXIT_FAILURE);
	}

	FT_Set_Pixel_Sizes(face, 0, 48);

	for (unsigned char c = 0; c < 128; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			fprintf(stderr, "Error loading character %c\n", c);
			continue;
		}

		glGenTextures(1, &manager->caracters[c].texture_id);
		glBindTexture(GL_TEXTURE_2D, manager->caracters[c].texture_id);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		manager->caracters[c].size.x = face->glyph->bitmap.width;
		manager->caracters[c].size.y = face->glyph->bitmap.rows;
		manager->caracters[c].bearing.x = face->glyph->bitmap_left;
		manager->caracters[c].bearing.y = face->glyph->bitmap_top;
		manager->caracters[c].advance = face->glyph->advance.x;
	}

	// reseting/cleanup
	glBindTexture(GL_TEXTURE_2D, 0);
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &manager->vao);
	glGenBuffers(1, &manager->vbo);
	glBindVertexArray(manager->vao);
	glBindBuffer(GL_ARRAY_BUFFER, manager->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return manager;
}

void text_manager_destroy(TextManager* manager)
{
	shader_destroy(manager->shader);
	free(manager);
}

void text_manager_render_text(TextManager* manager, const char* text, float x, float y, float scale)
{
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(manager->vao);

	size_t len = strlen(text);
	for (size_t i = 0; i < len; i++) {
		Caracter ch = manager->caracters[text[i]];

		float xpos = x + ch.bearing.x * scale;
		float ypos = y - (ch.size.y - ch.bearing.y) * scale;

		float w = ch.size.x * scale;
		float h = ch.size.y * scale;

		// update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h, 0.0f, 0.0f },
			{ xpos,     ypos,     0.0f, 1.0f },
			{ xpos + w, ypos,     1.0f, 1.0f },

			{ xpos,     ypos + h, 0.0f, 0.0f },
			{ xpos + w, ypos,     1.0f, 1.0f },
			{ xpos + w, ypos + h, 1.0f, 0.0f }
		};

		glBindTexture(GL_TEXTURE_2D, ch.texture_id);
		glBindBuffer(GL_ARRAY_BUFFER, manager->vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += (ch.advance >> 6) * scale;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
