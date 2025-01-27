#pragma once

typedef struct PlayerItem PlayerItem;
#include "enemy.h"
#include "player.h"
#include "raylib.h"

struct PlayerItem {
  int order;  // defines consistent order of items
  void (*beforeTick)(Player *);
  void (*afterTick)(Player *);
  void (*onDeath)(float);                 // overkill
  void (*onKill)(float, struct Enemy *);  // overkill, shot enemy
  void (*onMove)(Vector2);                // direction
  void (*drawBackground)(Player *);
  void (*drawForeground)(Player *);
};

typedef PlayerItem **Items;
extern Items items;

void InitializeItems();
void FreeItems();
void ItemsCallBeforeTick(Player *player);
void ItemsCallAfterTick(Player *player);
void ItemsCallDrawBackground(Player *player);
void ItemsCallDrawForeground(Player *player);

/*
Items to build:
- Flame grenade
  Every once in a while, shoot out a grenade which moves in an arc motion. When
it lands it spawns a circle on the ground of flame, which damages enemies and
player standing in it.
- Shifting Invincibility
  Every once in a while, become temporarily invincible.
- Poison modifier
  Make all damage also do poison damage, doing damage over time to enemies.
- Frost arc
  Shoot out an arc of frost which does little to no damage but slows enemies
quite a lot.
- Bloodthirst
  Whenever an enemy is killed, restore a little hp
- Stat increases
  Increased damage, attack rate, bullet speed, movement speed, max health and
range (zooms out camera as well)
- Flight
  Fly over trees and other obstacles. Enemies still damage you
*/