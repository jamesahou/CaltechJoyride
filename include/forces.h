#ifndef __FORCES_H__
#define __FORCES_H__

#include "collision.h"
#include "scene.h"
#include "user.h"

typedef enum { 
  HEALTH = 0, 
  MUFFLER = 1,
  SCORE_DOUBLER = 2,
  FORCE_FIELD = 3,
  SLOW_DOWN = 4,
} powerup_type_t;

typedef struct force {
  force_creator_t force_creator;
  void *aux;
  list_t *bodies;
} force_t;

/**
 * A function called when a collision occurs.
 * @param body1 the first body passed to create_collision()
 * @param body2 the second body passed to create_collision()
 * @param axis a unit vector pointing from body1 towards body2
 *   that defines the direction the two bodies are colliding in
 * @param aux the auxiliary value passed to create_collision()
 * @param force_const the force constant passed to create_collision()
 */
typedef void (*collision_handler_t)(body_t *body1, body_t *body2, vector_t axis,
                                    void *aux, double force_const);

/**
 * Function to initialize a force object
 * @param force_creator the function that creates the force
 * @param aux auxiliary information describing the objects involved in the force
 * @return the force object
 */
force_t *force_init(force_creator_t force_creator, void *aux, list_t *body);

/**
 * Function to free a force object
 * @param force
 */
void force_free(force_t *force);

/**
 * Function to free a body aux
 * @param aux
 */
void body_aux_free(void *aux);
/**
 * Adds a force creator to a scene that applies gravity between two bodies.
 * The force creator will be called each tick
 * to compute the Newtonian gravitational force between the bodies.
 * See
 * https://en.wikipedia.org/wiki/Newton%27s_law_of_universal_gravitation#Vector_form.
 * The force should not be applied when the bodies are very close,
 * because its magnitude blows up as the distance between the bodies goes to 0.
 *
 * @param scene the scene containing the bodies
 * @param G the gravitational proportionality constant
 * @param body1 the first body
 * @param body2 the second body
 */
void create_newtonian_gravity(scene_t *scene, double G, body_t *body1,
                              body_t *body2);

/**
 * Adds a force creator to a scene that applies gravity of a planet.
 * The force creator will be called each tick 
 * 
 * @param scene the scene containing the bodies
 * @param g the gravitational acceleration constant
 * @param body
 */
void create_planet_gravity(scene_t *scene, double g, body_t *body);

/**
 * Adds a force creator to a scene that acts like a spring between two bodies.
 * The force creator will be called each tick
 * to compute the Hooke's-Law spring force between the bodies.
 * See https://en.wikipedia.org/wiki/Hooke%27s_law.
 *
 * @param scene the scene containing the bodies
 * @param k the Hooke's constant for the spring
 * @param body1 the first body
 * @param body2 the second body
 */
void create_spring(scene_t *scene, double k, body_t *body1, body_t *body2);

/**
 * Adds a force creator to a scene that applies a drag force on a body.
 * The force creator will be called each tick
 * to compute the drag force on the body proportional to its velocity.
 * The force points opposite the body's velocity.
 *
 * @param scene the scene containing the bodies
 * @param gamma the proportionality constant between force and velocity
 *   (higher gamma means more drag)
 * @param body the body to slow down
 */
void create_drag(scene_t *scene, double gamma, body_t *body);

/**
 * Adds a force creator to a scene that calls a given collision handler
 * function each time two bodies collide.
 * This generalizes create_destructive_collision() from last week,
 * allowing different things to happen on a collision.
 * The handler is passed the bodies, the collision axis, and an auxiliary value.
 * It should only be called once while the bodies are still colliding.
 *
 * @param scene the scene containing the bodies
 * @param body1 the first body
 * @param body2 the second body
 * @param handler a function to call whenever the bodies collide
 * @param aux an auxiliary value to pass to the handler
 * @param force_const a constant to pass to the handler
 */
void create_collision(scene_t *scene, body_t *body1, body_t *body2,
                      collision_handler_t handler, void *aux,
                      double force_const);

/**
 * Adds a force creator to a scene that destroys two bodies when they collide.
 * The bodies should be destroyed by calling body_remove().
 * This should be represented as an on-collision callback
 * registered with create_collision().
 *
 * @param scene the scene containing the bodies
 * @param body1 the first body
 * @param body2 the second body
 */
void create_destructive_collision(scene_t *scene, body_t *body1, body_t *body2);

/**
 * The collision handler for for physics collisions. Applies impulses to
 * bodies according to the elasticity in `aux`.
 */
void physics_collision_handler(body_t *body1, body_t *body2, vector_t axis,
                               void *aux, double force_const);

/**
 * Adds a force creator to a scene that applies impulses
 * to resolve collisions between two bodies in the scene.
 * This should be represented as an on-collision callback
 * registered with create_collision().
 *
 * You may remember from project01 that you should avoid applying impulses
 * multiple times while the bodies are still colliding.
 * You should also have a special case that allows either body1 or body2
 * to have mass INFINITY, as this is useful for simulating walls.
 *
 * @param scene the scene containing the bodies
 * @param body1 the first body
 * @param body2 the second body
 * @param elasticity the "coefficient of restitution" of the collision;
 * 0 is a perfectly inelastic collision and 1 is a perfectly elastic collision
 */
void create_physics_collision(scene_t *scene, body_t *body1, body_t *body2,
                              double elasticity);

//void powerup_collision_handler(body_t *body1, body_t *body2, vector_t axis,
//                               void *aux, double force_const);
void health_powerup_handler(user_t *user);

void muffler_powerup_handler(user_t *user, powerup_type_t type);

void score_doubler_powerup_handler(user_t *user, powerup_type_t type);

//void create_powerup_collision(scene_t *scene, user_t *user, body_t *powerup, powerup_type_t type, list_t *body_assets);
#endif // #ifndef __FORCES_H__
