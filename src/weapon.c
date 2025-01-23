#include "weapon.h"

#include "enemy.h"
#include "helpers.h"
#include "level.h"
#include "player.h"
#include "raylib.h"
#include "raymath.h"

extern Camera2D camera;
extern EnemySpawner enemySpawner;
extern Level level;

Weapon InitialWeapon() {
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

void RemoveBullet(Bullet *bullet) { bullet->spawned = false; }

void BulletHitEnemy(Bullet *bullet, Weapon *weapon, Enemy *enemy) {
  RigidCollision(&bullet->body, &enemy->body);
  EnemyTakeDamage(enemy, weapon->damage);
  RemoveBullet(bullet);
}

bool BulletEnemyCollisions(Bullet *bullet, Weapon *weapon) {
  Enemy *enemies = enemySpawner.enemies;
  for (int i = 0; i < enemySpawner.highestEnemyIndex; ++i) {
    if (enemies[i].spawned && CheckCollision(bullet->body, enemies[i].body)) {
      BulletHitEnemy(bullet, weapon, &enemies[i]);
      return true;
    }
  }
}

bool BulletTreeCollisions(Bullet *bullet) {
  for (int i = 0; i < level.treeCount; ++i) {
    if (CheckCollision(bullet->body, GetTreeBody(level.trees[i]))) {
      RemoveBullet(bullet);
      return true;
    }
  }
  return false;
}

// Will return true if bullet hit something
bool HandleBulletCollisions(Bullet *bullet, Weapon *weapon) {
  // will call one after another until one returns true
  return BulletEnemyCollisions(bullet, weapon) || BulletTreeCollisions(bullet);
}

bool HandleBulletOffScreen(Bullet *bullet) {
  Vector2 topLeft = Vector2SubtractValue(
      GetScreenToWorld2D((Vector2){0, 0}, camera), bullet->body.radius + 100);
  Vector2 bottomRight = Vector2AddValue(
      GetScreenToWorld2D((Vector2){GetScreenWidth(), GetScreenHeight()},
                         camera),
      bullet->body.radius + 100);
  if (bullet->body.pos.x < topLeft.x || bullet->body.pos.x > bottomRight.x ||
      bullet->body.pos.y < topLeft.y || bullet->body.pos.y > bottomRight.y) {
    RemoveBullet(bullet);
    return true;
  }
  return false;
}

Vector2 ClosestEnemy(Vector2 pos, float maxRange) {
  Enemy *enemies = enemySpawner.enemies;
  Vector2 closest = {INFINITY, INFINITY};
  float closestDistanceSqr = maxRange * maxRange;
  for (int i = 0; i < enemySpawner.highestEnemyIndex; ++i) {
    if (enemies[i].spawned) {
      float distanceSqr = Vector2DistanceSqr(pos, enemies[i].body.pos);
      if (distanceSqr < closestDistanceSqr) {
        closestDistanceSqr = distanceSqr;
        closest = enemies[i].body.pos;
      }
    }
  }

  return closest;
}

bool SpawnBullet(Weapon *weapon, Vector2 pos) {
  Vector2 closestEnemy = ClosestEnemy(pos, weapon->range);
  if (closestEnemy.x == INFINITY) return;
  for (int i = 0; i < weapon->bulletCapacity; ++i) {
    if (!weapon->bullets[i].spawned) {
      weapon->bullets[i].spawned = true;
      weapon->bullets[i].body.pos = pos;
      weapon->bullets[i].body.velocity = Vector2Add(
          // player->body.velocity,
          Vector2Zero(),
          Vector2Scale(Vector2Normalize(Vector2Subtract(closestEnemy, pos)),
                       weapon->speed));  // TODO: aim at enemy, for now random¨
      weapon->bullets[i].body.radius = 5;
      weapon->bullets[i].body.mass = 25;
      return true;
    }
  }
  weapon->bulletCapacity *= 2;
  weapon->bullets =
      realloc(weapon->bullets, weapon->bulletCapacity * sizeof(Bullet));
  if (weapon->bullets == NULL) {
    weapon->bulletCapacity = 0;
    exit(1);  // OUT OF MEMORY
  }
  return SpawnBullet(weapon, pos);
}

void TickWeapon(Weapon *weapon, Vector2 playerPos) {
  if (time_in_seconds() - weapon->lastFired > (1 / weapon->fireRate)) {
    SpawnBullet(weapon, playerPos);
    weapon->lastFired = time_in_seconds();
  }
  for (int i = 0; i < weapon->bulletCapacity; ++i) {
    if (weapon->bullets[i].spawned) {
      weapon->bullets[i].body.pos = Vector2Add(
          weapon->bullets[i].body.pos,
          Vector2Scale(weapon->bullets[i].body.velocity, GetFrameTime()));
      if (HandleBulletCollisions(&weapon->bullets[i], weapon) ||
          HandleBulletOffScreen(&weapon->bullets[i])) {
        // bullet hit something
      }
    }
  }
}

void DrawBullet(Bullet *bullet) {
  DrawCircleV(bullet->body.pos, bullet->body.radius, BLUE);
}

void DrawWeapon(Weapon *weapon) {
  for (int i = 0; i < weapon->bulletCapacity; ++i) {
    if (weapon->bullets[i].spawned) DrawBullet(&weapon->bullets[i]);
  }
}