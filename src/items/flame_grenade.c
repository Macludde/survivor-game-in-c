#include <stdlib.h>

#include "item.h"

PlayerItem *FlameGrenade;

void InitializeItem() { FlameGrenade = calloc(1, sizeof(PlayerItem)); }

void TickFlame() {
  // damage everything in circle, enemies and player
}

void Splash() {
  // remove bullet, create flame area
}

void TickBullet() {
  // move bullet towards target, in arc motion
}

void Shoot() {
  // select target, spawn bullet
}

void Tick() {
  // check timer
  // shoot if necessary, and reset timer
}