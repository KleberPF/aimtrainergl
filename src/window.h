#pragma once

#include <stdbool.h>

#include <GLFW/glfw3.h>

typedef struct window {
	GLFWwindow* ptr;
	char* title;
	int width;
	int height;
	bool fullscreen;
} Window;

Window* window_create(int width, int height, const char* title, bool fullscreen);
void window_destroy(Window* window);

void framebuffer_size_callback_(GLFWwindow* window, int width, int height);