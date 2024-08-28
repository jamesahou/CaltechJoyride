#include "sdl_wrapper.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>

#include "asset_cache.h"

const char WINDOW_TITLE[] = "CS 3";
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 500;
const double MS_PER_S = 1e3;
const double AVERAGING_FACTOR = 0.5;
const int DRIVER_INDEX = -1;
const size_t MIN_POLY_SIDES = 3;
const size_t SCALING_FACTOR = 2;
const double TEXT_SCALING_FACTOR = 10;
const double HEIGHT_FACTOR = 0.17;
bool TEXT_INPUT = false;

/**
 * The coordinate at the center of the screen.
 */
vector_t center;
/**
 * The coordinate difference from the center to the top right corner.
 */
vector_t max_diff;
/**
 * The SDL window where the scene is rendered.
 */
SDL_Window *window;
/**
 * The renderer used to draw the scene.
 */
SDL_Renderer *renderer;
/**
 * The keypress handler, or NULL if none has been configured.
 */
key_handler_t key_handler = NULL;
/**
 * The text input handler, or NULL if none has been configured.
 */
text_input_handler_t text_input_handler = NULL;
/**
 * SDL's timestamp when a key was last pressed or released.
 * Used to mesasure how long a key has been held.
 */
uint32_t key_start_timestamp;
/**
 * The value of clock() when time_since_last_tick() was last called.
 * Initially 0.
 */
clock_t last_clock = 0;

/** Computes the center of the window in pixel coordinates */
vector_t get_window_center(void) {
  int *width = malloc(sizeof(*width)), *height = malloc(sizeof(*height));
  assert(width != NULL);
  assert(height != NULL);
  SDL_GetWindowSize(window, width, height);
  vector_t dimensions = {.x = *width, .y = *height};
  free(width);
  free(height);
  return vec_multiply(0.5, dimensions);
}

/**
 * Computes the scaling factor between scene coordinates and pixel coordinates.
 * The scene is scaled by the same factor in the x and y dimensions,
 * chosen to maximize the size of the scene while keeping it in the window.
 */
double get_scene_scale(vector_t window_center) {
  // Scale scene so it fits entirely in the window
  double x_scale = window_center.x / max_diff.x,
         y_scale = window_center.y / max_diff.y;
  return x_scale < y_scale ? x_scale : y_scale;
}

void sdl_set_text_input(bool val) {TEXT_INPUT = val; }

/** Maps a scene coordinate to a window coordinate */
vector_t get_window_position(vector_t scene_pos, vector_t window_center) {
  // Scale scene coordinates by the scaling factor
  // and map the center of the scene to the center of the window
  vector_t scene_center_offset = vec_subtract(scene_pos, center);
  double scale = get_scene_scale(window_center);
  vector_t pixel_center_offset = vec_multiply(scale, scene_center_offset);
  vector_t pixel = {.x = round(window_center.x + pixel_center_offset.x),
                    // Flip y axis since positive y is down on the screen
                    .y = round(window_center.y - pixel_center_offset.y)};
  return pixel;
}

/**
 * Converts an SDL key code to a char.
 * 7-bit ASCII characters are just returned
 * and arrow keys are given special character codes.
 */
char get_keycode(SDL_Keycode key) {
  switch (key) {
  case SDLK_LEFT:
    return LEFT_ARROW;
  case SDLK_UP:
    return UP_ARROW;
  case SDLK_RIGHT:
    return RIGHT_ARROW;
  case SDLK_DOWN:
    return DOWN_ARROW;
  case SDLK_SPACE:
    return SPACE_BAR;
  default:
    // Only process 7-bit ASCII characters
    return key == (SDL_Keycode)(char)key ? key : '\0';
  }
}

void sdl_init(vector_t min, vector_t max) {
  // Check parameters
  assert(min.x < max.x);
  assert(min.y < max.y);

  center = vec_multiply(0.5, vec_add(min, max));
  max_diff = vec_subtract(max, center);
  SDL_Init(SDL_INIT_EVERYTHING);
  window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT,
                            SDL_WINDOW_RESIZABLE);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

  assert(Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) != -1);
  
  TTF_Init();
}

