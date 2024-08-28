#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "leaderboard.h"

typedef struct score {
  char *username;
  size_t score;
} score_t;

const char *filepath = "assets/leaderboard.txt";
const char *font_path = "assets/font.ttf";
const size_t font_size = 18;
const size_t SPACE_OFFSET = 3;

vector_t text_location = (vector_t){375, 175};
rgb_color_t color = (rgb_color_t){0, 255, 255};
static const size_t TOP_FIVE = 5;

score_t *score_init(char *username, size_t num_score) {
    score_t *score = malloc(sizeof(score_t));
    assert(score);

    score->username = malloc(sizeof(char) * (strlen(username) + 1));
    assert(score->username);
    strcpy(score->username, username);
    score->score = num_score;

    return score;
}

void score_free(score_t *score) {
    free(score->username);
    free(score);
}

size_t score_compare(const void *s1, const void *s2) {
    score_t *score_1 = *(score_t **) s1;
    score_t *score_2 = *(score_t **) s2;

    return score_2->score - score_1->score;
}

void leaderboard_add_score(list_t *scores, score_t *score) {
    list_add(scores, score);
    list_sort(scores, (__compar_fn_t)score_compare);
}

void write_file(list_t *scores) {
    FILE *file = fopen(filepath, "w");
    assert(file);

    size_t i = 0;
    while (i < TOP_FIVE && i < list_size(scores)) {
        score_t *score = list_get(scores, i);
        fprintf(file, "%zu. %s : %zu\n", i + 1, score->username, score->score);
        i++;
    }

    fclose(file);
}

list_t *read_file() {
    list_t *list = list_init(TOP_FIVE, free);
    FILE *file = fopen(filepath, "r");
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

        list_add(list, line);
    }

    fclose(file);
    return list;
}

list_t *get_scores() {
    list_t *lines = read_file();
    list_t *list = list_init(TOP_FIVE, (free_func_t)score_free);

    for (size_t i = 0; i < list_size(lines); i++) {
        char* line = list_get(lines, i);
        strarray_t *strarray = mystr_split(line, ' ');
        score_t *score = score_init(strarray->data[1], atoi(strarray->data[3]));

        strarray_free(strarray);

        list_add(list, score);
    }
    list_free(lines);
    return list;
}

void update_score(score_t *new_score) {
    list_t *scores = get_scores();

    list_add(scores, new_score);
    list_sort(scores, (__compar_fn_t)score_compare);
    write_file(scores);

    list_free(scores);
}

size_t get_num_digits(size_t score) {
    size_t count = 0;
    while (score > 0) {
        count++;
        score /= 10;
    }
    return count;
}

list_t *score_to_str(list_t *scores) {
    list_t *lines = list_init(TOP_FIVE, (free_func_t)free);
    for (size_t i = 0; i < list_size(scores); i++) {
        score_t *score = list_get(scores, i);
        char *username = score->username;
        size_t digits = get_num_digits(score->score);
        char *line = malloc(sizeof(char) * ((SPACE_OFFSET) + strlen(username) + (SPACE_OFFSET) + digits + 1));
        sprintf(line, "%zu. %s : %zu", i + 1, score->username, score->score);
        list_add(lines, line);
    }
    return lines;
}

list_t *leaderboard_assets(list_t *scores) {
    list_t *lines = score_to_str(scores);

    list_t *assets = list_init(TOP_FIVE + 1, (free_func_t)asset_destroy);

    char *title = "LEADERBOARD";
    SDL_Rect title_bounding_box = {.w = strlen(title) * font_size, .h = font_size * 2, .x = text_location.x, .y = text_location.y - (font_size * 2)};
    asset_t *title_asset = asset_make_text(font_path, title_bounding_box, (const char *)strdup(title), color);
    list_add(assets, title_asset);

    for (size_t i = 0; i < list_size(lines); i++) {
        char *text = list_get(lines, i);
        SDL_Rect bounding_box = {.w = strlen(text) * font_size, .h = font_size * 2, .x = text_location.x, .y = text_location.y + (font_size * i * 2)};
        asset_t *asset = asset_make_text(font_path, bounding_box, (const char *)strdup(text), color);
        list_add(assets, asset);
    }
    
    list_free(lines);
    return assets;
}