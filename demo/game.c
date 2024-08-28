#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <float.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "user.h"
#include "asset.h"
#include "asset_cache.h"
#include "collision.h"
#include "forces.h"
#include "sdl_wrapper.h"
#include "leaderboard.h"
#include "database.h"

// SCREEN CONSTANTS
const vector_t MIN = {.x=0, .y=0};
const vector_t MAX = {.x=1000, .y=500};
const vector_t OFFSCREEN = {.x=-1000, .y=-1000};

// HEALTH BAR CONSTANTS
const size_t HEART_OFFSET = 20;
const size_t HEART_SIZE = 20;
const char *HEART_PATH = "assets/heart.png";
const vector_t INIT_HEART = {.x=975, .y=475};
static const size_t FULL_HEALTH = 100;
const size_t HEALTH_PER_HEART = 20;

// POWERUP CONSTANTS
const char *HEALTH_POWER_PATH = "assets/health-powerup.png";
const char *MUFFLER_POWER_PATH = "assets/muffler-powerup.png";
const char *SCORE_DOUBLER_POWER_PATH = "assets/score-doubler-powerup.png";
const char *FORCE_ICON_PATH = "assets/force_icon.png";
const char *SLOW_DOWN_POWER_PATH = "assets/slow-down-powerup.png";

const char *FORCE_FIELD_PATH = "assets/force_field.png";
const double FORCE_FIELD_RADIUS = (double) (MAX.y) / (double) 2.0;
const double FORCE_FIELD_TIME = 20.0;
const double FORCE_FIELD_ELASTICITY = 1.0;

const double POWERUP_RADIUS = 15;
const vector_t POWERUP_INIT_VEL = (vector_t) {-150, 0};
const size_t NUM_POWERUPS = 5;
const size_t MIN_SPAWN_Y = 150;
const double MIN_POW_SPAWN_INTERVAL = 10;
const double MAX_POW_SPAWN_INTERVAL = 20;
const double POWERUP_DURATION = 15;

// USER CONSTANTS
const size_t CIRC_NUM_POINTS = 20;
const double USER_RADIUS = 22;
const size_t USER_MASS = 1;
const vector_t USER_START_POS = (vector_t){100, 250};
static const size_t USERNAME_LEN = 16;
const double USER_GRAVITY = 350;

// JETPACK CONSTANTS
const char *JETPACK_FLAME_PATH = "assets/jetpack_flame.png";
const char *JETPACK_NO_FLAME_PATH = "assets/jetpack_no_flame.png";
const vector_t JETPACK_IMPULSE = (vector_t){0, 100};
const double JETPACK_HEIGHT = 70;
const double JETPACK_WIDTH = 35;
const vector_t JETPACK_OFFSET = (vector_t){-0.5 * JETPACK_WIDTH - 0.3 * USER_RADIUS, 0};

// COLOR CONSTANTS
const rgb_color_t user_color = (rgb_color_t){1, 0, 0};
const rgb_color_t white = (rgb_color_t){1,1,1};
const rgb_color_t black = (rgb_color_t){0,0,0};
const rgb_color_t cyan = (rgb_color_t){0,255,255};

// WALL CONSTANTS
const double WALL_DIM = 1;
const double WALL_OFFSET = 100;
const double GROUND_HEIGHT = 90;
const double WALL_ELASTICITY = 0.3;

// BACKGROUND CONSTANTS
const char *BACKGROUND_PATH = "assets/background.jpeg";

// OBSTACLE CONSTANTS
typedef enum { ROCKET, SINUSOIDAL, LASER, LASER_RIGHT, LASER_LEFT, BLOCK, USER,
               WALL, GAME, POWERUP, HEART, REBOUNDING } body_type_t;

const vector_t OBSTACLE_DIM = (vector_t){70, 50};
const size_t BLOCK_DAMAGE = 10;
const double SIN_FACTOR = 25;
const size_t VERTICAL_FACTOR = 10;
const vector_t BASE_OBJ_VEL = (vector_t){-200, 0};

const vector_t ROCKET_DIM = (vector_t){140, 50};
const vector_t ROCKET_VEL = (vector_t){-400, 0};
const double ROCKET_VEL_MAG = 400;
const size_t ROCKET_DAMAGE = 30;

const double LASER_HEIGHT = 18;
const double LASER_WARNING_HEIGHT = 54;
const double LASER_OFFSET = 94;
const double LASER_TIMER = 5;
const double WARNING_LIMIT = 2.0;
const size_t LASER_DAMAGE = 20;

// obstacle, rocket, laser, sinusoidal obstacle
const double OBSTACLE_PROB[4] = {0.4, 0.3, 0.2, 0.1}; 
const size_t ROCKET_THRESHOLD = 5000;
const size_t AIMING_THRESHOLD = 300;
const double MIN_OBS_SPAWN_INTERVAL = 0.5;
const double MAX_OBS_SPAWN_INTERVAL = 5;
const size_t RECT_NUM_POINTS = 4;
const double FREQUENCY = 0.01;

const char *BLOCK_PATH = "assets/block.png";
const char *ROCKET_PATH = "assets/rocket.png";
const char *LASER_PATH = "assets/laser.png";
const char *LASER_LEFT_PATH = "assets/laser_left.png";
const char *LASER_RIGHT_PATH = "assets/laser_right.png";

// TEXT CONSTANTS
static const char *font_path = "assets/font.ttf";
const char *RESPAWN = "ABILITY TO RESPAWN?";
const char *RESPAWN_MSG = "Respawning...";
const size_t RESPAWN_DELAY = 3;
static const size_t font_size = 18;
const size_t MAX_DIGITS = 10;

// TEXTBOX CONSTANTS
const char *RECTANGLE_PATH = "assets/rectangle.png";
static const vector_t text_location = (vector_t){375, 175};
const double TEXT_FACTOR = 2.5;

const char ASCII_0 = '0';
const char ASCII_9 = '9';
const char ASCII_A = 'A';
const char ASCII_z = 'z';
const char *PLACE_HOLDER = " USERNAME"; // space for SDL

// AUDIO CONSTANTS
const char* MENU_MUSIC_PATH = "assets/audio.wav";
const char* GAME_MUSIC_PATH = "assets/audio-1.wav";
const char* COLLISION_SOUND_PATH = "assets/collision_audio.wav";
const char* JETPACK_SOUND_PATH = "assets/jetpack_on_audio.wav";
const char* LASER_SOUND_PATH = "assets/laser.wav";
const char* POWRUP_SOUND_PATH = "assets/powerup.wav";

// STORE CONSTANTS
const size_t N_SKINS = 12;
const size_t STORE_ROWS = 2;
const size_t STORE_COLUMNS = 6;

const double CANVAS_SIZE = 100;
const char *CANVAS_PATH = "assets/frame.png";

const char* SKIN_PATHS[] = {"assets/skins/0.png", "assets/skins/1.png", "assets/skins/2.png",
  "assets/skins/3.png", "assets/skins/4.png", "assets/skins/5.png", "assets/skins/6.png",
  "assets/skins/7.png", "assets/skins/8.png", "assets/skins/9.png", "assets/skins/10.png",
  "assets/skins/11.png"
  };
const double SKIN_WIDTH = 60;
const double SKIN_HEIGHT = 80;

const SDL_Rect SCORE_BBOX = (SDL_Rect){MAX.x-200, 40, 50, 50};
const char *SCORE_FORMAT = "SCORE: %zu";
const char *EQUIPPED = "EQUIPPED";
const char *EQUIP = "EQUIP";

const size_t PRICES[] = {0, 500, 1000, 2500, 5000, 8000, 10000, 20000, 50000, 69420,
  75000, 99999};
const char *PRICE_FORMAT = "BUY $%zu";

const double STORE_BUTTON_TEXT_W = 100;
const double STORE_BUTTON_TEXT_H = 30;
const double STORE_BUTTON_W = 120;
const double STORE_BUTTON_H = 50;
const double STORE_BUTTON_OFFSET = 30;

