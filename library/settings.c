#include "asset.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"
#include "list.h"

const char *filepath = "assets/leaderboard.txt";
const char *font_path = "assets/font.ttf";
const size_t font_size = 18;

vector_t text_location = (vector_t){375, 175};
rgb_color_t color = (rgb_color_t){0, 0, 0};

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

void home_handler(state_t *state);

button_info_t settings_buttons[] = {
    {.image_path = "assets/green_rectangle.png",
     .font_path = "assets/font.ttf",
     .image_box = (SDL_Rect){400, 125, 200, 50},
     .text_box = (SDL_Rect){425, 135, 50, 50},
     .text_color = (rgb_color_t){0, 0, 0},
     .text = "YES",
     .handler = (void *)play_handler},
    {.image_path = "assets/green_rectangle.png",
     .font_path = "assets/font.ttf",
     .image_box = (SDL_Rect){400, 175, 200, 50},
     .text_box = (SDL_Rect){425, 185, 50, 50},
     .text_color = (rgb_color_t){0, 0, 0},
     .text = "NO",
     .handler = (void *)leaderboard_handler},
};

button_info_t back_button = {.image_path = "assets/block.png",
     .font_path = "assets/font.ttf",
     .image_box = (SDL_Rect){30, 30, 100, 50},
     .text_box = (SDL_Rect){60, 40, 50, 50},
     .text_color = (rgb_color_t){0, 0, 0},
     .text = "BACK",
     .handler = (void *)home_handler
};

void home_handler(state_t *state) {
  state->scene = state->home_scene;
  state->body_assets = state->home_body_assets;
}

const size_t SETTING_NUM_BUTTONS = 3;

list_t *button_assets() {
    list_t *button_assets = list_init(NUM_BUTTONS, (free_func_t)asset_destroy);

    char *respawn = "ABILITY TO RESPAWN?";
    SDL_Rect title_bounding_box = {.w = strlen(respawn) * font_size, .h = font_size * 2, .x = text_location.x, .y = text_location.y - (font_size * 2)};
    asset_t *title_asset = asset_make_text(font_path, title_bounding_box, respawn, color);
    list_add(button_assets, title_asset);

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