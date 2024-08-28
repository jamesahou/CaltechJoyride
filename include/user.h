#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "asset.h"
#include "asset_cache.h"
#include "collision.h"
#include "forces.h"
#include "leaderboard.h"
#include "sdl_wrapper.h"

typedef struct user user_t;

/**
 * Function to initialize the user 
 * @param body the body of the user
 * @param skin the skin of the user
 * @param jetpack body of the user's jetpack
 * @param jetpack_flame asset that represents when the jetpack is on
 * @param jetpack_no_flame asset that represents when the jetpack is off
 * @return a pointer to the newly allocated user
 */
user_t *user_init(body_t *body, asset_t *skin, body_t *jetpack, asset_t *jetpack_flame, asset_t *jetpack_no_flame);

/**
 * Function that gets the user's body
 * @param user the current user in the game
 * @return a pointer the user's body
 */
body_t *user_get_body(user_t *user);

asset_t *user_get_skin(user_t *user);

void user_set_skin(user_t *user, asset_t *skin);

/**
 * Function that gets the user's jetpack
 * @param user the current user in the game
 * @return a pointer to the user's jetpack
 */
body_t *user_get_jetpack(user_t *user);

/**
 * Function that gets the user's score multiplier
 * This is the multiple by which the score will increase by
 * @param user the current user in the game
 * @return the user's current score multiplier
 */
double user_get_score_multiplier(user_t *user);

/**
 * Function that sets the user's score multiplier
 * This is the multiple by which the score will increase by
 * @param user the current user in the game
 * @param score_multiplier the user's new score multiplier
 */
void user_set_score_multiplier(user_t *user, double score_multiplier);
 
 /**
  * Function that gets the user's damage multiplier
  * This is the multiple by which the user's health will decrease by
  * @param user the current user in the game
  * @return the user's current damage multiplier
  */
double user_get_damage_multiplier(user_t *user);

/**
 * Function that sets the user's damage multplier
 * This is the multiple by which the user's health will decrease by
 * @param user the current user in the game
 * @param damage_multiplier the user's new damage multiplier
 */
void user_set_damage_multiplier(user_t *user, double damage_multiplier);

/**
 * Function that gets the user's health
 * @param user the current user in the game
 * @return the current user's health
 */
size_t user_get_health(user_t *user);

/**
 * Function that sets the user's health
 * @param user the current user in the game
 * @param health the user's new health
 */
void user_set_health(user_t *user, size_t health);

/**
 * Function that gets the user's score
 * @param user the current user in the game
 * @return the user's current health
 */
size_t user_score(user_t *user);

/**
 * Function that adds the current user's score to a leaderboard
 * @param user the current user in the game
 * @param score the score corresponding to the user
 */
void user_add_score(user_t *user, score_t *score);

/**
 * Function that gets the user's leaderboard
 * @param user the current user in the game
 * @return a list containing the top 5 scores of the game
 */
list_t *user_get_leaderboard(user_t *user);

/**
 * Function that gets the user's username
 * @param user the current user in the game
 * @return the users username
 */
char *user_get_username(user_t *user);

/**
 * Function that sets the user's username
 * @param user the current user in the game
 * @param username the current user's username
 */
void user_set_username(user_t *user, char *username);

/**
 * Function that gets the user's jetpack with the flame on
 * @param user the current user in the game
 * @return the asset which contains the user's jetpack with the flame on
 */
asset_t *user_get_jetpack_flame(user_t *user);

/**
 * Function that gets the user's jetpack with the flame off
 * @param user the current user in the game
 * @return the asset which contains the user's jetpack with the flame off
 */
asset_t *user_get_jetpack_no_flame(user_t *user);

/**
 * Function that sets the status of the jetpack
 * @param user the current user in the game
 * @param status the status to set the jetpack to
 */
void user_set_jetpack_status(user_t *user, bool status);

/**
 * Function that gets the status of the jetpack
 * @param user the current user in the game
 * 
 * @return the status of the jetpack
 */
bool user_get_jetpack_status(user_t *user);

/**
 * Function that sets the score of the user
 * @param user the current user in the game
 * @param score the score to set
 */
void user_set_score(user_t *user, size_t score);

/**
 * Function that gets the score of the user
 * @param user the current user in the game
 * @return the score of the user
 */
size_t user_get_score(user_t *user);

/**
 * It updates the current score of the user
 * @param user the current user in the game
 */
void user_score_tick(user_t *user);

/**
 * Applies the damage to the user
 * @param user the current user in the game
 * @param the damage to apply
 */
void user_apply_damage(user_t *user, size_t damage);

/**
 * Frees the user
 * @param user the user to free
 */
void user_free(user_t *user);