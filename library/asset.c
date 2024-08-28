#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>

#include "asset.h"
#include "asset_cache.h"
#include "color.h"
#include "sdl_wrapper.h"

typedef struct asset {
  asset_type_t type;
  SDL_Rect bounding_box;
} asset_t;

typedef struct text_asset {
  asset_t base;
  TTF_Font *font;
  const char *text;
  rgb_color_t color;
} text_asset_t;

typedef struct image_asset {
  asset_t base;
  SDL_Texture *texture;
  body_t *body;
} image_asset_t;

typedef struct button_asset {
  asset_t base;
  image_asset_t *image_asset;
  text_asset_t *text_asset;
  button_handler_t handler;
  void *aux;
  bool is_rendered;
} button_asset_t;

const size_t INIT_TEXT = 18;

asset_timer_t *asset_timer_init(asset_t *asset, double exp_time, timer_tick_handler_t tick_handler,
                                    timer_handler_t exp_handler, void *aux) {
  asset_timer_t *asset_timer = malloc(sizeof(asset_timer_t));
  assert(asset_timer);

  asset_timer->asset = asset;
  asset_timer->timer = 0.0;
  asset_timer->exp_time = exp_time;
  asset_timer->tick_handler = tick_handler;
  asset_timer->exp_handler = exp_handler;
  asset_timer->expired = false;
  asset_timer->aux = aux;
  asset_timer->tick_condition = true;

  return asset_timer;
}

void asset_timer_free(asset_timer_t *asset_timer) {
  free(asset_timer);
}

void asset_timer_tick(asset_timer_t *asset_timer, state_t* state, double dt) {
  if(asset_timer->expired) {
    return;
  }

  asset_timer->timer += dt;
  if(asset_timer->timer >= asset_timer->exp_time) {
    
    asset_timer->exp_handler(state, asset_timer->asset, asset_timer->aux);
    asset_timer->expired = true;
  }

  if (asset_timer->tick_handler != NULL && asset_timer->tick_condition) {
    asset_timer->tick_handler(state, asset_timer);
  }
}

/**
 * Allocates memory for an asset with the given parameters.
 *
 * @param ty the type of the asset
 * @param bounding_box the bounding box containing the location and dimensions
 * of the asset when it is rendered
 * @return a pointer to the newly allocated asset
 */
static asset_t *asset_init(asset_type_t ty, SDL_Rect bounding_box) {
  asset_t *new;
  switch (ty) {
  case ASSET_IMAGE: {
    new = malloc(sizeof(image_asset_t));
    break;
  }
  case ASSET_FONT: {
    new = malloc(sizeof(text_asset_t));
    break;
  }
  case ASSET_BUTTON: {
    new = malloc(sizeof(button_asset_t));
    break;
  }
  default: {
    assert(false && "Unknown asset type");
  }
  }
  assert(new);
  new->type = ty;
  new->bounding_box = bounding_box;
  return new;
}

asset_type_t asset_get_type(asset_t *asset) { return asset->type; }

asset_t *asset_make_image_with_body(const char *filepath, body_t *body) {
  SDL_Rect temp = {.w = 0, .h = 0, .x = 0, .y = 0};
  image_asset_t *image = (image_asset_t *)asset_init(ASSET_IMAGE, temp);
  image->texture =
      (SDL_Texture *)asset_cache_obj_get_or_create(ASSET_IMAGE, filepath);
  image->body = body;
  return (asset_t *)image;
}

asset_t *asset_make_image(const char *filepath, SDL_Rect bounding_box) {
  image_asset_t *image = (image_asset_t *)asset_init(ASSET_IMAGE, bounding_box);
  image->texture =
      (SDL_Texture *)asset_cache_obj_get_or_create(ASSET_IMAGE, filepath);
  image->body = NULL;
  return (asset_t *)image;
}

asset_t *asset_make_text(const char *filepath, SDL_Rect bounding_box,
                         const char *text, rgb_color_t color) {
  text_asset_t *txt = (text_asset_t *)asset_init(ASSET_FONT, bounding_box);
  txt->font = asset_cache_obj_get_or_create(ASSET_FONT, filepath);
  txt->color = color;
  txt->text = text;
  return (asset_t *)txt;
}

