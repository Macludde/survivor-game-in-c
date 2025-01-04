#include "raylib.h"
#include "raymath.h"
#include "camera.h"

void MoveCameraToPlayer(Camera2D *camera, Vector2 playerPos)
{
	camera->target = Vector2Lerp(camera->target, playerPos, CAMERA_MOVEMENT_SPEED * GetFrameTime());
}

void ZoomCamera(Camera2D *camera)
{
	camera->zoom += ((float)GetMouseWheelMove() * 0.05f);
	if (camera->zoom > 3.0f)
		camera->zoom = 3.0f;
	else if (camera->zoom < 0.1f)
		camera->zoom = 0.1f;
}

void TickCamera(Camera2D *camera, Vector2 playerPos)
{
	MoveCameraToPlayer(camera, playerPos);
	ZoomCamera(camera);
}