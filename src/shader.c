#include "shader.h"

#include "util.h"
#include "filereader.h"

Shader* shader_create(const char* vertex_path, const char* fragment_path)
{
	Shader* shader = xmalloc(sizeof(*shader));

	const char* vertex_code = read_text_file(vertex_path);
	const char* fragment_code = read_text_file(fragment_path);

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_code, NULL);
	glCompileShader(vertex_shader);
	check_compile_errors(vertex_shader, VERTEX);

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_code, NULL);
	glCompileShader(fragment_shader);
	check_compile_errors(fragment_shader, FRAGMENT);

	shader->id = glCreateProgram();
	glAttachShader(shader->id, vertex_shader);
	glAttachShader(shader->id, fragment_shader);
	glLinkProgram(shader->id);
	check_compile_errors(shader->id, PROGRAM);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return shader;
}

void shader_destroy(Shader* shader)
{
	glDeleteProgram(shader->id);
	free(shader);
}

void check_compile_errors(GLuint shader, ShaderType type)
{
	GLint success;
	GLchar info_log[1024];

	if (type != PROGRAM) {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 1024, NULL, info_log);
			fprintf(stderr, "Shader compilation error of type: %d\n%s\n", type, info_log);
		}
	} else {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, 1024, NULL, info_log);
			fprintf(stderr, "Shader compilation error of type: %d\n%s\n", type, info_log);
		}
	}
}

void shader_use(Shader* shader)
{
	glUseProgram(shader->id);
}

void shader_set_int(Shader* shader, const char* name, GLint value)
{
	glUniform1i(glGetUniformLocation(shader->id, name), value);
}

void shader_set_float(Shader* shader, const char* name, GLfloat value)
{
	glUniform1f(glGetUniformLocation(shader->id, name), value);
}

void shader_set_vec3(Shader* shader, const char* name, vec3s* value)
{
	glUniform3fv(glGetUniformLocation(shader->id, name), 1, &value[0]);
}

void shader_set_vec4(Shader* shader, const char* name, vec4s* value)
{
	glUniform4fv(glGetUniformLocation(shader->id, name), 1, &value[0]);
}

void shader_set_mat4(Shader* shader, const char* name, mat4s* value)
{
	glUniformMatrix4fv(glGetUniformLocation(shader->id, name), 1, GL_FALSE, value->raw);
}