asset_t *asset_make_button(SDL_Rect bounding_box, asset_t *image_asset,
                           asset_t *text_asset, button_handler_t handler, void *aux) {
  button_asset_t *button =
      (button_asset_t *)asset_init(ASSET_BUTTON, bounding_box);
  assert(image_asset == NULL || image_asset->type == ASSET_IMAGE);
  button->image_asset = (image_asset_t *)image_asset;
  assert(text_asset == NULL || text_asset->type == ASSET_FONT);
  button->text_asset = (text_asset_t *)text_asset;

  button->handler = handler;
  button->aux = aux;

  button->is_rendered = false;

  return (asset_t *)button;
}

/**
 * Checks if coordinates (x, y) are within the given bounding box.
 *
 * @param bounding_box a SDL_Rect bounding box
 * @param x the x coordinate
 * @param y the y coordinate
 * @return whether the coordinate (x, y) is in the bounding box or not
 */
bool check_bounding_box(SDL_Rect bounding_box, double x, double y) {
  bool xIn = (bounding_box.x <= x) && (x <= bounding_box.x + bounding_box.w);
  bool yIn = (bounding_box.y <= y) && (y <= bounding_box.y + bounding_box.h);

  return xIn && yIn;
}

void asset_on_button_click(asset_t *button, state_t *state, double x,
                           double y) {
  button_asset_t *btn = (button_asset_t *)button;
  if (!btn->is_rendered) {
    return;
  }

  if (check_bounding_box(btn->base.bounding_box, x, y)) {
    btn->is_rendered = false;
    btn->handler(state, btn->aux);
  }
}

body_t *asset_get_body(asset_t *asset) {
  if (!asset || asset->type != ASSET_IMAGE) {
    return NULL;
  }
  image_asset_t *image_asset = (image_asset_t *)asset;
  return image_asset->body;
}

void asset_render(asset_t *asset) {
  if (!asset) {
    return;
  } else {
    switch (asset->type) {
    // case ASSET_IMAGE: {
    //   image_asset_t *image_asset = (image_asset_t *)asset;
    //   if (image_asset->body != NULL) {
    //     asset->bounding_box = sdl_get_bounding_box(image_asset->body);
    //   }
    //   vector_t v = {.x = asset->bounding_box.x, .y = asset->bounding_box.y};
    //   sdl_draw_image(image_asset->texture, asset->bounding_box.w,
    //                  asset->bounding_box.h, v);
    //   break;
    // }  
    case ASSET_IMAGE: {
      image_asset_t *image_asset = (image_asset_t *)asset;
      if (image_asset->body != NULL) {
        asset->bounding_box = sdl_get_bounding_box(image_asset->body);
        vector_t v = {.x = asset->bounding_box.x, .y = asset->bounding_box.y};
        sdl_draw_rotating_image(image_asset->texture, asset->bounding_box.w,
                     asset->bounding_box.h, v, body_get_rotation(image_asset->body), body_get_centroid(image_asset->body));
      }
      else {
        vector_t v = {.x = asset->bounding_box.x, .y = asset->bounding_box.y};
        sdl_draw_image(image_asset->texture, asset->bounding_box.w,
                      asset->bounding_box.h, v);
      }
      break;
    }
    case ASSET_FONT: {
      text_asset_t *txt_asset = (text_asset_t *)asset;
      vector_t v = {.x = asset->bounding_box.x, .y = asset->bounding_box.y};
      sdl_draw_text(txt_asset->text, txt_asset->font, INIT_TEXT, v,
                    txt_asset->color);
      break;
    }
    case ASSET_BUTTON: {
      button_asset_t *button = (button_asset_t *)asset;

      asset_render((asset_t *)button->image_asset);
      asset_render((asset_t *)button->text_asset);

      button->is_rendered = true;
      break;
    }
    }
  }
}

SDL_Rect asset_get_bounding_box(asset_t *asset) {
  return asset->bounding_box;
}

void asset_button_clear(asset_t *asset) { ((button_asset_t *)asset)->is_rendered = false; }

void asset_destroy(asset_t *asset) { 
  if (asset->type == ASSET_BUTTON) {
    button_asset_t *button_asset = (button_asset_t *) asset;
    if (button_asset->aux != NULL) {
      free(button_asset->aux);
    }
  }
  free(asset); 
}