#include "helpers.h"

#include <time.h>

#include "raylib.h"

#ifdef _WIN32

double time_in_seconds() {
  return (double)clock() / CLOCKS_PER_SEC;  // Convert clock ticks to seconds
}

#else
#include <sys/time.h>

double time_in_seconds() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec +
         tv.tv_usec / 1e6;  // Convert seconds and microseconds to seconds
}
#endif

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
