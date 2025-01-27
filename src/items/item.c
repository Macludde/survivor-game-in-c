#include "item.h"

#include "lib/stb_ds.h"

PlayerItem *InitializeFlameGrenade();
PlayerItem *InitializeSimpleGun();
void FreeSimpleGun();
void FreeFlameGrenade();

Items items;

void InitializeItems() {
  arrput(items, InitializeSimpleGun());
  arrput(items, InitializeFlameGrenade());
}

void FreeItems() {
  FreeSimpleGun();
  FreeFlameGrenade();
  for (int i = 0; i < arrlen(items); ++i) {
    free(items[i]);
  }
  arrfree(items);
}

void ItemsCallBeforeTick(OLD_Player *player) {
  for (int i = 0; i < arrlen(player->items); ++i) {
    if (player->items[i]->beforeTick == NULL) continue;
    player->items[i]->beforeTick(player);
  }
}
void ItemsCallAfterTick(OLD_Player *player) {
  for (int i = 0; i < arrlen(player->items); ++i) {
    if (player->items[i]->afterTick == NULL) continue;
    player->items[i]->afterTick(player);
  }
}
void ItemsCallDrawBackground(OLD_Player *player) {
  for (int i = 0; i < arrlen(player->items); ++i) {
    if (player->items[i]->drawBackground == NULL) continue;
    player->items[i]->drawBackground(player);
  }
}
void ItemsCallDrawForeground(OLD_Player *player) {
  for (int i = 0; i < arrlen(player->items); ++i) {
    if (player->items[i]->drawForeground == NULL) continue;
    player->items[i]->drawForeground(player);
  }
}