#include "helpers.h"

#include <sys/time.h>

#include "raylib.h"

double time_in_seconds() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec +
         tv.tv_usec / 1e6;  // Convert seconds and microseconds to seconds
}

Color SlightColorVariation(Color base) {
  return ColorLerp(base,
                   (Color){
                       .r = GetRandomValue(-10, 10),
                       .g = GetRandomValue(-10, 10),
                       .b = GetRandomValue(-10, 10),
                       .a = 255,
                   },
                   0.1f);
}