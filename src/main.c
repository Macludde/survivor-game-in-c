#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"
#include "raymath.h"

#include "resource_dir.h" // utility header for SearchAndSetResourceDir
#include "level.h"
#include "player.h"
#include "camera.h"
#include "enemy.h"

#define DEFAULT_SCREEN_WIDTH 1280
#define DEFAULT_SCREEN_HEIGHT 800
#define GAME_NAME "SurvivorGame"

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

void HandleScreenResizing(Camera2D *camera)
{
	static int screenWidth;
	static int screenHeight;
	if (IsKeyPressed(KEY_F11))
		ToggleFullscreen();
	if (GetScreenWidth() != screenWidth || GetScreenHeight() != screenHeight)
	{
		screenWidth = GetScreenWidth();
		screenHeight = GetScreenHeight();
		camera->offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
	}
}

void SetupWindow()
{
	unsigned int CONFIG_FLAGS = FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN;
	SetConfigFlags(CONFIG_FLAGS & ~FLAG_VSYNC_HINT); // disable vsync for now
	SetTargetFPS(targetFps);
	// Create the window and OpenGL contextREEN
	InitWindow(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, GAME_NAME);
}

int main()
{
	SetupWindow();
	Player player = INITIAL_PLAYER;
	Camera2D camera = {
		.offset = {DEFAULT_SCREEN_WIDTH / 2.0f, DEFAULT_SCREEN_HEIGHT / 2.0f},
		.target = player.pos,
		.rotation = 0.0f,
		.zoom = 1.0f,
	};
	Level level = {
		.width = 1000,
		.height = 1000,
		.treeCount = 20,
	};
	// Level *level = AllocateLevel(1000, 1000, 20); // I chose to keep the level on the stack for now
	InitializeLevel(&level);

	EnemySpawner enemySpawner;
	InitializeEnemySpawner(&enemySpawner);

	// game loop
	while (!WindowShouldClose()) // run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
		HandleScreenResizing(&camera);
		HandleDebuggingKeys();

		TickPlayer(&player);
		TickCamera(&camera, player.pos);
		TickEnemySpawner(&enemySpawner, &camera, &level, &player);
		if (IsKeyPressed(KEY_F1))
			RemoveAllEnemies(&enemySpawner);

		// drawing
		BeginDrawing();
		ClearBackground(WHITE);

		BeginMode2D(camera);
		// draw world
		DrawLevel(&level);
		DrawEnemies(&enemySpawner);
		DrawPlayer(&player);

		EndMode2D();

		// draw UI

		DrawFPS(10, 10);
		DrawText(TextFormat("TARGET: %i", targetFps), 10, 40, 20, DARKGRAY);

		// end the frame and get ready for the next one
		EndDrawing();
	}

	free(level.trees);
	free(enemySpawner.enemies);

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
