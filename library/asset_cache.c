#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>

#include "asset.h"
#include "asset_cache.h"
#include "list.h"
#include "sdl_wrapper.h"

static list_t *ASSET_CACHE;
const size_t INITIAL_CAPACITY = 5;
const ssize_t INVALID = -1;
const size_t FONT_SIZE = 18;

typedef struct {
  asset_type_t type;
  const char *filepath;
  void *obj;
} entry_t;

static void asset_cache_free_entry(entry_t *entry) {
  switch (entry->type) {
  case ASSET_FONT:
    TTF_CloseFont(entry->obj);
    break;
  case ASSET_IMAGE:
    SDL_DestroyTexture(entry->obj);
    break;
  case ASSET_BUTTON:
    free(entry->obj);
    break;
  }
}

void asset_cache_init() {
  ASSET_CACHE =
      list_init(INITIAL_CAPACITY, (free_func_t)asset_cache_free_entry);
}

void asset_cache_destroy() { list_free(ASSET_CACHE); }

ssize_t entry_exists(const char *filepath) {
  for (size_t i = 0; i < list_size(ASSET_CACHE); i++) {
    entry_t *file = list_get(ASSET_CACHE, i);
    if (file->type != ASSET_BUTTON && strcmp(file->filepath, filepath) == 0) {
      return i;
    }
  }
  return INVALID;
}

void *asset_cache_obj_get_or_create(asset_type_t ty, const char *filepath) {
  ssize_t idx = entry_exists(filepath);
  if (idx != INVALID) {
    return ((entry_t *)list_get(ASSET_CACHE, idx))->obj;
  } else {
    entry_t *entry = malloc(sizeof(entry_t));
    assert(entry);
    entry->type = ty;
    entry->filepath = filepath;
    switch (ty) {
    case ASSET_FONT: {
      entry->obj = sdl_load_font(filepath, FONT_SIZE);
      break;
    }
    case ASSET_IMAGE: {
      entry->obj = sdl_load_image(filepath);
      break;
    }
    default: {
      abort();
    }
    }
    list_add(ASSET_CACHE, entry);
    return entry->obj;
  }
}

void asset_cache_register_button(asset_t *button) {
  entry_t *entry = malloc(sizeof(entry_t));
  assert(entry);

  entry->type = asset_get_type(button);
  assert(entry->type == ASSET_BUTTON);
  entry->filepath = NULL;
  entry->obj = button;

  list_add(ASSET_CACHE, entry);
}

void asset_cache_handle_buttons(state_t *state, double x, double y) {
  for (size_t i = 0; i < list_size(ASSET_CACHE); i++) {
    entry_t *entry = list_get(ASSET_CACHE, i);
    if (entry->type == ASSET_BUTTON) {
      asset_on_button_click(entry->obj, state, x, y);
    }
  }
}