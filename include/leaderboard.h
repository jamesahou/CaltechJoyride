#include "vector.h"
#include "asset.h"
#include "asset_cache.h"
#include "list.h"
#include "sdl_wrapper.h"
#include "mystr.h"

/**
 * A score type. Stores username and the score.
*/
typedef struct score score_t;

/**
 * Allocates memory for a new score and initializes
 * the username and score field.
 * 
 * @param username username of the player
 * @param num_score score the user got
 * 
 * @return a point to the newly allocated score
*/
score_t *score_init(char *username, size_t num_score);

/**
 * Releases the memory allocated for a score.
 * 
 * @param score a point to a score returned from score_init()
*/
void score_free(score_t *score);

/**
 * Adds the score into the scores list.
 * 
 * @param score a pointer to a score
*/
void leaderboard_add_score(list_t *scores, score_t *score);

/**
 * Writes the list of scores provided onto leaderboard.txt.
 * Follows the following format #. {username} : {score}\n
 * 
 * @param scores a list of scores
*/
void write_file(list_t *scores);

/**
 * Reads the lines from leaderboard.txt.
 * 
 * @return a list of strings.
*/
list_t *read_file();

/**
 * Reads the scores from leaderboard.txt.
 * 
 * @return a list of the top five scores
*/
list_t *get_scores();

/**
 * Compares two scores. 
 * 
 * @param s1 a pointer to score 1
 * @param s2 a pointer to score 2
 * 
 * @return 1 if s1 less than s2, 0 if there equal, -1 otherwise, 
*/
size_t score_compare(const void *s1, const void *s2);

/**
 * Updates if the new score is better than one of the top five.
 * Else, ignores it.
 * 
 * @param new_score new score
*/
void update_score(score_t *new_score);


/**
 * Returns the number of digits in a score.
 * 
 * @param score a score
 * 
 * @return the number of digits in the scocre
*/
size_t get_num_digits(size_t score);

/**
 * Prepares a list of text assets to render the leaderboard.
 * 
 * @return a list of text assets
*/
list_t *leaderboard_assets(list_t *scores);