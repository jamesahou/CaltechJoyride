#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <SDL_render.h>

#include "asset.h"
#include "asset_cache.h"
#include "collision.h"
#include "forces.h"
#include "sdl_wrapper.h"

const char *BLOCK_PATH = "assets/block.png";
const char *ROCKET_PATH = "assets/rocket.png";
const char *ROTATING_OBSTACLE_PATH = "assets/block.png";
const char *LASER_PATH = "assets/laser.png";

const size_t LASER_HEIGHT = 18;
const double LASER_TIMER = 5.0;

const rgb_color_t obs_color = (rgb_color_t){0, 0, 0};

body_t *make_obstacle(state_t *state, vector_t dim, vector_t center) {
  list_t *c = list_init(4, free);
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

  body_t *obstacle = body_init(c, 1, obs_color);
  body_set_centroid(obstacle, center);
  return obstacle;
}

void render_obstacle(state_t *state, vector_t dim, vector_t center, vector_t velocity) {
  body_t *block = make_obstacle(state, dim, center);
  body_set_velocity(block, velocity);
  
  scene_add_body(state->scene, block);
  
  list_add(state->body_assets,
              asset_make_image_with_body(BLOCK_PATH, block));
}

void render_rocket(state_t *state, vector_t dim, vector_t center, vector_t velocity) {
  body_t *rocket = make_obstacle(state, dim, center);

  body_set_velocity(rocket, velocity);
  
  scene_add_body(state->scene, rocket);
  
  list_add(state->body_assets,
              asset_make_image_with_body(ROCKET_PATH, rocket));
}

void render_rotating_obstacle(state_t *state, vector_t dim, vector_t center, vector_t velocity) {
  body_t *rotating_obstacle = make_obstacle(state, dim, center);

  body_set_velocity(rotating_obstacle, velocity);

  body_set_rotation(rotating_obstacle, 1);

  scene_add_body(state->scene, rotating_obstacle);

  list_add(state->body_assets, 
              asset_make_image_with_body(ROTATING_OBSTACLE_PATH, rotating_obstacle));
}

void render_laser(state_t *state, size_t center_y) {
  vector_t center = (vector_t){500, center_y};
  vector_t dim = (vector_t){MAX.x, LASER_HEIGHT};

  body_t *laser = make_obstacle(state, dim, center);
  laser->is_laser = true;
  laser->info = (double) 0.0;

  scene_add_body(state->scene, laser);

  list_add(state->body_assets, 
              asset_make_image_with_body(LASER_PATH, laser));
}

void is_out_of_bound(state_t *state, size_t idx, double dt) {
  body_t *body = scene_get_body(state->scene, idx);
  if (body_get_centroid(body).x < 0) {
        scene_remove_body(state->scene, idx);
        list_remove(state->body_assets, idx); 
  }
  else {
    if (scene_get_body(state->scene, idx).is_laser) {
        body->info += dt;
        if (body->info >= LASER_TIMER) {
             scene_remove_body(state->scene, idx);
             list_remove(state->body_assets, idx);
        }
    }  
    asset_render(list_get(state->body_assets, idx));
  }
}

bool check_hit(state_t *state); 