const size_t ITEMS_PER_SKIN = 6;
const size_t FRAME_COUNT = 2;
const size_t EQUIP_IDX_OFFSET = 2;
const size_t BUY_IDX_OFFSET = 3;

// SCORE RENDERING CONSTANTS
const vector_t SCORE_LOC = (vector_t){30, 50};

// MENU CONSTANTS
const size_t HOME_NUM_BUTTONS = 5;
const size_t SETTINGS_NUM_BUTTONS = 2;

typedef struct button_info {
  const char *image_path;
  const char *font_path;
  SDL_Rect image_box;
  SDL_Rect text_box;
  rgb_color_t text_color;
  const char *text;
  button_handler_t handler;
} button_info_t;

typedef struct powerup_aux {
  state_t *state;
  body_t *powerup;
  powerup_type_t type;
} powerup_aux_t;

struct state {
  list_t *timed_assets;

  list_t *body_assets;
  list_t *home_body_assets;
  list_t *game_body_assets;
  list_t *leaderboard_body_assets;
  list_t *setting_body_assets;
  list_t *store_body_assets;
  list_t *login_body_assets;

  scene_t *scene;
  scene_t *home_scene;
  scene_t *game_scene;
  scene_t *leaderboard_scene;
  scene_t *setting_scene;
  scene_t *store_scene;
  scene_t *login_scene;
  
  user_t *user;
  bool respawn;
  bool has_respawned;
  bool warning;
  bool transition;
  bool slowdown;
  size_t health;

  list_t *database;
  db_entry_t *user_data;
  
  list_t *store_strings;    // to keep track of malloc'ed strings
  size_t current_skin;

  body_t *left_wall;
  body_t *right_wall;
  body_t *ceiling;
  body_t *ground;

  double time_since_obstacle;
  double next_obstacle_time;

  double time_since_powerup;
  double next_powerup_time;
  double buffer_time;

  asset_t *background;

  char *score_str;
  asset_t *score_asset;

  Mix_Music *menu_music;
  Mix_Music *game_music;
  Mix_Chunk *jetpack_sound;
  Mix_Chunk *collision_sound;
  Mix_Chunk *laser_sound;
  Mix_Chunk *powerup_sound;
};

void make_store(state_t *state);

void reset_game(body_t *body1, body_t *body2, vector_t axis, void *aux,
                double force_const);

void respawn_game(state_t *state);

void play_handler(state_t *state, void *aux);

void leaderboard_handler(state_t *state, void *aux);

void login_handler(state_t *state, void *aux);

void logout_handler(state_t *state, void *aux);

void settings_handler(state_t *state, void *aux);

void store_handler(state_t *state, void *aux);

void home_handler(state_t *state, void *aux);

void yes_handler(state_t *state, void *aux);

void no_handler(state_t *state, void *aux);

void speed_everything_up(state_t *state);

void make_user(state_t *state);

button_info_t back_button = {.image_path = "assets/block.png",
     .font_path = "assets/font.ttf",
     .image_box = (SDL_Rect){30, 30, 100, 50},
     .text_box = (SDL_Rect){60, 40, 50, 50},
     .text_color = (rgb_color_t){0, 0, 0},
     .text = "BACK",
     .handler = (void *)home_handler
};

button_info_t settings_buttons[] = {
    {.image_path = "assets/block.png",
     .font_path = "assets/font.ttf",
     .image_box = (SDL_Rect){400, 175, 75, 50},
     .text_box = (SDL_Rect){425, 185, 50, 50},
     .text_color = (rgb_color_t){0, 0, 0},
     .text = "YES",
     .handler = (void *)yes_handler},
    {.image_path = "assets/block.png",
     .font_path = "assets/font.ttf",
     .image_box = (SDL_Rect){400, 225, 75, 50},
     .text_box = (SDL_Rect){425, 235, 50, 50},
     .text_color = (rgb_color_t){0, 0, 0},
     .text = "NO",
     .handler = (void *)no_handler},
};

button_info_t login = {.image_path = "assets/block.png",
                          .font_path = "assets/font.ttf",
                          .image_box = (SDL_Rect){400, 275, 200, 50},
                          .text_box = (SDL_Rect){425, 285, 50, 50},
                          .text_color = (rgb_color_t){0, 0, 0},
                          .text = "Login",
                          .handler = (void *)login_handler};

button_info_t home_buttons[] = {
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
     .handler = (void *)store_handler},
    {.image_path = "assets/block.png",
     .font_path = "assets/font.ttf",
     .image_box = (SDL_Rect){400, 325, 200, 50},
     .text_box = (SDL_Rect){425, 335, 50, 50},
     .text_color = (rgb_color_t){0, 0, 0},
     .text = "LOGOUT",
     .handler = (void *)logout_handler}
};

double rand_range(double low, double high) {
  double normed = (double)rand() / (double) RAND_MAX;
  return low + (normed) * (high - low);
}

asset_t *make_back_button() {
  asset_t *text_asset = asset_make_text(back_button.font_path, back_button.text_box, back_button.text, back_button.text_color);
  asset_t *image_asset = asset_make_image(back_button.image_path, back_button.image_box);
  asset_t *back_button_asset = asset_make_button(back_button.image_box, image_asset, text_asset, back_button.handler, NULL);
  asset_cache_register_button(back_button_asset);

  return back_button_asset;
}

list_t *home_button_assets() {
    list_t *button_assets = list_init(HOME_NUM_BUTTONS, (free_func_t)asset_destroy);

    for (size_t i = 0; i < HOME_NUM_BUTTONS; i ++) {
        button_info_t button = home_buttons[i];
        asset_t *text_asset = asset_make_text(button.font_path, button.text_box, button.text, button.text_color);
        asset_t *image_asset = asset_make_image(button.image_path, button.image_box);
        asset_t *button_asset = asset_make_button(button.image_box, image_asset, text_asset, button.handler, NULL);
        asset_cache_register_button(button_asset);
        list_add(button_assets, button_asset);
    }
    return button_assets;
}

list_t *login_button_assets() {
  list_t *button_assets = list_init(1, (free_func_t)asset_destroy);

  asset_t *text_asset = asset_make_text(login.font_path, login.text_box, login.text, login.text_color);
  asset_t *image_asset = asset_make_image(login.image_path, login.image_box);
  asset_t *button_asset = asset_make_button(login.image_box, image_asset, text_asset, login.handler, NULL);
  asset_cache_register_button(button_asset);
  list_add(button_assets, button_asset);

  return button_assets;
}

list_t *settings_button_assets() {
    list_t *button_assets = list_init(SETTINGS_NUM_BUTTONS, (free_func_t)asset_destroy);

    SDL_Rect title_bounding_box = {.w = strlen(RESPAWN) * font_size, .h = font_size * 2, .x = text_location.x, .y = text_location.y - (font_size * 2)};
    asset_t *title_asset = asset_make_text(font_path, title_bounding_box, RESPAWN, cyan);
    list_add(button_assets, title_asset);

    for (size_t i = 0; i < SETTINGS_NUM_BUTTONS; i ++) {
        button_info_t button = settings_buttons[i];
        asset_t *text_asset = asset_make_text(button.font_path, button.text_box, button.text, button.text_color);
        asset_t *image_asset = asset_make_image(button.image_path, button.image_box);
        asset_t *button_asset = asset_make_button(button.image_box, image_asset, text_asset, button.handler, NULL);
        asset_cache_register_button(button_asset);
        list_add(button_assets, button_asset);
    }

    list_add(button_assets, make_back_button());
    return button_assets;
}

body_type_t *make_type_info(body_type_t type) {
  body_type_t *info = malloc(sizeof(body_type_t));
  *info = type;
  return info;
}

body_type_t get_type(body_t *body) {
  return *(body_type_t *)body_get_info(body);
}

void annihlate_body(body_t *body, state_t *state) {
  for (size_t i = 0; i < list_size(state->body_assets); i++) {
    asset_t *asset = list_get(state->body_assets, i);
    if (asset_get_body(asset) == body) {
      list_remove(state->body_assets, i);
      asset_destroy(asset);
      break;
    }
  }
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    if (scene_get_body(state->scene, i) == body) {
      scene_remove_body(state->scene, i);
      break;
    }
  }
}

