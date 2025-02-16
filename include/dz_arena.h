#pragma once

// Arena allocation implementation
// Usage:
//  Malloc to give yourself a pointer to a spot in the arena
//  When the arena is cleared or freed, this pointer becomes invalid
//  Under the hood, this works by using one big malloc that is
//  constantly reused When the arena is cleared or freed, EVERYTHING
//  allocated becomes invalid

#include <stdlib.h>

typedef enum DzArenaError {
  DzArenaError_NONE,
  DzArenaError_MALLOC,  // Could not MALLOC
  DzArenaError_ALLOC,   // Alloc goes over max_size

  DzArenaError_COUNT,
} DzArenaError;

// Area Allocator
// Allocates objects into the area. Cannot free individual objects,
// can only destroy the whole arena. Arena automatically resizes,
// until max_size is reached.
typedef struct DZArena {
  size_t max_size;
  size_t first_empty_byte;
  char *data;
  DzArenaError error;
} DZArena;

extern const size_t DZ_ARENA_DEFAULT_MAX_SIZE;  // 1 MB

// Initializes an arena. If max_size is NULL, uses a default of
// DZ_ARENA_DEFAULT_MAX_SIZE Must be freed using dz_arena_free
extern DZArena dz_arena_init(size_t max_size);

// Frees ALL memory inside the arena
extern void dz_arena_free(DZArena *arena);

// Clears the area. Using a pointer from the arena after this results
// in UB Doesn't realloc anything
extern void dz_arena_clear(DZArena *arena);

// Allocates n_bytes bytes inside the arena, and returns a pointer to
// the allocated memory
// For safety, it also initializes the bytes to 0
extern void *dz_arena_alloc(DZArena *arena, size_t n_bytes);
