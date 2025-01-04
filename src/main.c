/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include <stdio.h>
#include "raylib.h"
#include "raymath.h"

#include "resource_dir.h" // utility header for SearchAndSetResourceDir

#define DEFAULT_SCREEN_WIDTH 1280
#define DEFAULT_SCREEN_HEIGHT 800
#define GAME_NAME "SurvivorGame"

float playerSpeed = 600;
Vector2 currentPlayerMovementDelta;

Vector2 GetPlayerMovementDelta()
{
	Vector2 delta = Vector2Zero();
	if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
		delta.y -= 1;
	if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
		delta.y += 1;

	if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
		delta.x -= 1;
	if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
		delta.x += 1;
	// we cast float to int. Because length is either 0 (0,0), 1 (vertical or horizontal) or 2 (diagonal)
	int lengthSquared = Vector2LengthSqr(delta); // faster than length, no sqrt
	if (lengthSquared == 0)
		return delta;
	if (lengthSquared == 1)
		return delta;
	return Vector2Normalize(delta);
}

#define ACCELERATION_SPEED 5.0f
// stop faster, accelerate slower
#define RETARDATION_SPEED 10.0f
Vector2 GetNewPlayerPos(Vector2 playerPos)
{
	Vector2 desiredPlayerMovementDelta = GetPlayerMovementDelta();
	float frameTime = GetFrameTime();
	if (Vector2Equals(currentPlayerMovementDelta, desiredPlayerMovementDelta))
	{
	}
	else if (Vector2DistanceSqr(currentPlayerMovementDelta, desiredPlayerMovementDelta) < 0.01)
		currentPlayerMovementDelta = desiredPlayerMovementDelta;
	else
	{
		float lerpAmount = Clamp(Vector2LengthSqr(desiredPlayerMovementDelta) == 0
									 ? RETARDATION_SPEED * frameTime
									 : ACCELERATION_SPEED * frameTime,
								 0, 1);
		currentPlayerMovementDelta = Vector2Lerp(currentPlayerMovementDelta,
												 desiredPlayerMovementDelta,
												 lerpAmount);
	}
	return Vector2Add(playerPos, Vector2Scale(currentPlayerMovementDelta, playerSpeed * frameTime));
}

int targetFps = 60;
void HandleDebuggingKeys()
{

	if (IsKeyPressed(KEY_F10))
	{
		if (targetFps == 60)
			targetFps = 40;
		else if (targetFps == 40)
			targetFps = 20;
		else if (targetFps == 20)
			targetFps = 0;
		else
			targetFps = 60;
		SetTargetFPS(targetFps);
	}
}

unsigned int CONFIG_FLAGS = FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN;
int main()
{
	SetConfigFlags(CONFIG_FLAGS & ~FLAG_VSYNC_HINT); // disable vsync for now
	SetTargetFPS(targetFps);
	// Create the window and OpenGL context
	InitWindow(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, GAME_NAME);

	Vector2 playerPos;

	// logic
	int screenWidth = GetScreenWidth();
	int screenHeight = GetScreenHeight();
	playerPos.x = screenWidth / 2;
	playerPos.y = screenHeight / 2;

	// game loop
	while (!WindowShouldClose()) // run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
		if (GetScreenWidth() != screenWidth || GetScreenHeight() != screenHeight)
		{
			screenWidth = GetScreenWidth();
			screenHeight = GetScreenHeight();
			playerPos.x = screenWidth / 2;
			playerPos.y = screenHeight / 2;
		}
		HandleDebuggingKeys();

		playerPos = GetNewPlayerPos(playerPos);

		// drawing
		BeginDrawing();
		DrawText(TextFormat("FPS: %i, TARGET: %i", GetFPS(), targetFps), 10, 10, 20, DARKGRAY);

		ClearBackground(WHITE);
		DrawCircle(playerPos.x, playerPos.y, 20, DARKGREEN);

		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