void remove_timed_asset(state_t *state, body_t *body) {
  for (size_t i = 0; i < list_size(state->timed_assets); i ++) {
    asset_timer_t *asset_timer = list_get(state->timed_assets, i);
    if (asset_get_body(asset_timer->asset) == body) {
      list_remove(state->timed_assets, i);
      asset_timer_free(asset_timer);
      break;
    }
  }
}

void destructive_timer_handler(state_t *state, asset_t *asset, void *aux) {
  annihlate_body(asset_get_body(asset), state);
}

list_t *make_rectangle(vector_t center, double width, double height) {
  list_t *points = list_init(RECT_NUM_POINTS, free);
  vector_t *p1 = malloc(sizeof(vector_t));
  *p1 = (vector_t){center.x - width / 2, center.y - height / 2};

  vector_t *p2 = malloc(sizeof(vector_t));
  *p2 = (vector_t){center.x + width / 2, center.y - height / 2};

  vector_t *p3 = malloc(sizeof(vector_t));
  *p3 = (vector_t){center.x + width / 2, center.y + height / 2};

  vector_t *p4 = malloc(sizeof(vector_t));
  *p4 = (vector_t){center.x - width / 2, center.y + height / 2};

  list_add(points, p1);
  list_add(points, p2);
  list_add(points, p3);
  list_add(points, p4);

  return points;
}

list_t *make_circle(double radius, size_t num_points, vector_t center) {
  list_t *points = list_init(num_points, free);
  for(size_t i = 0; i < num_points; i++) {
    double angle = 2 * M_PI * i/ num_points;
    vector_t *v = malloc(sizeof(*v));
    *v = (vector_t) {center.x + radius * cos(angle),
                    center.y + radius * sin(angle)};
    
    list_add(points, v);
  }

  return points;
}

body_t *make_heart(vector_t center, size_t len) {
  list_t *pts = make_rectangle(center, len, len);
  body_t *heart = body_init_with_info(pts, 1, white, make_type_info(HEART), free);
  return heart;
}

void render_health_bar(state_t *state) {
  double initx = INIT_HEART.x;
  size_t health = user_get_health(state->user);
  size_t n = (health / HEALTH_PER_HEART);
  if (health < HEALTH_PER_HEART && health > 0) {
    n = 1;
  }

  list_t *to_be_removed = list_init(1, free);
  size_t num_bodies = scene_bodies(state->scene);
  for (size_t i = 0; i < num_bodies; i++) {
    body_t *body = scene_get_body(state->scene, i);
    body_type_t type = get_type(body);
    if (type == HEART) {
      list_add(to_be_removed, body);
    }
  }

  ssize_t num_removed_hearts = list_size(to_be_removed);
  for (ssize_t i = num_removed_hearts - 1; i >= 0; i--) {
    annihlate_body(list_get(to_be_removed, i), state);
  }

  for (size_t i = 0; i < n; i++) {
     vector_t v = {.x=initx, .y=INIT_HEART.y};
     initx -= (HEART_OFFSET + (HEART_SIZE / 2.0));
     body_t *heart = make_heart(v, HEALTH_PER_HEART);
     list_add(state->game_body_assets, asset_make_image_with_body(HEART_PATH, 
              heart));
     scene_add_body(state->scene, heart);
  }
}

void render_username_input(state_t *state) {
  char *username = user_get_username(state->user);
  SDL_Rect username_bounding_box = {.w = strlen(username) * font_size, 
    .h = font_size * 2, .x = login.text_box.x, 
    .y = login.text_box.y - (font_size * TEXT_FACTOR)};
  SDL_Rect image_bounding_box = {.w = login.image_box.w, 
    .h = login.image_box.h, .x = login.image_box.x, 
    .y = login.image_box.y - (font_size * TEXT_FACTOR)};
  asset_t *username_asset = asset_make_text(font_path, username_bounding_box, 
    username, black);
  asset_t *image_asset = asset_make_image(RECTANGLE_PATH, image_bounding_box);
  list_add(state->login_body_assets, image_asset);
  list_add(state->login_body_assets, username_asset);
}

void add_score(state_t *state) {
  char *username = user_get_username(state->user);
  size_t user_score = user_get_score(state->user);

  score_t *score = score_init(username, user_score);
  user_add_score(state->user, score);
}

void make_walls(state_t *state) {
  list_t *wall1_shape =
      make_rectangle((vector_t){MAX.x+WALL_OFFSET, MAX.y / 2}, WALL_DIM, MAX.y);
  body_t *wall1 = body_init_with_info(wall1_shape, INFINITY, white,
                                      make_type_info(WALL), free);
  state->right_wall = wall1;

  list_t *wall2_shape =
      make_rectangle((vector_t){-WALL_OFFSET, MAX.y / 2}, WALL_DIM, MAX.y);
  body_t *wall2 = body_init_with_info(wall2_shape, INFINITY, white,
                                      make_type_info(WALL), free);
  state->left_wall = wall2;
  
  list_t *ceiling_shape =
      make_rectangle((vector_t){MAX.x / 2, MAX.y}, MAX.x, WALL_DIM);
  body_t *ceiling = body_init_with_info(ceiling_shape, INFINITY, white,
                                        make_type_info(WALL), free);
  state->ceiling = ceiling;
                                        
  list_t *ground_shape =
      make_rectangle((vector_t){MAX.x / 2, GROUND_HEIGHT}, MAX.x, WALL_DIM);
  body_t *ground = body_init_with_info(ground_shape, INFINITY, white,
                                       make_type_info(WALL), free);
  state->ground = ground;
                                       
  scene_add_body(state->scene, wall1);
  scene_add_body(state->scene, wall2);
  scene_add_body(state->scene, ceiling);
  scene_add_body(state->scene, ground);
}

void equip_handler(state_t *state, void *aux) {
  state->current_skin = ((size_t *) aux)[0];
  asset_t *new_skin = asset_make_image_with_body(SKIN_PATHS[state->current_skin], 
    user_get_body(state->user));
  user_set_skin(state->user, new_skin);
}

void buy_handler(state_t *state, void *aux) {
  size_t idx = ((size_t *) aux)[0];
  size_t current_score = db_entry_get_score(state->user_data);
  if (current_score >= PRICES[idx]) {
    db_entry_set_score(state->user_data, current_score - PRICES[idx]);
    db_entry_unlock_skin(state->user_data, idx);
    database_write(state->database);
  }
}

