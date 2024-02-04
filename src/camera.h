#pragma once

#include <cglm/struct.h>

#include "window.h"

typedef enum CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
} CameraMovement;

typedef struct camera {
	Window* window;

	vec3s position;
	vec3s front;
	vec3s up;
	vec3s right;
	vec3s world_up;

	// euler angles
	float yaw;
	float pitch;

	// options
	float movement_speed;
	float mouse_sensitivity;
	float zoom;
} Camera;

Camera* camera_create(Window* window, vec3s position, vec3s up, float yaw, float pitch);
void camera_destroy(Camera* camera);

void camera_update_vectors(Camera* camera);
mat4s camera_get_view_matrix(Camera* camera);
void camera_process_keyboard(Camera* camera, CameraMovement direction, float delta_time);
void camera_process_mouse_movement(Camera* camera, float xoffset, float yoffset);
