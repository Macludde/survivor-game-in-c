#include "enemy.h"
#include "helpers.h"
#include "item.h"
#include "level.h"
#include "lib/stb_ds.h"
#include "modules/camera.h"
#include "player.h"
#include "raylib.h"
#include "raymath.h"

extern EnemySpawner enemySpawner;
extern Level level;
extern Items items;

#define SG_DAMAGE 10
#define SG_RANGE 800
#define SG_SPEED 800
#define SG_FIRE_DELAY 1  // seconds

typedef struct Bullet {
  PhysicsBody body;
} Bullet;

static Bullet *bullets = NULL;

static void RemoveBullet(int index) { arrdelswap(bullets, index); }

static void BulletHit(int index, Entity *entity, float damage) {
  float remainingHealth = EntityTakeDamage(entity, damage);
  if (remainingHealth <= 0) {
    // entity died
  } else {
    // RigidCollision(&bullets[index].body, &entity->body);
  }
  RemoveBullet(index);
}

static bool HandleBulletCollisions(int index, Entity **entities, int count,
                                   float damage) {
  for (int i = count - 1; i >= 0; --i) {
    if (entities[i]->type == ENTITY_TYPE_PLAYER) continue;
    if (CheckCollision(bullets[index].body, entities[i]->body)) {
      BulletHit(index, entities[i], damage);
      return true;
    }
  }
  return false;
}

static bool HandleBulletOffScreen(int index) {
  Bullet bullet = bullets[index];
  if (!IsCircleOnScreen(bullet.body.pos, bullet.body.radius)) {
    RemoveBullet(index);
    return true;
  }
  return false;
}

static Vector2 ClosestEnemy(Vector2 pos, float maxRange) {
  Enemy *enemies = enemySpawner.enemies;
  Vector2 closest = {INFINITY, INFINITY};
  float closestDistanceSqr = maxRange * maxRange;
  for (int i = 0; i < enemySpawner.highestEnemyIndex; ++i) {
    if (enemies[i].spawned) {
      float distanceSqr = Vector2DistanceSqr(pos, enemies[i].entity.body.pos);
      if (distanceSqr < closestDistanceSqr) {
        closestDistanceSqr = distanceSqr;
        closest = enemies[i].entity.body.pos;
      }
    }
  }

  return closest;
}

static bool SpawnBullet(OLD_Player *player) {
  Vector2 origin = player->entity.body.pos;
  Vector2 closestEnemy = ClosestEnemy(origin, SG_RANGE * player->stats.range);
  if (closestEnemy.x == INFINITY) return false;  // no enemies in range
  Vector2 direction = Vector2Add(
      // player->entity.body.velocity,
      Vector2Zero(),
      Vector2Scale(Vector2Normalize(Vector2Subtract(closestEnemy, origin)),
                   player->stats.bulletSpeed * SG_SPEED));
  Bullet newBullet = {
      .body =
          {
              .pos = origin,
              .velocity = direction,
              .radius = 5,
              .mass = 25,
          },
  };
  arrput(bullets, newBullet);
  return true;
}

static void Tick(OLD_Player *player) {
  static double lastShot = 0;
  Vector2 playerPos = player->entity.body.pos;
  double now = time_in_seconds();
  if (now - lastShot > SG_FIRE_DELAY / player->stats.attackSpeed) {
    SpawnBullet(player);
    lastShot = now;
  }
  for (int i = 0; i < arrlen(bullets); ++i) {
    Bullet *bullet = &bullets[i];
    bullet->body.pos = Vector2Add(
        bullet->body.pos, Vector2Scale(bullet->body.velocity, GetFrameTime()));
    // if (HandleBulletCollisions(i, level.allEntities,
    // arrlen(level.allEntities),
    //                            SG_DAMAGE * player->stats.damage))
    //   continue;
    if (HandleBulletOffScreen(i)) continue;
  }
}

static void DrawBullet(int index) {
  Bullet bullet = bullets[index];
  DrawCircleV(bullet.body.pos, bullet.body.radius, BLUE);
}

static void Draw(OLD_Player *player) {
  for (int i = 0; i < arrlen(bullets); ++i) {
    DrawBullet(i);
  }
}

PlayerItem *InitializeSimpleGun() {
  PlayerItem *item = calloc(1, sizeof(PlayerItem));
  item->afterTick = Tick;
  item->drawForeground = Draw;
  return item;
}

void FreeSimpleGun(PlayerItem *item) { arrfree(bullets); }