void make_store(state_t *state) {
  for (size_t i = 0; i < STORE_ROWS; i++) {
    for (size_t j = 0; j < STORE_COLUMNS; j++) {
      size_t skin_idx = i * STORE_COLUMNS + j;
      double x = (MAX.x / (double) (STORE_COLUMNS + 1)) * (double) (j + 1);

      double canvas_y = (MAX.y / (double) (STORE_ROWS + 1)) * (double) (i + 1);
      SDL_Rect canvas_bbox = (SDL_Rect) {x - (CANVAS_SIZE / 2.0), 
        canvas_y - (CANVAS_SIZE / 2.0), CANVAS_SIZE, CANVAS_SIZE};
      list_add(state->store_body_assets, asset_make_image(CANVAS_PATH, 
        canvas_bbox));

      SDL_Rect skin_bbox = (SDL_Rect) {x - (SKIN_WIDTH / 2.0), 
        canvas_y - (SKIN_HEIGHT / 2.0), SKIN_WIDTH, SKIN_HEIGHT};
      list_add(state->store_body_assets, asset_make_image(SKIN_PATHS[skin_idx],
       skin_bbox));

      double button_y = canvas_y + (CANVAS_SIZE / 2.0) + STORE_BUTTON_OFFSET;
      SDL_Rect text_bbox = (SDL_Rect) {x - (STORE_BUTTON_TEXT_W / 2.0), 
        button_y - (STORE_BUTTON_TEXT_H / 2.0), 
        STORE_BUTTON_TEXT_W, STORE_BUTTON_TEXT_H};
      SDL_Rect image_bbox = (SDL_Rect) {x - (STORE_BUTTON_W / 2.0), 
        button_y - (STORE_BUTTON_H / 2.0), STORE_BUTTON_W, STORE_BUTTON_H};
    
      asset_t *image_asset = asset_make_image(BLOCK_PATH, image_bbox);
      list_add(state->store_body_assets, image_asset);
      asset_t *text_asset = asset_make_text(font_path, text_bbox, EQUIPPED, 
        cyan);
      list_add(state->store_body_assets, text_asset);
      
      asset_t *equip_text = asset_make_text(font_path, text_bbox, EQUIP, black);
      asset_t *equip_image = asset_make_image(BLOCK_PATH, image_bbox);
      size_t *equip_index = malloc(sizeof(size_t));
      equip_index[0] = skin_idx;
      asset_t *equip_button_asset = asset_make_button(image_bbox, equip_image, 
        equip_text, (void *) equip_handler, equip_index);
      asset_cache_register_button(equip_button_asset);
      list_add(state->store_body_assets, equip_button_asset);

      char *price_str = malloc(sizeof(char) * (MAX_DIGITS + 
        strlen(PRICE_FORMAT) + 1));
      sprintf(price_str, PRICE_FORMAT, PRICES[skin_idx]);
      asset_t *buy_text = 
        asset_make_text(font_path, text_bbox, price_str, black);
      asset_t *buy_image = asset_make_image(BLOCK_PATH, image_bbox);
      size_t *buy_index = malloc(sizeof(size_t));
      buy_index[0] = skin_idx;
      asset_t *buy_button_asset = asset_make_button(image_bbox, buy_image, 
        buy_text, (void *) buy_handler, buy_index);
      asset_cache_register_button(buy_button_asset);
      list_add(state->store_body_assets, buy_button_asset);
      list_add(state->store_strings, price_str);
    }
  }
  list_add(state->store_body_assets, make_back_button());
}

void reset_store(state_t *state) {
  for (size_t i = 0; i < N_SKINS; i++) {
    size_t base_idx = i * ITEMS_PER_SKIN + FRAME_COUNT;
    asset_button_clear(list_get(state->store_body_assets, 
      base_idx + EQUIP_IDX_OFFSET));
    asset_button_clear(list_get(state->store_body_assets, 
      base_idx + BUY_IDX_OFFSET));
  }
}

void make_user(state_t *state) {
  state->current_skin = 0;
  list_t *upoints = make_circle(USER_RADIUS, CIRC_NUM_POINTS, USER_START_POS);
  body_t *user_body = body_init_with_info(upoints, USER_MASS, user_color, 
                                    make_type_info(USER), free);
  scene_add_body(state->scene, user_body);
  create_planet_gravity(state->scene, USER_GRAVITY, user_body);
  asset_t *default_skin = asset_make_image_with_body(
      SKIN_PATHS[state->current_skin], user_body);

  list_t *jpoints = make_rectangle(vec_add(USER_START_POS, JETPACK_OFFSET),
                                            JETPACK_WIDTH, JETPACK_HEIGHT);
  body_t *jetpack = body_init_with_info(jpoints, INFINITY, white, 
      make_type_info(USER), free);
  
  asset_t *jetpack_no_flame = asset_make_image_with_body(JETPACK_NO_FLAME_PATH,
   jetpack);
  asset_t *jetpack_flame = asset_make_image_with_body(JETPACK_FLAME_PATH,
   jetpack);
  user_t *user = user_init(user_body, default_skin, jetpack, jetpack_flame,
   jetpack_no_flame);  
                 
  state->user = user;
}

void leaderboard_handler(state_t *state, void *aux) {
  if (state->scene == state->home_scene) {
    state->leaderboard_body_assets = leaderboard_assets(
      user_get_leaderboard(state->user));
    list_add(state->leaderboard_body_assets, make_back_button());
    state->scene = state->leaderboard_scene;
    state->body_assets = state->leaderboard_body_assets;
  }
}

void play_handler(state_t *state, void *aux) {
  if (state->scene == state->home_scene) {
    state->scene = state->game_scene;
    state->body_assets = state->game_body_assets;
    Mix_HaltMusic();
    sdl_play_music(state->game_music);
  }
}

void login_handler(state_t *state, void *aux) {
  char *username = user_get_username(state->user);
  if (state->scene == state->login_scene 
    && strcmp(username, PLACE_HOLDER)) {
    sdl_set_text_input(false);
    
    state->user_data = database_get_user(state->database, username);

    state->scene = state->home_scene;
    state->body_assets = state->home_body_assets;
  }
}

void settings_handler(state_t *state, void *aux) {
  if (state->scene == state->home_scene) {
    state->scene = state->setting_scene;
    state->body_assets = state->setting_body_assets;
  }
}

void store_handler(state_t *state, void *aux) {
  if (state->scene == state->home_scene) {
    state->scene = state->store_scene;
    state->body_assets = state->store_body_assets;
  }
}

void reset_setting(state_t *state) {
  size_t num_elem = list_size(state->setting_body_assets);
  for (size_t i = 1; i < num_elem; i++) {
    asset_button_clear(list_get(state->setting_body_assets, i));
  }
}

void home_handler(state_t *state, void *aux) {
  state->scene = state->home_scene;
  state->body_assets = state->home_body_assets;
  reset_setting(state);
}

void yes_handler(state_t *state, void *aux) {
  state->respawn = true;
  home_handler(state, aux);
}

void no_handler(state_t *state, void *aux) {
  state->respawn = false;
  home_handler(state, aux);
}

void user_input_handler(char input, state_t *state) {
  char *username = user_get_username(state->user);
  if (strlen(username) < USERNAME_LEN) {
    if (((input >= ASCII_0) && (input <= (ASCII_9))) || 
      ((input >= ASCII_A) && (input <= ASCII_z))) {
      if (strcmp(username, (char*)PLACE_HOLDER) == 0) {
        strcpy(username, " "); // space for SDL
        username[1] = input;
        username[2] = '\0';
      }
      else {
        username[strlen(username) + 1] = '\0';
        username[strlen(username)] = input;
        user_set_username(state->user, username);
      }
    }
  }

  size_t username_len = strlen(username);
  if (input == '\b' && username_len >= 2 && strcmp(username, 
    (char *)PLACE_HOLDER) != 0) {
    username[username_len - 1] = '\0';
    if (strlen(username) == 1) {
      user_set_username(state->user, (char*)PLACE_HOLDER);
    }
  }
}

void logout_handler(state_t *state, void *aux) {
  if (state->scene == state->home_scene) {
    state->scene = state->login_scene;
    state->body_assets = state->login_body_assets;
    sdl_set_text_input(true);
    user_set_username(state->user, (char *)PLACE_HOLDER);

    state->current_skin = 0;
    asset_t *new_skin = asset_make_image_with_body(
        SKIN_PATHS[state->current_skin], user_get_body(state->user));
    user_set_skin(state->user, new_skin);

    state->respawn = false;

    reset_store(state);
  }
}

body_t *make_obstacle(state_t *state, vector_t dim, vector_t center, 
  body_type_t type) {
  list_t *c = list_init(RECT_NUM_POINTS, free);
  vector_t *v1 = malloc(sizeof(vector_t));
  assert(v1);
  *v1 = (vector_t){0, 0};
  list_add(c, v1);

  vector_t *v2 = malloc(sizeof(vector_t));
  assert(v2);
  *v2 = (vector_t){dim.x, 0};
  list_add(c, v2);

  vector_t *v3 = malloc(sizeof(vector_t));
  assert(v3);
  *v3 = (vector_t){dim.x, dim.y};
  list_add(c, v3);

  vector_t *v4 = malloc(sizeof(vector_t));
  assert(v4);
  *v4 = (vector_t){0, dim.y};
  list_add(c, v4);

  body_t *obstacle = body_init_with_info(c, 1, white, make_type_info(type), 
    free);
  body_set_centroid(obstacle, center);
  return obstacle;
}

