#include "asset.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"
#include "list.h"

typedef struct button_info {
  const char *image_path;
  const char *font_path;
  SDL_Rect image_box;
  SDL_Rect text_box;
  rgb_color_t text_color;
  const char *text;
  button_handler_t handler;
} button_info_t;

void play_handler(state_t *state);

void leaderboard_handler(state_t *state);

void settings_handler(state_t *state);

void store_handler(state_t *state);

button_info_t buttons[] = {
    {.image_path = "assets/block.png",
     .font_path = "assets/font.ttf",
     .image_box = (SDL_Rect){400, 125, 200, 50},
     .text_box = (SDL_Rect){425, 135, 50, 50},
     .text_color = (rgb_color_t){0, 0, 0},
     .text = "PLAY",
     .handler = (void *)play_handler},
    {.image_path = "assets/block.png",
     .font_path = "assets/font.ttf",
     .image_box = (SDL_Rect){400, 175, 200, 50},
     .text_box = (SDL_Rect){425, 185, 50, 50},
     .text_color = (rgb_color_t){0, 0, 0},
     .text = "LEADERBOARD",
     .handler = (void *)leaderboard_handler},
    {.image_path = "assets/block.png",
     .font_path = "assets/font.ttf",
     .image_box = (SDL_Rect){400, 225, 200, 50},
     .text_box = (SDL_Rect){425, 235, 50, 50},
     .text_color = (rgb_color_t){0, 0, 0},
     .text = "SETTINGS",
     .handler = (void *)settings_handler},
    {.image_path = "assets/block.png",
     .font_path = "assets/font.ttf",
     .image_box = (SDL_Rect){400, 275, 200, 50},
     .text_box = (SDL_Rect){425, 285, 50, 50},
     .text_color = (rgb_color_t){0, 0, 0},
     .text = "STORE",
     .handler = (void *)store_handler}
};
  
const size_t NUM_BUTTONS = 4;

list_t *button_assets() {
    list_t *button_assets = list_init(NUM_BUTTONS, (free_func_t)asset_destroy);

    for (size_t i = 0; i < NUM_BUTTONS; i ++) {
        button_info_t button = buttons[i];
        asset_t *text_asset = asset_make_text(button.font_path, button.text_box, button.text, button.text_color);
        asset_t *image_asset = asset_make_image(button.image_path, button.image_box);
        asset_t *button_asset = asset_make_button(button.image_box, image_asset, text_asset, button.handler);
        asset_cache_register_button(button_asset);
        list_add(button_assets, button_asset);
    }
    return button_assets;
}

void leaderboard_handler(state_t *state) {
    state->scene = state->leaderboard_scene;
    state->body_assets = state->leaderboard_body_assets;
}

void play_handler(state_t *state) {
  state->body_assets = state->game_body_assets;
  state->scene = state->game_scene;
}

void settings_handler(state_t *state) {
  state->scene = state->setting_scene;
  state->body_assets = state->setting_body_assets;
}

void store_handler(state_t *state) {
  state->scene = state->store_scene;
  state->body_assets = state->store_body_assets;
}