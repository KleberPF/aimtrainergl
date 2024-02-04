#pragma once

#include <glad/glad.h>
#include <cglm/struct.h>

typedef enum ShaderType_ {
	VERTEX,
	FRAGMENT,
	PROGRAM
} ShaderType;

typedef struct shader {
	GLuint id;
} Shader;

Shader* shader_create(const char* vertex_path, const char* fragment_path);
void shader_destroy(Shader* shader);

static void check_compile_errors(GLuint shader, ShaderType type);

void shader_use(Shader* shader);
void shader_set_int(Shader* shader, const char* name, GLint value);
void shader_set_float(Shader* shader, const char* name, GLfloat value);

void shader_set_vec3(Shader* shader, const char* name, vec3s* value);
void shader_set_vec4(Shader* shader, const char* name, vec4s* value);
void shader_set_mat4(Shader* shader, const char* name, mat4s* value);