void obstacle_out_handler(body_t *body1, body_t *body2, vector_t axis, 
  void *aux, double force_const) {
  state_t *state = aux;
  if (get_type(body1) == SINUSOIDAL) {remove_timed_asset(state, body1); }
  annihlate_body(body1, state);
}

void register_walls(state_t *state, body_t *body) {
  create_collision(state->scene, body, state->right_wall, obstacle_out_handler,
    (void *) state, 0);
  create_collision(state->scene, body, state->left_wall, obstacle_out_handler, 
    (void *) state, 0);
  create_collision(state->scene, body, state->ground, obstacle_out_handler, 
    (void *) state, 0);
  create_collision(state->scene, body, state->ceiling, obstacle_out_handler, 
    (void *) state, 0);
}

void obstacle_collision_handler(body_t *body1, body_t *body2, vector_t axis, 
  void *aux, double force_const) {
  state_t *state = aux;
  body_type_t obs_type = get_type(body1);
  if (obs_type == SINUSOIDAL) { remove_timed_asset(state, body1); }
  if (obs_type != LASER) { annihlate_body(body1, state); }

  user_apply_damage(state->user, force_const);
  render_health_bar(state);
  sdl_play_effect(state->collision_sound);

  if (user_get_health(state->user) <= 0) {
    asset_destroy(state->score_asset);
    state->score_asset = NULL;
    if (!state->respawn) {
      add_score(state);
      size_t current_total = db_entry_get_score(state->user_data);
      db_entry_set_score(state->user_data, current_total + 
        user_get_score(state->user));
      database_write(state->database);
      reset_game(body1, body2, axis, aux, force_const);
    }
    else if (state->respawn && !state->has_respawned) {
      respawn_game(state);
      state->has_respawned = true;
    } else {
      state->has_respawned = false;
      reset_game(body1, body2, axis, aux, force_const);
    }
  }
}

void rocket_field_handler(body_t *body1, body_t *body2, vector_t axis, 
  void *aux, double force_const) {
  state_t *state = aux;
  size_t asset_len = list_size(state->body_assets);
  for (size_t i = 0; i < asset_len; i++) {
    if (asset_get_body(list_get(state->body_assets, i)) == body1) {
      list_remove(state->body_assets, i);
      break;
    }
  }
  body_remove(body1);
}

void create_rocket_field_collision(state_t *state, body_t *rocket, 
  body_t *field) {
  sdl_play_effect(state->collision_sound);
  create_collision(state->scene, rocket, field, rocket_field_handler, 
    (void *)state, 0);
}

void register_force_field(state_t *state, body_t *body) {
  size_t num_bodies = scene_bodies(state->scene);
  for (size_t i = 0; i < num_bodies; i++) {
    body_t *temp = scene_get_body(state->scene, i);
    if (get_type(temp) == REBOUNDING) {
      if (get_type(body) != ROCKET) {
        create_physics_collision(state->scene, temp, body, 
          FORCE_FIELD_ELASTICITY);
        
      } else {
        create_rocket_field_collision(state, body, temp);
      }
      return;
    }
  }
}

void render_obstacle(state_t *state, vector_t dim, vector_t center, 
  vector_t velocity) {
  body_t *block = make_obstacle(state, dim, center, BLOCK);
  body_set_velocity(block, velocity); 
  
  scene_add_body(state->scene, block);
  
  list_add(state->body_assets,
              asset_make_image_with_body(BLOCK_PATH, block));

  create_collision(state->scene, block, user_get_body(state->user), 
    obstacle_collision_handler, (void *) state, BLOCK_DAMAGE);
  create_collision(state->scene, block, state->left_wall, obstacle_out_handler, 
    (void *) state, 0);

  register_force_field(state, block);
  register_walls(state, block);
}

void render_rocket(state_t *state, vector_t dim, vector_t center, 
  vector_t velocity) {
  body_t *rocket = make_obstacle(state, dim, center, ROCKET);

  body_set_velocity(rocket, velocity);
  
  scene_add_body(state->scene, rocket);
  
  list_add(state->body_assets,
              asset_make_image_with_body(ROCKET_PATH, rocket));

  register_force_field(state, rocket);
  create_collision(state->scene, rocket, user_get_body(state->user), 
    obstacle_collision_handler, (void *) state, ROCKET_DAMAGE);
  register_walls(state, rocket);
}

void update_rockets(state_t *state) {
 size_t num_bodies = scene_bodies(state->scene);
 for (size_t i = 0; i < num_bodies; i++) {
   body_t *obstacle = scene_get_body(state->scene, i);

   if (get_type(obstacle) == ROCKET) {
     body_t *user_body = user_get_body(state->user);
     vector_t user_center = body_get_centroid(user_body);
     vector_t obstacle_center = body_get_centroid(obstacle);

     if (get_distance(user_center, obstacle_center) < AIMING_THRESHOLD) {
       if (state->slowdown) {
        body_set_velocity(obstacle, vec_multiply(0.5, ROCKET_VEL));
       } else {
        body_set_velocity(obstacle, ROCKET_VEL);
       }
      
       break;
     }

     vector_t new_velo = vec_subtract(user_center, obstacle_center);
     double rocket_velo;

     if (state->slowdown) {
        rocket_velo = (ROCKET_VEL_MAG / 2.0);
     } else {
        rocket_velo = ROCKET_VEL_MAG;
     }

     double normalizing = (rocket_velo / (vec_get_length(new_velo)));
     body_set_velocity(obstacle, vec_multiply(normalizing, new_velo));
   }
 }
}

void velo_updater(state_t *state, asset_timer_t *timer) {
  body_t *body = asset_get_body(timer->asset);
  vector_t body_centroid = body_get_centroid(body);
  vector_t velocity = body_get_velocity(body);
  body_set_velocity(body, (vector_t){velocity.x, (SIN_FACTOR * 
    cos(FREQUENCY * body_centroid.x))});
}

void render_sin_obstacle(state_t *state, vector_t dim, vector_t velocity) {
  vector_t center = {.x = MAX.x, .y = MAX.y / 2};
  body_t *sin_obstacle = make_obstacle(state, dim, center, SINUSOIDAL);

  body_set_velocity(sin_obstacle, (vector_t){.x = velocity.x, 
    .y = velocity.x / VERTICAL_FACTOR});

  scene_add_body(state->scene, sin_obstacle);

  asset_t *sin_obstacle_asset = asset_make_image_with_body(BLOCK_PATH, 
    sin_obstacle);
  list_add(state->body_assets, sin_obstacle_asset);
  
  asset_timer_t *sin_timer = asset_timer_init(sin_obstacle_asset, DBL_MAX, 
    velo_updater, NULL, NULL);
  list_add(state->timed_assets, sin_timer);

  register_force_field(state, sin_obstacle);
  create_collision(state->scene, sin_obstacle, user_get_body(state->user), 
    obstacle_collision_handler, (void *)state, BLOCK_DAMAGE);
  create_collision(state->scene, sin_obstacle,  state->left_wall, 
    obstacle_out_handler, (void *)state, 0);
  register_walls(state, sin_obstacle);
}

void render_laser(state_t *state, double center_y) {
  vector_t center = (vector_t){MAX.x / 2, center_y};
  vector_t dim = (vector_t){MAX.x - LASER_OFFSET, LASER_HEIGHT};
  
  body_t *laser = make_obstacle(state, dim, center, LASER);

  scene_add_body(state->scene, laser);

  asset_t *laser_asset = asset_make_image_with_body(LASER_PATH, laser);
  
  list_add(state->body_assets, laser_asset);
  asset_timer_t *laser_timer = asset_timer_init(laser_asset, LASER_TIMER, NULL,
     destructive_timer_handler, NULL);
  list_add(state->timed_assets, laser_timer);

  create_collision(state->scene, laser, user_get_body(state->user), 
    obstacle_collision_handler, (void *)state, LASER_DAMAGE);
}

