#ifndef __ASSET_H__
#define __ASSET_H__

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <color.h>
#include <sdl_wrapper.h>
#include <stddef.h>

typedef enum { ASSET_IMAGE, ASSET_FONT, ASSET_BUTTON } asset_type_t;

typedef struct asset asset_t;
typedef struct asset_timer asset_timer_t;

/**
 * A timer handler
 * 
 * @param state the state of the game
 * @param asset the associated asset of the timer
 * @param aux auxillary 
*/
typedef void (*timer_handler_t)(state_t *state, asset_t *asset, void *aux);

/**
 * A timer handler
 * 
 * @param state the state of the game
 * @param timer the timer
*/
typedef void (*timer_tick_handler_t)(state_t *state, asset_timer_t *timer);

typedef struct asset_timer {
  asset_t *asset;
  double timer;
  double exp_time;
  timer_tick_handler_t tick_handler;
  timer_handler_t exp_handler;
  bool expired;
  bool tick_condition;
  void *aux;
} asset_timer_t;

/**
 * Allocates memory for an asset timer with the given parameters.
 *
 * @param asset the asset to put a timer on
 * @param exp_time expiration time of the asset
 * @param exp_handler handler that performs an action when timer ends
 * @param aux auxilliary info for the handler.
 * @return a pointer to the newly allocated asset
 */
asset_timer_t *asset_timer_init(asset_t *asset, double exp_time, timer_tick_handler_t tick_handler,
                                    timer_handler_t exp_handler, void *aux);

/**
 * Frees the memory allocated for the asset_timer.
 * @param asset_timer the asset timer to free
 */
void asset_timer_free(asset_timer_t *asset_timer);

/**
 * Ticks a given timer
 *
 * @param asset_timer the asset timer
 * @param state the state of the game
 * @param dt elapsed time
 */
void asset_timer_tick(asset_timer_t *asset_timer, state_t* state, double dt);

/**
 * Gets the `assx
 */
asset_type_t asset_get_type(asset_t *asset);

/**
 * Allocates memory for an image asset with the given parameters.
 *
 * @param filepath the filepath to the image file
 * @param bounding_box the bounding box containing the location and dimensions
 * of the text when it is rendered
 * @return a pointer to the newly allocated image asset
 */
asset_t *asset_make_image(const char *filepath, SDL_Rect bounding_box);

/**
 * Allocates memory for an image asset with an attached body. When the asset
 * is rendered, the image will be rendered on top of the body.
 *
 * @param filepath the filepath to the image file
 * @param body the body to render the image on top of
 * @return a pointer to the newly allocated image asset
 */
asset_t *asset_make_image_with_body(const char *filepath, body_t *body);

/**
 * Allocates memory for a text asset with the given parameters.
 *
 * @param filepath the filepath to the .ttf file
 * @param bounding_box the bounding box containing the location and dimensions
 * of the text when it is rendered
 * @param text the text to render
 * @param color the color of the text
 * @return a pointer to the newly allocated text asset
 */
asset_t *asset_make_text(const char *filepath, SDL_Rect bounding_box,
                         const char *text, rgb_color_t color);

/**
 * A button handler.
 *
 * @param state the state of the game
 */
typedef void (*button_handler_t)(void *state, void *aux);

/**
 * Allocates memory for a button asset with the given parameters.
 * Note that `image_asset` and `text_asset` should be owned by the caller. Thus,
 * `asset_destroy` will only free the memory allocated for the button.
 *
 * Asserts that `image_asset` is NULL or has type `ASSET_IMAGE`.
 * Asserts that `text_asset` is NULL or has type `ASSET_FONT`.
 *
 * @param bounding_box the bounding box containing the area on the screen that
 * should activate the button handler.
 * @param image_asset the image that the button renders. Can be NULL.
 * @param text_asset the text that the button renders. Can be NULL.
 * @param handler the button handler that runs when the button is clicked.
 * @param aux potentially auxiliary info for the handler
 */
asset_t *asset_make_button(SDL_Rect bounding_box, asset_t *image_asset,
                           asset_t *text_asset, button_handler_t handler, void *aux);

/**
 * Runs the button handler for `button` if
 *
 * 1. `x` and `y` are contained in the button's bounding box.
 * 2. AND `button` is currently rendered onto the screen.
 *
 * @param button the pointer to the button asset
 * @param state the game state
 * @param x the x position of the mouse click
 * @param y the y position of the mouse click
 */
void asset_on_button_click(asset_t *button, state_t *state, double x, double y);

/**
 * Returns the body associated with the image body asset
 * 
 * @param asset the asset to retrieve from
 * @return the body associated with the asset
 */
body_t *asset_get_body(asset_t *asset);

/**
 * Renders the asset to the screen.
 * @param asset the asset to render
 */
void asset_render(asset_t *asset);

/**
 * Set a button_asset_t type of asset to false.
 * @param asset the asset
*/
void asset_button_clear(asset_t *asset);

/**
 * Frees the memory allocated for the asset.
 * @param asset the asset to free
 */
void asset_destroy(asset_t *asset);

/**
 * Gets the bounding box of an asset
 * @param asset the asset to get the bounding box of
 * @return a SDL Rect object representing the asset's bounding box
*/
SDL_Rect asset_get_bounding_box(asset_t *asset);

#endif // #ifndef __ASSET_H__
