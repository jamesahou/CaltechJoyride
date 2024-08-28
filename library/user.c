#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "body.h"
#include "user.h"

static const char *PLACE_HOLDER = " USERNAME"; // space for SDL
static const size_t USERNAME_LEN = 15;
const size_t NUM_POWERUP = 4;
const size_t TOP_FIVE = 5;
const size_t FULL_HEALTH = 100;


typedef struct user {
  body_t *body;
  asset_t *skin;
  body_t *jetpack;
  double score_multiplier;
  double damage_multiplier;
  size_t health;
  size_t score;
  list_t *leaderboard;
  asset_t *jetpack_flame;
  asset_t *jetpack_no_flame;
  char *username;
  bool jetpack_on;
} user_t;

user_t *user_init(body_t *body, asset_t *skin, body_t *jetpack, asset_t *jetpack_flame, asset_t *jetpack_no_flame) {
    user_t *user = malloc(sizeof(user_t));
    assert(user);

    user->body = body;
    user->skin = skin;
    user->jetpack = jetpack;
    user->score_multiplier = 1;
    user->damage_multiplier = 1;

    user->health = FULL_HEALTH;
    user->score = 0;
    user->jetpack_flame = jetpack_flame;
    user->jetpack_no_flame = jetpack_no_flame;
    user->jetpack_on = false;
    user->username = malloc(sizeof(char) * (USERNAME_LEN + 2));
    user->leaderboard = list_init(TOP_FIVE, (free_func_t)score_free);
    strcpy(user->username, PLACE_HOLDER);

    return user;
}

body_t *user_get_body(user_t *user) {
  return user->body;
}

asset_t *user_get_skin(user_t *user) {
  return user->skin;
}

void user_set_skin(user_t *user, asset_t *skin) {
  if (user->skin != NULL) {
    asset_destroy(user->skin);
  }
  user->skin = skin;
}

body_t *user_get_jetpack(user_t *user) {
  return user->jetpack;
}

void user_add_score(user_t *user, score_t *score) {
  list_add(user->leaderboard, score);
  list_sort(user->leaderboard, (__compar_fn_t)score_compare);
  if (list_size(user->leaderboard) > TOP_FIVE) {
    list_remove(user->leaderboard, list_size(user->leaderboard) - 1);
  }
}

list_t *user_get_leaderboard(user_t *user) {
  return user->leaderboard;
}

double user_get_score_multiplier(user_t *user) {
  return user->score_multiplier;
}

void user_set_score_multiplier(user_t *user, double score_multiplier) {
  user->score_multiplier = score_multiplier;
}
 
double user_get_damage_multiplier(user_t *user) {
  return user->damage_multiplier;
}

void user_set_damage_multiplier(user_t *user, double damage_multiplier) {
  user->damage_multiplier = damage_multiplier;
}

char *user_get_username(user_t *user) {
  return user->username;
}

void user_set_username(user_t *user, char *username) { strcpy(user->username, username); }

size_t user_get_health(user_t *user) {
  return user->health;
}

void user_set_health(user_t *user, size_t health) {
  user->health = health;
}

size_t user_score(user_t *user) {
  return user->score;
}

asset_t *user_get_jetpack_flame(user_t *user) {
  return user->jetpack_flame;
}

asset_t *user_get_jetpack_no_flame(user_t *user) {
  return user->jetpack_no_flame;
}

void user_set_jetpack_status(user_t *user, bool status) {
  user->jetpack_on = status;
}

bool user_get_jetpack_status(user_t *user) {
  return user->jetpack_on;
}

size_t user_get_score(user_t *user) {
  return user->score;
}

void user_set_score(user_t *user, size_t score) {
  user->score = score;
}

void user_score_tick(user_t *user) {
  user_set_score(user, user_get_score(user) + (user_get_score_multiplier(user)));
}

void user_apply_damage(user_t *user, size_t damage) {
  size_t current_health = user_get_health(user);
  ssize_t new_health = current_health - (user_get_damage_multiplier(user) * damage);
  if (new_health < 0) {
    new_health = 0;
  }
  user_set_health(user, new_health);
}

void user_free(user_t *user) {
  free(user->username);
  free(user);
}