void laser_warning_tick_handler(state_t *state, asset_timer_t *timer) {
  if (timer->timer >= WARNING_LIMIT) {
    body_t *body = asset_get_body(timer->asset);
    render_laser(state, body_get_centroid(body).y);
    timer->tick_condition = false;
  } 
}

void render_laser_warning(state_t *state, double center_y) {
  vector_t dim = (vector_t){50, LASER_WARNING_HEIGHT};
  vector_t center_left = (vector_t){MIN.x + (dim.x / 2) - 1, center_y};
  vector_t center_right = (vector_t){MAX.x - (dim.x / 2) + 1, center_y};

  body_t *laser_left = make_obstacle(state, dim, center_left, LASER_LEFT);
  body_t *laser_right = make_obstacle(state, dim, center_right, LASER_RIGHT);

  scene_add_body(state->scene, laser_left);
  scene_add_body(state->scene, laser_right);

  asset_t *laser_left_asset = asset_make_image_with_body(LASER_LEFT_PATH, 
    laser_left);
  asset_t *laser_right_asset = asset_make_image_with_body(LASER_RIGHT_PATH, 
    laser_right);

  list_add(state->body_assets, laser_left_asset);
  list_add(state->body_assets, laser_right_asset);

  asset_timer_t *laser_left_timer = asset_timer_init(laser_left_asset, 
    LASER_TIMER + WARNING_LIMIT, laser_warning_tick_handler, 
    destructive_timer_handler, NULL);
  asset_timer_t *laser_right_timer = asset_timer_init(laser_right_asset, 
    LASER_TIMER + WARNING_LIMIT, NULL, destructive_timer_handler, NULL);

  list_add(state->timed_assets, laser_left_timer);
  list_add(state->timed_assets, laser_right_timer);

  sdl_play_effect(state->laser_sound);
}

void render_score(state_t *state) {
  size_t score = user_get_score(state->user);
  if (state->score_str != NULL) {
    free(state->score_str);
    state->score_str = NULL;
  }

  state->score_str = malloc(sizeof(char) * (strlen(SCORE_FORMAT) + 
    get_num_digits(score) + 1));
  sprintf(state->score_str, SCORE_FORMAT, score);
  SDL_Rect score_bounding_box = {.w = strlen(state->score_str) * 
    (font_size + 1), 
    .h = font_size * 2, .x = SCORE_LOC.x, 
    .y = SCORE_LOC.y - (font_size * TEXT_FACTOR)};

  if (state->score_asset != NULL) {
    asset_destroy(state->score_asset);
    state->score_asset = NULL;
  }

  state->score_asset = asset_make_text(font_path, score_bounding_box, 
    state->score_str, cyan);
}

void spawn_obstacles(state_t *state, double dt) {
  state->time_since_obstacle += dt;

  if (state->time_since_obstacle >= state->next_obstacle_time) {
    double p1 = OBSTACLE_PROB[0];
    double p2 = OBSTACLE_PROB[1] + p1;
    double p3 = OBSTACLE_PROB[2]+ p2;
    double p4 = OBSTACLE_PROB[3] + p3;
    double obstacle_p = rand_range(0.0, 1.0);
    
    if(obstacle_p < p1) {
      vector_t obstable_loc = (vector_t){MAX.x, 
        rand_range(GROUND_HEIGHT, MAX.y)};
      if (state->slowdown) {
        render_obstacle(state, OBSTACLE_DIM, obstable_loc, 
          vec_multiply(0.5, BASE_OBJ_VEL));
      } else {
        render_obstacle(state, OBSTACLE_DIM, obstable_loc, BASE_OBJ_VEL);
      }
    } else if (p1 <= obstacle_p && obstacle_p < p2) {
      vector_t rocket_loc = (vector_t){MAX.x, 
        rand_range(GROUND_HEIGHT, MAX.y)};
      if (state->slowdown) {
        render_rocket(state, ROCKET_DIM, rocket_loc, 
          vec_multiply(0.5, ROCKET_VEL));
      } else {
        render_rocket(state, ROCKET_DIM, rocket_loc, ROCKET_VEL);
      }
    } else if (p2 <= obstacle_p && obstacle_p < p3) {
      double laser_loc = rand_range(GROUND_HEIGHT, MAX.y);
      state->warning = true;
      render_laser_warning(state, laser_loc);
    } else if (p3 <= obstacle_p && obstacle_p < p4) {
      if (state->slowdown) {
        render_sin_obstacle(state, OBSTACLE_DIM, 
          vec_multiply(0.5, BASE_OBJ_VEL));
      } else {
        render_sin_obstacle(state, OBSTACLE_DIM, BASE_OBJ_VEL);
      }
    }

    state->time_since_obstacle = 0.0;
    state->next_obstacle_time = rand_range(MIN_OBS_SPAWN_INTERVAL, 
      MAX_OBS_SPAWN_INTERVAL);
  }
}

void powerup_timer_handler(state_t *state, asset_t *asset, void *aux) {
  powerup_type_t powerup_type = (powerup_type_t) aux;
  switch(powerup_type) {
    case MUFFLER:
      user_set_damage_multiplier(state->user, 1);
      break;
    case SCORE_DOUBLER:
      user_set_score_multiplier(state->user, 1);
      break;
    case SLOW_DOWN:
      speed_everything_up(state);
      state->slowdown = false;
    default:
      break;
  }
}

powerup_aux_t *powerup_aux_init(state_t *state, body_t *powerup, 
  powerup_type_t type) {
  powerup_aux_t *aux = malloc(sizeof(powerup_aux_t));
  assert(aux);

  aux->state = state;
  aux->powerup = powerup;
  aux->type = type;

  return aux;
}

void make_force_field(state_t *state) {
  list_t *field_pts = make_circle(FORCE_FIELD_RADIUS, CIRC_NUM_POINTS, 
    USER_START_POS);
  body_t *field_body = body_init_with_info(field_pts, INFINITY, user_color, 
                                    make_type_info(REBOUNDING), free);

  scene_add_body(state->scene, field_body);

  asset_t *field_asset = asset_make_image_with_body(FORCE_FIELD_PATH, 
    field_body);
  list_add(state->body_assets, field_asset);

  asset_timer_t *ff_timer = asset_timer_init(field_asset, FORCE_FIELD_TIME, 
    NULL, destructive_timer_handler, NULL);
  list_add(state->timed_assets, ff_timer);

  size_t num_bodies = scene_bodies(state->scene);
  for (size_t i = 0; i < num_bodies; i++) {
    body_t *temp = scene_get_body(state->scene, i);
    body_type_t obs_type = get_type(temp);
    if (obs_type == BLOCK || obs_type == SINUSOIDAL) {
      create_physics_collision(state->scene, field_body, temp, 
        FORCE_FIELD_ELASTICITY);
    } else if (obs_type == ROCKET) {
      create_rocket_field_collision(state, temp, field_body);
    }
  }
}

void slow_everything_down(state_t *state) {
  size_t num_bodies = scene_bodies(state->scene);
  for (size_t i = 0; i < num_bodies; i++) {
    body_t *body = scene_get_body(state->scene, i);
    body_type_t obs_type = get_type(body);
    if (obs_type == BLOCK || obs_type == SINUSOIDAL || 
      obs_type == ROCKET) {
      body_set_velocity(body, vec_multiply(0.5, body_get_velocity(body)));
    } 
  }
}

void speed_everything_up(state_t *state) {
  size_t num_bodies = scene_bodies(state->scene);
  for (size_t i = 0; i < num_bodies; i++) {
    body_t *body = scene_get_body(state->scene, i);
    body_type_t obs_type = get_type(body);
    if (obs_type == BLOCK || obs_type == SINUSOIDAL || obs_type == ROCKET) {
      body_set_velocity(body, vec_multiply(2, body_get_velocity(body)));
    } 
  }
}

