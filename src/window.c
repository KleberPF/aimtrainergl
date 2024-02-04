#include "window.h"

#include <glad/glad.h>

#include <string.h>

#include "util.h"

Window* window_create(int width, int height, const char* title, bool fullscreen)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWmonitor* monitor = fullscreen ? glfwGetPrimaryMonitor() : NULL;
	GLFWwindow* ptr = glfwCreateWindow(width, height, title, monitor, NULL);
	if (ptr == NULL) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(ptr);
	glfwSwapInterval(0); // turn off vsync
	glfwSetFramebufferSizeCallback(ptr, framebuffer_size_callback_);

	glfwSetInputMode(ptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// has to run after glfwCreateWindow
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		exit(EXIT_FAILURE);
	}

	Window* window = xmalloc(sizeof(*window));

	window->ptr = ptr;
	window->width = width;
	window->height = height;
	window->title = _strdup(title);
	window->fullscreen = monitor != NULL;

	return window;
}

void window_destroy(Window* window)
{
	free(window->title);
	glfwTerminate();
	free(window);
}

void framebuffer_size_callback_(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}