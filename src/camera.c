#include "camera.h"

#include <cglm/util.h>

#include "util.h"

Camera* camera_create(Window* window, vec3s position, vec3s up, float yaw, float pitch)
{
	Camera* camera = xmalloc(sizeof(*camera));

	camera->window = window;
	camera->position = position;
	camera->world_up = up;
	camera->yaw = yaw;
	camera->pitch = pitch;

	camera->movement_speed = 2.5f;
	camera->mouse_sensitivity = 0.03f;
	camera->zoom = 45.0f;

	camera_update_vectors(camera);

	return camera;
}

void camera_destroy(Camera* camera)
{
	free(camera);
}

void camera_update_vectors(Camera* camera)
{
	camera->front.x = cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
	camera->front.y = sin(glm_rad(camera->pitch));
	camera->front.z = sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
	camera->front = glms_vec3_normalize(camera->front);
	// also re-calculate the Right and Up vector
	camera->right = glms_vec3_normalize(glms_vec3_cross(camera->front, camera->world_up));
	camera->up = glms_vec3_normalize(glms_vec3_cross(camera->right, camera->front));
}

mat4s camera_get_view_matrix(Camera* camera)
{
	return glms_lookat(camera->position, glms_vec3_add(camera->position, camera->front), camera->up);
}

void camera_process_keyboard(Camera* camera, CameraMovement direction, float delta_time)
{
	float velocity = camera->movement_speed * delta_time;

	switch (direction) {
	case FORWARD:
		camera->position = glms_vec3_add(camera->position, glms_vec3_scale(camera->front, velocity));
		break;
	case BACKWARD:
		camera->position = glms_vec3_sub(camera->position, glms_vec3_scale(camera->front, velocity));
		break;
	case LEFT:
		camera->position = glms_vec3_sub(camera->position, glms_vec3_scale(camera->right, velocity));
		break;
	case RIGHT:
		camera->position = glms_vec3_add(camera->position, glms_vec3_scale(camera->right, velocity));
		break;
	}
}

void camera_process_mouse_movement(Camera* camera, float xoffset, float yoffset)
{
	xoffset *= camera->mouse_sensitivity;
	yoffset *= camera->mouse_sensitivity;

	camera->yaw += xoffset;
	camera->pitch += yoffset;

	// constrain pitch
	if (camera->pitch > 89.0f) {
		camera->pitch = 89.0f;
	} else if (camera->pitch < -89.0f) {
		camera->pitch = -89.0f;
	}

	camera_update_vectors(camera);
}