bool sdl_is_done(void *state) {
  SDL_Event *event = malloc(sizeof(*event));
  assert(event != NULL);
  while (SDL_PollEvent(event)) {
    switch (event->type) {
    case SDL_MOUSEBUTTONUP: {
      SDL_MouseButtonEvent *mouse_click = (SDL_MouseButtonEvent *)event;
      asset_cache_handle_buttons(state, mouse_click->x, mouse_click->y);
      break;
    }
    case SDL_QUIT:
      free(event);
      return true;
    case SDL_KEYDOWN: {
      if (TEXT_INPUT) {
        char c = event->key.keysym.sym;
        text_input_handler(c, state);
        break;
      }
    }
    case SDL_KEYUP:
      // Skip the keypress if no handler is configured
      // or an unrecognized key was pressed
      if (key_handler == NULL)
        break;
      char key = get_keycode(event->key.keysym.sym);
      if (key == '\0')
        break;

      uint32_t timestamp = event->key.timestamp;
      if (!event->key.repeat) {
        key_start_timestamp = timestamp;
      }
      key_event_type_t type =
          event->type == SDL_KEYDOWN ? KEY_PRESSED : KEY_RELEASED;
      double held_time = (timestamp - key_start_timestamp) / MS_PER_S;
      key_handler(key, type, held_time, state);
      break;
    }
  }
  free(event);
  return false;
}

void sdl_clear(void) {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);
}

void sdl_draw_polygon(polygon_t *poly, rgb_color_t color) {
  list_t *points = polygon_get_points(poly);
  // Check parameters
  size_t n = list_size(points);
  assert(n >= 3);

  vector_t window_center = get_window_center();

  // Convert each vertex to a point on screen
  int16_t *x_points = malloc(sizeof(*x_points) * n),
          *y_points = malloc(sizeof(*y_points) * n);
  assert(x_points != NULL);
  assert(y_points != NULL);
  for (size_t i = 0; i < n; i++) {
    vector_t *vertex = list_get(points, i);
    vector_t pixel = get_window_position(*vertex, window_center);
    x_points[i] = pixel.x;
    y_points[i] = pixel.y;
  }

  // Draw polygon with the given color
  filledPolygonRGBA(renderer, x_points, y_points, n, color.r * 255,
                    color.g * 255, color.b * 255, 255);
  free(x_points);
  free(y_points);
}

void sdl_show(void) {
  // Draw boundary lines
  vector_t window_center = get_window_center();
  vector_t max = vec_add(center, max_diff),
           min = vec_subtract(center, max_diff);
  vector_t max_pixel = get_window_position(max, window_center),
           min_pixel = get_window_position(min, window_center);
  SDL_Rect *boundary = malloc(sizeof(*boundary));
  boundary->x = min_pixel.x;
  boundary->y = max_pixel.y;
  boundary->w = max_pixel.x - min_pixel.x;
  boundary->h = min_pixel.y - max_pixel.y;
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderDrawRect(renderer, boundary);
  free(boundary);

  SDL_RenderPresent(renderer);
}

SDL_Texture *sdl_load_image(const char *img_path) {
  SDL_Texture *img = IMG_LoadTexture(renderer, img_path);
  assert(img != NULL);
  return img;
}

void sdl_draw_image(SDL_Texture *texture, size_t width, size_t height,
                    vector_t pos) {
  // from https://gist.github.com/Nico-Adamo/22f5040d004b9e64dfeb761e1eace5e6
  SDL_Rect *texrec = malloc(sizeof(SDL_Rect));
  assert(texrec != NULL);

  texrec->x = pos.x;
  texrec->y = pos.y;
  texrec->w = width;
  texrec->h = height;

  SDL_RenderCopy(renderer, texture, NULL, texrec);
  free(texrec);
}

void sdl_draw_rotating_image(SDL_Texture *texture, size_t width, size_t height,
                    vector_t pos, double angle, vector_t center) {
  // from https://gist.github.com/Nico-Adamo/22f5040d004b9e64dfeb761e1eace5e6
  SDL_Rect *texrec = malloc(sizeof(SDL_Rect));
  assert(texrec != NULL);

  texrec->x = pos.x;
  texrec->y = pos.y;
  texrec->w = width;
  texrec->h = height;

  SDL_Point *c = malloc(sizeof(SDL_Point));
  assert(c);

  c->x = center.x;
  c->y = center.y;

  SDL_RenderCopyEx(renderer, texture, NULL, texrec, angle, c, SDL_FLIP_NONE);
  free(texrec);
  free(c);
}

