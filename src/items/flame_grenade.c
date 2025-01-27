#include <math.h>
#include <stdlib.h>

#include "helpers.h"
#include "item.h"
#include "level.h"
#include "lib/stb_ds.h"
#include "modules/camera.h"
#include "raylib.h"
#include "raymath.h"

#define FLAME_TTL 5       // seconds
#define FLAME_SIZE 100    // radius
#define TIME_TO_SPLASH 1  // seconds
#define DEFAULT_RANGE 400
#define TICK_DAMAGE 10        // per second
#define FLAME_FIRE_DELAY 1.2  // in seconds

extern EnemySpawner enemySpawner;
extern Level level;

static PlayerItem *FlameGrenade;

typedef struct FlameGrenadeBullet {
  Vector2 target;
  Vector2 origin;
  float progress;
  Vector2 pos;
} FlameGrenadeBullet;
typedef struct FlameSplash {
  Vector2 pos;
  double createdAt;  // time
} FlameSplash;

static FlameGrenadeBullet *bullets = NULL;
static FlameSplash *splahes = NULL;

static void TickFlame(int index) {
  if (time_in_seconds() - splahes[index].createdAt > FLAME_TTL) {
    arrdelswap(splahes, index);
    return;
  }
  EntityAreaTakeDamage(level.allEntities, arrlen(level.allEntities),
                       splahes[index].pos, FLAME_SIZE,
                       TICK_DAMAGE * GetFrameTime());
  // damage everything in circle, enemies and player
}

static void Splash(int index) {
  // remove bullet, create flame area
  Vector2 splashPosition = bullets[index].target;
  FlameSplash newSplash = {
      .pos = splashPosition,
      .createdAt = time_in_seconds(),
  };
  arrput(splahes, newSplash);
  arrdelswap(bullets, index);
}

static void TickBullet(int index) {
  // move bullet towards target, in arc motion
  bullets[index].progress += TIME_TO_SPLASH * GetFrameTime();
  if (bullets[index].progress >= 1) {
    Splash(index);
    return;
  }
}

// can return NULL
static Enemy *GetRandomEnemyInRange(Vector2 origin, float range) {
  Enemy *enemies = enemySpawner.enemies;
  Vector2 closest = {INFINITY, INFINITY};
  float rangeSqr = range * range;
  // I use the heap here instead of the stack because I imagine there could be
  // many enemies
  int *availableEnemyIndices =
      malloc(enemySpawner.highestEnemyIndex *
             sizeof(int));  // only need to store MAX highestEnemyIndex enemies
  int currentAvailableIndex = 0;
  for (int i = 0; i < enemySpawner.highestEnemyIndex; ++i) {
    if (enemies[i].spawned) {
      float distanceSqr =
          Vector2DistanceSqr(origin, enemies[i].entity.body.pos);
      if (distanceSqr < rangeSqr) {
        availableEnemyIndices[currentAvailableIndex++] = i;
      }
    }
  }
  if (currentAvailableIndex == 0) return NULL;
  int randomIndex = GetRandomValue(0, currentAvailableIndex);
  Enemy *enemy = &enemies[availableEnemyIndices[randomIndex]];
  free(availableEnemyIndices);
  return enemy;
}

static FlameGrenadeBullet CreateBullet(Vector2 origin, Vector2 target) {
  return (FlameGrenadeBullet){
      .origin = origin,
      .target = target,
      .progress = 0,
  };
}

static void Shoot(Vector2 origin, float rangeModifier) {
  Enemy *target = GetRandomEnemyInRange(origin, DEFAULT_RANGE * rangeModifier);
  if (target == NULL) return;
  // create bullet
  FlameGrenadeBullet newBullet = CreateBullet(origin, target->entity.body.pos);
  arrput(bullets, newBullet);
}

static void Tick(OLD_Player *player) {
  static double lastShot = 0;
  double now = time_in_seconds();
  if (now - lastShot > FLAME_FIRE_DELAY) {
    Shoot(player->entity.body.pos, 1.0f);
    lastShot = now;
  }

  for (int i = 0; i < arrlen(bullets); ++i) TickBullet(i);
  for (int i = 0; i < arrlen(splahes); ++i) TickFlame(i);
}

static void DrawFlame(int index) {
  DrawCircleV(splahes[index].pos, FLAME_SIZE, ORANGE);
  DrawCircleLinesV(splahes[index].pos, FLAME_SIZE, RED);
}

static void DrawBullet(int index) {
  float pathLengthSqr =
      Vector2DistanceSqr(bullets[index].origin, bullets[index].target);
  if (!IsCircleOnScreen(bullets[index].target, pathLengthSqr))
    return;  // don't bother calculating arc and position

  // calculate parabolic between origin and target, then draw circle at
  Vector2 arcPoint = PointAlongArc(bullets[index].origin, bullets[index].target,
                                   bullets[index].progress);
  DrawCircleV(arcPoint, 5, RED);
}

static void DrawBackground() {
  for (int i = 0; i < arrlen(splahes); ++i) DrawFlame(i);
}
static void DrawForeground() {
  for (int i = 0; i < arrlen(bullets); ++i) DrawBullet(i);
}

PlayerItem *InitializeFlameGrenade() {
  FlameGrenade = calloc(1, sizeof(PlayerItem));
  FlameGrenade->afterTick = Tick;
  FlameGrenade->drawBackground = DrawBackground;
  FlameGrenade->drawForeground = DrawForeground;
  return FlameGrenade;
}

void FreeFlameGrenade() {
  arrfree(bullets);
  arrfree(splahes);
}