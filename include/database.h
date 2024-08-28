#include "list.h"
#include "mystr.h"
#include <stdbool.h>

typedef struct db_entry {
    char *username;
    size_t total_score;
    bool *skins;
} db_entry_t;

/**
 * Initialize a database entry object.
 *
 * @param username the entry's username
 * @param total_score the user's total score
 * @param skins the skin indicator array
 * @return a pointer to a db_entry_t object
 */
db_entry_t *db_entry_init(char *username, size_t total_score, bool skins[]);

/**
 * Frees a database entry object
 *
 * @param entry the database entry to free
 */
void db_entry_free(db_entry_t *entry);

/**
 * Write the given database to the database text file 
 *
 * @param db the database to dump
 */
void database_write(list_t *db);

/**
 * Read and return a database from the database text file. 
 *
 * @return a database pointer (a list of database entries)
 */
list_t *database_read();

/**
 * Get the database entry corresponding to the given username, create a new
 * entry if the username is not found (new account)
 *
 * @param db the database to search through/modify
 * @param username the username to query/register
 * 
 * @return returns the database entry corresponding to the user
 */
db_entry_t *database_get_user(list_t* db, char *username);

/**
 * Get the score of the database entry
 *
 * @param entry the database entry to get the score from
 * 
 * @return the score of the entry
 */
size_t db_entry_get_score(db_entry_t *entry);

/**
 * Set the score of the entry
 *
 * @param entry the database entry to set the score for
 */
void db_entry_set_score(db_entry_t *entry, size_t new_score);

/**
 * Check if the entry/user has the skin of the given id
 *
 * @param entry the database entry to check
 * @param skin_id the id of the skin to check for
 * 
 * @return whether the user has the skin
 */
bool db_entry_has_skin(db_entry_t *entry, size_t skin_id);

/**
 * Unlock the skin for the user -> set the skin bool to true
 *
 * @param entry the database entry to unlock skins for
 * @param skin_id the id of the skin to unlock
 */
void db_entry_unlock_skin(db_entry_t *entry, size_t skin_id);