void powerup_collision_handler(body_t *body1, body_t *body2, vector_t axis,
                               void *aux, double force_const) {
    powerup_aux_t *aux_info = (powerup_aux_t *)aux;
    state_t *state = aux_info->state;
    powerup_type_t type = aux_info->type;
    sdl_play_effect(state->powerup_sound);
    switch(type) {
    case HEALTH:
      user_set_health(state->user, FULL_HEALTH);
      render_health_bar(state);
      break;
    case MUFFLER:
      user_set_damage_multiplier(state->user, 0.5);
      asset_timer_t *muffler_timer = asset_timer_init(NULL, POWERUP_DURATION, 
        NULL, powerup_timer_handler, (void *) MUFFLER);
      list_add(state->timed_assets, muffler_timer);
      break;
    case SCORE_DOUBLER:
      user_set_score_multiplier(state->user, 2);
      asset_timer_t *doubler_timer = asset_timer_init(NULL, POWERUP_DURATION, 
        NULL, powerup_timer_handler, (void *) SCORE_DOUBLER);
      list_add(state->timed_assets, doubler_timer);
      break;
    case FORCE_FIELD:
      make_force_field(state);
      break;
    case SLOW_DOWN:
      slow_everything_down(state);
      state->slowdown = true;
      asset_timer_t *slow_down_timer = asset_timer_init(NULL, 
        POWERUP_DURATION, NULL, powerup_timer_handler, (void *) SLOW_DOWN);
      list_add(state->timed_assets, slow_down_timer);
  }
  
  annihlate_body(body2, state);
}

void create_powerup_collision(state_t *state, body_t *powerup, 
  powerup_type_t type) {
  powerup_aux_t *aux_info = powerup_aux_init(state, powerup, type);
  create_collision(state->scene, user_get_body(state->user), powerup, 
    powerup_collision_handler, aux_info, 0);
}

void make_powerup(state_t *state, double spawn_y, powerup_type_t powerup_type) {
  vector_t loc = (vector_t){MAX.x, spawn_y};
  list_t *pts = make_circle(POWERUP_RADIUS, CIRC_NUM_POINTS, loc);
  body_t *powerup = body_init_with_info(pts, 1, white, make_type_info(POWERUP), 
    free);
  scene_add_body(state->scene, powerup);
  if (state->slowdown) {
    body_set_velocity(powerup, vec_multiply(0.5, POWERUP_INIT_VEL));
  } else {
    body_set_velocity(powerup, POWERUP_INIT_VEL);
  }
  create_planet_gravity(state->scene, USER_GRAVITY, powerup);
  create_physics_collision(state->scene, state->ground, powerup, 
    FORCE_FIELD_ELASTICITY);
  create_collision(state->scene, powerup, state->left_wall, 
    obstacle_out_handler, (void *) state, 0);

  asset_t *asset;
  switch (powerup_type) {
    case HEALTH:
      asset = asset_make_image_with_body(HEALTH_POWER_PATH, powerup);
      break;
    case SCORE_DOUBLER:
      asset = asset_make_image_with_body(SCORE_DOUBLER_POWER_PATH, powerup);
      break;
    case MUFFLER:
      asset = asset_make_image_with_body(MUFFLER_POWER_PATH, powerup);
      break;
    case FORCE_FIELD:
      asset = asset_make_image_with_body(FORCE_ICON_PATH, powerup);
      break;
    case SLOW_DOWN:
      asset = asset_make_image_with_body(SLOW_DOWN_POWER_PATH, powerup);
      break;
    default:
      return;
  }

  list_add(state->body_assets, asset);
  create_powerup_collision(state, powerup, powerup_type);
}

void spawn_powerups(state_t *state, double dt) {
  state->time_since_powerup += dt;

  if (state->time_since_powerup >= state->next_powerup_time) {
    size_t random_choice = rand() % NUM_POWERUPS;

    make_powerup(state, rand_range(GROUND_HEIGHT + MIN_SPAWN_Y, MAX.y), 
      (powerup_type_t) random_choice);

    state->time_since_powerup = 0.0;
    state->next_powerup_time = rand_range(MIN_POW_SPAWN_INTERVAL,
      MAX_POW_SPAWN_INTERVAL);
  }
}

void reset_game(body_t *body1, body_t *body2, vector_t axis, void *aux,
                double force_const) {
  state_t *state = aux;
  
  state->scene = state->game_scene;
  state->body_assets = state->game_body_assets;

  Mix_HaltMusic();
  sdl_play_music(state->menu_music);

  size_t num_bodies = scene_bodies(state->scene);
  for (size_t i = 0; i < num_bodies; i++) {
    body_t *body = scene_get_body(state->scene, i);
    body_type_t t = get_type(body);
    if (t != USER && t != WALL) {
      body_remove(body);
    }
  }

  list_clear(state->timed_assets);
  list_clear(state->body_assets);

  body_set_centroid(user_get_body(state->user), USER_START_POS);
  body_reset(user_get_body(state->user));
  body_set_velocity(user_get_body(state->user), VEC_ZERO);
  user_set_health(state->user, FULL_HEALTH);
  
  user_set_score(state->user, 0);
  render_health_bar(state);
  
  state->time_since_obstacle = 0.0;
  state->next_obstacle_time = 0.0;

  state->time_since_powerup = 0.0;
  state->next_powerup_time = MIN_POW_SPAWN_INTERVAL;

  home_handler(state, NULL);
}

void expose_user(state_t *state, vector_t new_pos) {
  body_t *body = user_get_body(state->user);
  body_set_centroid(body, new_pos);
  user_set_damage_multiplier(state->user, 0);
  user_set_health(state->user, FULL_HEALTH);
  render_health_bar(state);

  state->transition = true;
}

void respawn_timer_handler(state_t *state, asset_t *asset, void *aux) {
  for (size_t i = 0; i < list_size(state->body_assets); i++) {
    if (list_get(state->body_assets, i) == asset) {
      list_remove(state->body_assets, i);
      break;
    }
  }
  asset_destroy(asset);
  expose_user(state, USER_START_POS);
}

void render_respawn(state_t *state) {
  SDL_Rect respawn_bounding_box = {.w = strlen(RESPAWN_MSG) * font_size, 
    .h = font_size * 2, .x = text_location.x, 
    .y = text_location.y - (font_size * 2)};
  asset_t *respawn_asset = asset_make_text(font_path, respawn_bounding_box, 
    RESPAWN_MSG, cyan);
  asset_timer_t *respawn_timer = asset_timer_init(respawn_asset, RESPAWN_DELAY, NULL, 
    respawn_timer_handler, NULL);
  list_add(state->timed_assets, respawn_timer);
  list_add(state->body_assets, respawn_asset);
}

void hide_user(state_t *state) {
  body_t *body = user_get_body(state->user);
  body_set_centroid(body, OFFSCREEN);
}

void immunity_effect(state_t *state, double dt) {
  state->buffer_time += dt;
  if (state->buffer_time > RESPAWN_DELAY) {
    user_set_damage_multiplier(state->user, 1);
    user_set_score_multiplier(state->user, 1);
    state->buffer_time = 0;
    state->transition = false;
  }
}

void respawn_game(state_t *state) {
  user_set_score_multiplier(state->user, 0);
  hide_user(state);
  render_respawn(state);
}

void check_bounds(state_t *state) {
  body_t *body = user_get_body(state->user);
  vector_t curr_centroid = body_get_centroid(body);
  if (curr_centroid.y + USER_RADIUS > (MAX.y)) {
    body_set_centroid(body, (vector_t){curr_centroid.x, 
      MAX.y - USER_RADIUS});
    body_set_velocity(body, VEC_ZERO);
  } else if (curr_centroid.y - USER_RADIUS < GROUND_HEIGHT) {
    body_set_centroid(body, (vector_t){curr_centroid.x, 
      USER_RADIUS + GROUND_HEIGHT});
    body_set_velocity(body, VEC_ZERO);
  }
}

