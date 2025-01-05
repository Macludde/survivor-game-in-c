#include "raylib.h"
#include "raymath.h"

#include "player.h"
#include "level.h"
#include "enemy.h"
#include "helpers.h"

extern Camera2D camera;
extern EnemySpawner enemySpawner;

Weapon InitialWeapon()
{
	return (Weapon){
		.damage = 10,
		.range = 800,
		.speed = 300,
		.fireRate = 1,
		.lastFired = 0,
		.bullets = calloc(DEFAULT_BULLET_CAPACITY, sizeof(Bullet)),
		.bulletCapacity = DEFAULT_BULLET_CAPACITY,
	};
}
Player InitialPlayer()
{
	return (Player){
		.speed = 200,
		.size = 20,
		.weapon = InitialWeapon(),
	};
}

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

void RemoveBullet(Bullet *bullet)
{
	bullet->spawned = false;
}

void BulletHitEnemy(Bullet *bullet, Weapon *weapon, Enemy *enemy)
{
	EnemyTakeDamage(enemy, weapon->damage);
	RemoveBullet(bullet);
}

void HandleBulletCollisions(Bullet *bullet, Weapon *weapon)
{
	Enemy *enemies = enemySpawner.enemies;
	for (int i = 0; i < enemySpawner.highestEnemyIndex; ++i)
	{
		if (enemies[i].spawned && CheckCollisionCircles(bullet->pos, bullet->size, enemies[i].pos, enemies[i].size))
		{
			BulletHitEnemy(bullet, weapon, &enemies[i]);
			return;
		}
	}
}

void HandleBulletOffScreen(Bullet *bullet)
{
	Vector2 topLeft = Vector2SubtractValue(GetScreenToWorld2D((Vector2){0, 0}, camera), bullet->size + 100);
	Vector2 bottomRight = Vector2AddValue(GetScreenToWorld2D((Vector2){GetScreenWidth(), GetScreenHeight()}, camera), bullet->size + 100);
	if (bullet->pos.x < topLeft.x || bullet->pos.x > bottomRight.x || bullet->pos.y < topLeft.y || bullet->pos.y > bottomRight.y)
		RemoveBullet(bullet);
}

Vector2 ClosestEnemy(Player *player, float maxRange)
{
	Enemy *enemies = enemySpawner.enemies;
	Vector2 closest = {INFINITY, INFINITY};
	float closestDistanceSqr = maxRange * maxRange;
	for (int i = 0; i < enemySpawner.highestEnemyIndex; ++i)
	{
		if (enemies[i].spawned)
		{
			float distanceSqr = Vector2DistanceSqr(player->pos, enemies[i].pos);
			if (distanceSqr < closestDistanceSqr)
			{
				closestDistanceSqr = distanceSqr;
				closest = enemies[i].pos;
			}
		}
	}

	return closest;
}

void SpawnBullet(Player *player)
{
	Weapon *weapon = &player->weapon;
	Vector2 closestEnemy = ClosestEnemy(player, weapon->range);
	if (closestEnemy.x == INFINITY)
		return;
	for (int i = 0; i < weapon->bulletCapacity; ++i)
	{
		if (!weapon->bullets[i].spawned)
		{
			weapon->bullets[i].spawned = true;
			weapon->bullets[i].pos = player->pos;
			weapon->bullets[i].velocity = Vector2Scale(Vector2Normalize(Vector2Subtract(closestEnemy, player->pos)), weapon->speed); // TODO: aim at enemy, for now random¨
			weapon->bullets[i].size = 10;
			return;
		}
	}
	weapon->bulletCapacity *= 2;
	weapon->bullets = realloc(weapon->bullets, weapon->bulletCapacity * sizeof(Bullet));
	if (weapon->bullets == NULL)
	{
		weapon->bulletCapacity = 0;
		exit(1); // OUT OF MEMORY
	}
	return SpawnBullet(player);
}

void TickWeapon(Player *player)
{
	Weapon *weapon = &player->weapon;
	if (time_in_seconds() - weapon->lastFired > (1 / weapon->fireRate))
	{
		weapon->lastFired = time_in_seconds();
		SpawnBullet(player);
	}
	for (int i = 0; i < weapon->bulletCapacity; ++i)
	{
		if (weapon->bullets[i].spawned)
		{
			weapon->bullets[i].pos = Vector2Add(weapon->bullets[i].pos, Vector2Scale(weapon->bullets[i].velocity, GetFrameTime()));
			HandleBulletCollisions(&weapon->bullets[i], weapon);
			HandleBulletOffScreen(&weapon->bullets[i]);
		}
	}
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
	TickWeapon(player);
}

void DrawBullet(Bullet *bullet)
{
	DrawCircleV(bullet->pos, bullet->size, BLUE);
}

void DrawPlayer(Player *player)
{
	DrawCircleV(player->pos, player->size, PLAYER_COLOR);
	for (int i = 0; i < player->weapon.bulletCapacity; ++i)
	{
		if (player->weapon.bullets[i].spawned)
			DrawBullet(&player->weapon.bullets[i]);
	}
}