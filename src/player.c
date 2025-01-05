#include "raylib.h"
#include "raymath.h"

#include "player.h"
#include "level.h"

Vector2 GetDesiredVeloctiy()
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

Vector2 GetNewPlayerPos(Player *player)
{
	Vector2 desiredVelocity = GetDesiredVeloctiy();
	float frameTime = GetFrameTime();
	if (Vector2Equals(player->velocity, desiredVelocity))
	{
	}
	else if (Vector2DistanceSqr(player->velocity, desiredVelocity) < 0.01)
		player->velocity = desiredVelocity;
	else
	{
		float lerpAmount = Clamp(Vector2LengthSqr(desiredVelocity) == 0
									 ? RETARDATION_SPEED * frameTime
									 : ACCELERATION_SPEED * frameTime,
								 0, 1);
		player->velocity = Vector2Lerp(player->velocity,
									   desiredVelocity,
									   lerpAmount);
	}
	return Vector2Add(player->pos, Vector2Scale(player->velocity, player->speed * frameTime));
}

void TickPlayer(Player *player, Level *level)
{
	Vector2 newPos = GetNewPlayerPos(player);
	// stop if hitting object
	for (int i = 0; i < level->treeCount; ++i)
	{
		if (CheckCollisionCircles(newPos, player->size, level->trees[i], TREE_COLLISION_RADIUS))
		{
			Vector2 delta = Vector2Subtract(newPos, level->trees[i]); // pointing from tree to player
			Vector2 direction = Vector2Normalize(delta);
			float distance = Vector2Length(delta);
			float overlap = player->size + TREE_COLLISION_RADIUS - distance;
			newPos = Vector2Add(newPos, Vector2Scale(direction, overlap));
		}
	}
	player->pos = newPos;
}

void DrawPlayer(Player *player)
{
	DrawCircleV(player->pos, player->size, PLAYER_COLOR);
}