void update_user(state_t *state) {
  vector_t user_centroid = body_get_centroid(user_get_body(state->user));
  vector_t new_jetpack_c = vec_add(user_centroid, JETPACK_OFFSET);

  body_set_centroid(user_get_jetpack(state->user), new_jetpack_c);
  if (user_get_jetpack_status(state->user)) {
    asset_render(user_get_jetpack_flame(state->user));
  } else {
    asset_render(user_get_jetpack_no_flame(state->user));
  }
  
  asset_render(user_get_skin(state->user));
  user_score_tick(state->user);
  render_score(state);
  check_bounds(state);
}

void make_game(state_t *state) {
  make_user(state);
  make_walls(state);
  render_health_bar(state);
}

void tick_timers(state_t *state, double dt) {
  for(size_t i = 0; i < list_size(state->timed_assets); i++) {
    asset_timer_t *asset_timer = list_get(state->timed_assets, i);
    if (asset_timer->expired) {
      list_remove(state->timed_assets, i);
      asset_timer_free(asset_timer);
      i--;
    } else {
      asset_timer_tick(asset_timer, state, dt);
    }
  }
}

void render_store(state_t *state) {
  char *display_str = malloc(sizeof(char) * (MAX_DIGITS + strlen(SCORE_FORMAT) +
    1));
  sprintf(display_str, SCORE_FORMAT, db_entry_get_score(state->user_data));
  list_add(state->store_body_assets, asset_make_text(font_path, SCORE_BBOX, 
    display_str, black));
  list_add(state->store_strings, display_str);

  for (size_t i = 0; i < N_SKINS; i++) {
    size_t base_idx = i * ITEMS_PER_SKIN;
    for (size_t j = 0; j < FRAME_COUNT; j++) {
      asset_t *asset = list_get(state->store_body_assets, base_idx + j);
      asset_render(asset);
    }
    
    if (i == state->current_skin) {
      asset_t *image = list_get(state->store_body_assets, base_idx + 
        FRAME_COUNT);
      asset_t *text = list_get(state->store_body_assets, base_idx + 
        FRAME_COUNT + 1);
      asset_render(image);
      asset_render(text);
    } else if (db_entry_has_skin(state->user_data, i)) {
      asset_render(list_get(state->store_body_assets, base_idx + FRAME_COUNT + 
        EQUIP_IDX_OFFSET));
    } else {
      asset_render(list_get(state->store_body_assets, base_idx + FRAME_COUNT + 
        BUY_IDX_OFFSET));
    }
  }

  size_t num_assets = list_size(state->store_body_assets);
  for (size_t i = N_SKINS * ITEMS_PER_SKIN; i < num_assets; i++) {
    asset_render(list_get(state->store_body_assets, i));
  }

  asset_destroy(list_remove(state->store_body_assets, num_assets - 1));
}

/**
 * Move player on display screen based on key pressed.
 *
 * @param key the character of the key pressed
 * @param type event type connected to key
 * @param held_time double value representing the amount of time the key is held
 * down
 * @param state the state representing the current demo
 */
void on_key(char key, key_event_type_t type, double held_time, state_t *state) {
  if (type == KEY_PRESSED && key == SPACE_BAR) {
    sdl_play_effect(state->jetpack_sound);
    user_set_jetpack_status(state->user, true);
    body_add_impulse(user_get_body(state->user), JETPACK_IMPULSE);
  } else if (type == KEY_RELEASED) {
    user_set_jetpack_status(state->user, false);
    body_reset(user_get_body(state->user));
  }
}

state_t *emscripten_init() {
  sdl_set_text_input(true);
  asset_cache_init();
  sdl_init(MIN, MAX);
  sdl_on_key((void *)on_key);

  state_t *state = malloc(sizeof(state_t));

  state->time_since_obstacle = 0.0;
  state->next_obstacle_time = 0.0;
  state->buffer_time = 0.0;

  state->time_since_powerup = 0.0;
  state->next_powerup_time = MIN_POW_SPAWN_INTERVAL;
  state->respawn = false;
  state->score_asset = NULL;
  state->score_str = NULL;

  
  state->leaderboard_scene = scene_init();
  state->game_scene = scene_init();
  state->home_scene = scene_init();
  state->setting_scene = scene_init();
  state->store_scene = scene_init();
  state->login_scene = scene_init();

  state->transition = false;
  state->slowdown = false;
  srand(time(NULL));

  state->home_body_assets = home_button_assets();

  state->login_body_assets = login_button_assets();

  state->setting_body_assets = settings_button_assets();

  state->store_strings = list_init(1, (free_func_t)free);
  state->store_body_assets = list_init(1, (free_func_t)asset_destroy);

  state->game_body_assets = list_init(1, (free_func_t)asset_destroy);
  state->timed_assets = list_init(1, (free_func_t)asset_timer_free);

  state->scene = state->game_scene;
  state->body_assets = state->game_body_assets;

  make_game(state);

  SDL_Rect background_box = {
      .x = MIN.x, .y = MIN.y, .w = MAX.x - MIN.x, .h = MAX.y - MIN.y};
  state->background = asset_make_image(BACKGROUND_PATH, background_box);

  state->menu_music = sdl_load_music(MENU_MUSIC_PATH);
  state->game_music = sdl_load_music(GAME_MUSIC_PATH);

  state->jetpack_sound = sdl_load_wav(JETPACK_SOUND_PATH);
  state->collision_sound = sdl_load_wav(COLLISION_SOUND_PATH);
  state->laser_sound = sdl_load_wav(LASER_SOUND_PATH);
  state->powerup_sound = sdl_load_wav(POWRUP_SOUND_PATH);
  sdl_play_music(state->menu_music);

  state->database = database_read();
  state->has_respawned = false;

  make_store(state);
  
  state->scene = state->login_scene;
  state->body_assets = state->login_body_assets;
  return state;
}

bool emscripten_main(state_t *state) {
  double dt = time_since_last_tick();
  if (state->scene == state->login_scene) {
    sdl_on_input((void *)user_input_handler);
    render_username_input(state);
  }

  scene_tick(state->scene, dt);
  if (state->scene == state->game_scene) {
    tick_timers(state, dt);
  }
  sdl_clear();

  asset_render(state->background);
  if (state->body_assets != state->store_body_assets) {
    for (size_t i = 0; i < list_size(state->body_assets); i++) {
      asset_render(list_get(state->body_assets, i));
    }
  } else {
    render_store(state);
  }
  if (state->score_asset != NULL) {
    asset_render(state->score_asset);
  }

  if (state->scene == state->game_scene) {
    spawn_obstacles(state, dt);
    update_user(state);
    if (state->transition) {
      immunity_effect(state, dt);
    } else {
      spawn_powerups(state, dt);
      
      if (user_get_score(state->user) > ROCKET_THRESHOLD) {
        update_rockets(state);
      }
    }
  }
  sdl_show();

  return false;
}

void emscripten_free(state_t *state) {
  list_free(state->timed_assets);

  list_free(state->home_body_assets);
  list_free(state->game_body_assets);
  list_free(state->leaderboard_body_assets);
  list_free(state->setting_body_assets);
  list_free(state->store_body_assets);
  list_free(state->login_body_assets);

  scene_free(state->home_scene);
  scene_free(state->game_scene);
  scene_free(state->leaderboard_scene);
  scene_free(state->setting_scene);
  scene_free(state->store_scene);
  scene_free(state->login_scene);

  user_free(state->user);

  database_write(state->database);
  list_free(state->database);

  list_free(state->store_strings);

  body_free(state->left_wall);
  body_free(state->right_wall);
  body_free(state->ceiling);
  body_free(state->ground);

  asset_destroy(state->background);

  asset_destroy(state->score_asset);
  if (state->score_str != NULL) { free(state->score_str); }
  asset_cache_destroy();

  Mix_HaltMusic();
	Mix_FreeMusic(state->menu_music);
  Mix_FreeMusic(state->game_music);
  Mix_FreeChunk(state->jetpack_sound);
  Mix_FreeChunk(state->collision_sound);
  Mix_FreeChunk(state->laser_sound);
  Mix_FreeChunk(state->powerup_sound);
	Mix_CloseAudio();

  free(state); 
}