Mix_Chunk *sdl_load_wav(const char *wav_path) {
  Mix_Chunk *wave = Mix_LoadWAV(wav_path);
  assert(wave != NULL);
  return wave;
}

Mix_Music *sdl_load_music(const char *music_path) {
  Mix_Music *music = Mix_LoadMUS(music_path);
  assert(music != NULL);
  return music;
}

void sdl_play_effect(Mix_Chunk* wav) {
  Mix_PlayChannel(-1, wav, 0);
}

void sdl_play_music(Mix_Music* music) {
  Mix_PlayMusic( music, -1);
}


TTF_Font *sdl_load_font(const char *font_path, size_t point_size) {
  TTF_Font *font = TTF_OpenFont(font_path, point_size);
  assert(font != NULL);
  return font;
}

void sdl_draw_text(const char *message, TTF_Font *font, size_t size,
                   vector_t pos, rgb_color_t color) {
  // taken from https://gist.github.com/emayecs/196b9ca61d95316f36c2d0025f3875ad
  SDL_Color sdl_color = {color.r, color.g, color.b};
  SDL_Surface *surface = TTF_RenderText_Solid(font, message, sdl_color);
  assert(surface != NULL);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  assert(texture != NULL);
  SDL_Rect *texrec = malloc(sizeof(SDL_Rect));
  assert(texrec != NULL);

  texrec->x = pos.x;
  texrec->y = pos.y;
  texrec->w = strlen(message) * TEXT_SCALING_FACTOR;
  texrec->h = size * TEXT_SCALING_FACTOR * HEIGHT_FACTOR;

  SDL_RenderCopy(renderer, texture, NULL, texrec);
  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
  free(texrec);
}

void sdl_render_scene(scene_t *scene, void *aux) {
  sdl_clear();
  size_t body_count = scene_bodies(scene);
  for (size_t i = 0; i < body_count; i++) {
    body_t *body = scene_get_body(scene, i);
    list_t *shape = body_get_shape(body);
    polygon_t *poly = polygon_init(shape, (vector_t){0, 0}, 0, 0, 0, 0);
    sdl_draw_polygon(poly, *body_get_color(body));
    list_free(shape);
  }
  if (aux != NULL) {
    body_t *body = aux;
    sdl_draw_polygon(body_get_polygon(body), *body_get_color(body));
  }
  sdl_show();
}

void sdl_on_key(key_handler_t handler) { key_handler = handler; }

void sdl_on_input(text_input_handler_t handler) { text_input_handler = handler; }

double time_since_last_tick(void) {
  clock_t now = clock();
  double difference = last_clock
                          ? (double)(now - last_clock) / CLOCKS_PER_SEC
                          : 0.0; // return 0 the first time this is called
  last_clock = now;
  return difference;
}

SDL_Rect sdl_get_bounding_box(body_t *body) {
  list_t *shapes = body_get_shape(body);
  vector_t top_left = {.x = __DBL_MAX__, .y = -__DBL_MAX__};
  vector_t bottom_right = {.x = -__DBL_MAX__, .y = __DBL_MAX__};
  for (size_t i = 0; i < list_size(shapes); i++) {
    vector_t *v = list_get(shapes, i);
    if (v->x < top_left.x) {
      top_left.x = v->x;
    } else if (v->x > bottom_right.x) {
      bottom_right.x = v->x;
    }
    if (v->y > top_left.y) {
      top_left.y = v->y;
    } else if (v->y < bottom_right.y) {
      bottom_right.y = v->y;
    }
  }
  
  vector_t center = get_window_center();

  vector_t pixel_top = get_window_position(top_left, center);
  vector_t pixel_bottom = get_window_position(bottom_right, center);

  int width = pixel_bottom.x - pixel_top.x;
  int height = pixel_bottom.y - pixel_top.y;

  SDL_Rect box = {.x = pixel_top.x, .y = pixel_top.y, .w = width, .h = height};

  list_free(shapes);
  return box;
}