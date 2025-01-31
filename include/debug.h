#pragma once

#ifdef DEBUG
#define DEBUG_PRINT(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__);
#else
#define DEBUG_PRINT(fmt, ...)  // Don't do anything in release builds
#endif

void HandleDebuggingKeys();