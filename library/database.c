#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

const char *DB_PATH = "assets/database.txt";
const size_t NUM_SKINS = 12;
const size_t NUM_INFO = 2;

db_entry_t *db_entry_init(char *username, size_t total_score, bool *skins) {
    db_entry_t *entry = malloc(sizeof(db_entry_t));
    assert(entry);


    size_t username_len = strlen(username);
    entry->username = malloc(sizeof(char) * (username_len + 1));
    assert(entry->username);
    strcpy(entry->username, username);
    entry->username[username_len] = '\0';
    
    entry->total_score = total_score;
    entry->skins = skins;

    return entry;
}

void db_entry_free(db_entry_t *entry) {
    free(entry->username);
    free(entry->skins);
    free(entry);
}

void database_write(list_t *db) {
    FILE *file = fopen(DB_PATH, "w");
    assert(file);

    for (size_t i = 0; i < list_size(db); i++) {
        db_entry_t *entry = list_get(db, i);
        
        char *list_str = malloc(sizeof(char) * (NUM_SKINS * 2)); // 2 for commas
        assert(list_str);
        for (size_t j = 0; j < NUM_SKINS; j++) {
            if(entry->skins[j]) {
                list_str[2 * j] = '1';
            } else {
                list_str[2 * j] = '0';
            }
            if (j == NUM_SKINS - 1) {
                list_str[2 * j + 1] = '\0';
            } else {
                list_str[2 * j + 1] = ',';
            }
        }

        fprintf(file, "%s,%zu,%s\n", entry->username, entry->total_score, list_str);
        free(list_str);
    }

    fclose(file);
}

list_t *database_read() {
    list_t *list = list_init(1, (free_func_t) db_entry_free);
    FILE *file = fopen(DB_PATH, "r");
    assert(file);
    
    size_t line_num = 0;
    char c = getc(file);
    while (c != EOF) {
        size_t line_len = 0;
        while (c != '\n') {
            line_len++;
            c = getc(file);        
        }

        fseek(file, line_num, SEEK_SET);
        line_num += line_len + 1;

        char *line = malloc(sizeof(char) * (line_len + 1));
        assert(line);
        size_t idx = 0;
        while (idx <= line_len) {
            c = getc(file);
            line[idx] = c;
            idx++;
        }
        line[line_len] = '\0';
        c = getc(file); // Moves one

        strarray_t *split = mystr_split(line, ',');
        assert(split->length == NUM_INFO + NUM_SKINS);

        bool *skins = malloc(sizeof(bool) * NUM_SKINS);
        assert(skins);
        for (size_t i = 0; i < NUM_SKINS; i++) {
            skins[i] = strcmp("1", split->data[NUM_INFO + i]) == 0;
        }
        
        size_t total_score = (size_t) atoi(split->data[1]);
        db_entry_t *db_entry = db_entry_init(split->data[0], total_score, skins);

        list_add(list, db_entry);
        
        strarray_free(split);
        free(line);
    }

    fclose(file);
    return list;
}

db_entry_t *database_get_user(list_t* db, char *username) {
    ssize_t index = -1;
    for (size_t i = 0; i < list_size(db); i++) {
        db_entry_t *entry = list_get(db, i);
        if (strcmp(username, entry->username) == 0) {
            index = i;
            break;
        }
    }

    db_entry_t *retrieved;
    if (index == -1) {
        bool *skins = calloc(NUM_SKINS, sizeof(bool));
        assert(skins);
        skins[0] = true;    // default skin given

        retrieved = db_entry_init(username, 0, skins);
        list_add(db, retrieved);
        database_write(db);
    } else {
        retrieved = list_get(db, index);
    }

    return retrieved;
}

size_t db_entry_get_score(db_entry_t *entry) {
    return entry->total_score;
}

void db_entry_set_score(db_entry_t *entry, size_t new_score) {
    entry->total_score = new_score;
}

bool db_entry_has_skin(db_entry_t *entry, size_t skin_id) {
    assert(0 <= skin_id && skin_id < NUM_SKINS);
    return entry->skins[skin_id];
}

void db_entry_unlock_skin(db_entry_t *entry, size_t skin_id) {
    assert(0 <= skin_id && skin_id < NUM_SKINS);
    entry->skins[skin_id] = true;
}