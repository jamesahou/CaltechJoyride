#ifndef __SDL_WRAPPER_H__
#define __SDL_WRAPPER_H__

#include "color.h"
#include "list.h"
#include "polygon.h"
#include "scene.h"
#include "state.h"
#include "vector.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>

// Values passed to a key handler when the given arrow key is pressed
typedef enum {
  LEFT_ARROW = 1,
  UP_ARROW = 2,
  RIGHT_ARROW = 3,
  DOWN_ARROW = 4,
  SPACE_BAR = 5,
} arrow_key_t;

/**
 * The possible types of key events.
 * Enum types in C are much more primitive than in Java; this is equivalent to:
 * typedef unsigned int KeyEventType;
 * #define KEY_PRESSED 0
 * #define KEY_RELEASED 1
 */
typedef enum { KEY_PRESSED, KEY_RELEASED } key_event_type_t;

/**
 * A keypress handler.
 * When a key is pressed or released, the handler is passed its char value.
 * Most keys are passed as their char value, e.g. 'a', '1', or '\r'.
 * Arrow keys have the special values listed above.
 *
 * @param key a character indicating which key was pressed
 * @param type the type of key event (KEY_PRESSED or KEY_RELEASED)
 * @param held_time if a press event, the time the key has been held in seconds
 */
typedef void (*key_handler_t)(char key, key_event_type_t type, double held_time,
                              void *state);

/**
 * A text input handler.
 *
 * @param input the input text
 */
typedef void (*text_input_handler_t)(char input, void *state);

/**
 * Set the text input flag
 * 
 * @param val the value to set the flag to
*/
void sdl_set_text_input(bool val);

/**
 * Initializes the SDL window and renderer.
 * Must be called once before any of the other SDL functions.
 *
 * @param min the x and y coordinates of the bottom left of the scene
 * @param max the x and y coordinates of the top right of the scene
 */
void sdl_init(vector_t min, vector_t max);

/**
 * Processes all SDL events and returns whether the window has been closed.
 * This function must be called in order to handle inputs.
 *
 * @return true if the window was closed, false otherwise
 */
bool sdl_is_done(void *state);

/**
 * Clears the screen. Should be called before drawing polygons in each frame.
 */
void sdl_clear(void);

/**
 * Draws a polygon from the given list of vertices and a color.
 *
 * @param poly a struct representing the polygon
 * @param color the color used to fill in the polygon
 */
void sdl_draw_polygon(polygon_t *poly, rgb_color_t color);

/**
 * Displays the rendered frame on the SDL window.
 * Must be called after drawing the polygons in order to show them.
 */
void sdl_show(void);

/**
 * Takes the image from assets and stores it as a texture object
 *
 * @param img_path path to the image
 * @return the image in a texture object.
 */
SDL_Texture *sdl_load_image(const char *img_path);

/**
 * Takes the image passed in, renders it, and then displays
 * it.
 *
 * @param texture a pointer to the image in the texture format
 * @param width the desired width of the displayed image
 * @param height the desired height of the displayed image
 * @param pos the position of the image
 */
void sdl_draw_image(SDL_Texture *texture, size_t width, size_t height,
                    vector_t pos);

/**
 * Takes the image passed in, renders it, and then displays
 * it.
 *
 * @param texture a pointer to the image in the texture format
 * @param width the desired width of the displayed image
 * @param height the desired height of the displayed image
 * @param pos the position of the image
 * @param angle the angle
 * @param center the center of rotation
 */
void sdl_draw_rotating_image(SDL_Texture *texture, size_t width, size_t height,
                    vector_t pos, double angle, vector_t center);

/**
 * Takes the wav from the stored file and returns corresponding Mix object.
 *
 * @param wav_path path to the .wav file.
 * @return the wav sound in a Mix_Chunk object
 */
Mix_Chunk *sdl_load_wav(const char *wav_path);

/**
 * Takes the music from the stored file and returns corresponding Mix object.
 *
 * @param music_path path to the music file.
 * @return the music in a Mix_Music object
 */
Mix_Music *sdl_load_music(const char *music_path);

/**
 * Plays the given sound effect object and asserts its successful.
 *
 * @param wav sound effect object.
 */
void sdl_play_effect(Mix_Chunk* wav);

/**
 * Plays the given music object and asserts its successful.
 *
 * @param music the music object.
 */
void sdl_play_music(Mix_Music* music);

/**
 * Takes the font from assets and stores it as a TTF_Font object
 *
 * @param font_path path to the font.
 * @param point_size size of the font
 * @return the font in a TTF_Font object.
 */
TTF_Font *sdl_load_font(const char *font_path, size_t point_size);

/**
 * Takes the string passed in, renders it, and then displays
 * it.
 *
 * @param message a string containing message to be rendered
 * @param font
 * @param size the desired size of the displayed text
 * @param pos the desired location of the displayed text
 */
void sdl_draw_text(const char *message, TTF_Font *font, size_t size,
                   vector_t pos, rgb_color_t color);

/**
 * Draws all bodies in a scene.
 * This internally calls sdl_clear(), sdl_draw_polygon(), and sdl_show(),
 * so those functions should not be called directly.
 *
 * @param scene the scene to draw
 * @param aux an additional body to draw (can be NULL if no additional bodies)
 */
void sdl_render_scene(scene_t *scene, void *aux);

/**
 * Registers a function to be called every time a key is pressed.
 * Overwrites any existing handler.
 *
 * Example:
 * ```
 * void on_key(char key, key_event_type_t type, double held_time) {
 *     if (type == KEY_PRESSED) {
 *         switch (key) {
 *             case 'a':
 *                 printf("A pressed\n");
 *                 break;
 *             case UP_ARROW:
 *                 printf("UP pressed\n");
 *                 break;
 *         }
 *     }
 * }
 * int main(void) {
 *     sdl_on_key(on_key);
 *     while (!sdl_is_done());
 * }
 * ```
 *
 * @param handler the function to call with each key press
 */
void sdl_on_key(key_handler_t handler);

void sdl_on_input(text_input_handler_t handler);

/**
 * Gets the amount of time that has passed since the last time
 * this function was called, in seconds.
 *
 * @return the number of seconds that have elapsed
 */
double time_since_last_tick(void);

/**
 * Gets the bounding box for a given body.
 *
 * @param body given body
 * @return the bouding box for the body
 */
SDL_Rect sdl_get_bounding_box(body_t *body);

#endif // #ifndef __SDL_WRAPPER_H__
