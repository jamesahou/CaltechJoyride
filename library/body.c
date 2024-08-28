#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "body.h"

typedef struct body {
  polygon_t *poly;

  double mass;
  double rotation;
  vector_t velo;
  vector_t centroid;
  vector_t force;
  vector_t impulse;
  bool removed;
  bool is_laser;

  void *info;
  free_func_t info_freer;
} body_t;

void body_reset(body_t *body) {
  body->force = VEC_ZERO;
  body->impulse = VEC_ZERO;
}
/**
 * Frees the given vector
 *
 * @param vec the vector to be freed
 */
void vec_free(vector_t *vec) { free(vec); }

body_t *body_init_with_info(list_t *shape, double mass, rgb_color_t color,
                            void *info, free_func_t info_freer) {
  body_t *body = malloc(sizeof(body_t));
  assert(body);

  polygon_t *poly = polygon_init(shape, VEC_ZERO, 0, color.r, color.g, color.b);
  body->poly = poly;
  body->centroid = polygon_centroid(body->poly);
  polygon_translate(body->poly, vec_negate(body->centroid));
  body->mass = mass;
  body->velo = VEC_ZERO;
  body->rotation = 0;
  body->force = VEC_ZERO;
  body->impulse = VEC_ZERO;
  body->removed = false;
  body->is_laser = false;
  body->info = info;
  body->info_freer = info_freer;

  return body;
}

body_t *body_init(list_t *shape, double mass, rgb_color_t color) {
  body_t *body = body_init_with_info(shape, mass, color, NULL, NULL);
  return body;
}

void body_free(body_t *body) {
  polygon_free(body->poly);
  if (body->info_freer != NULL) {
    body->info_freer(body->info);
  }
  free(body);
}

list_t *body_get_shape(body_t *body) {
  polygon_rotate(body->poly, body->rotation, VEC_ZERO);
  // translate and rotate the rest of the points around the centroid
  polygon_translate(body->poly, body->centroid);

  list_t *o_shape = polygon_get_points(body->poly);
  list_t *shape_copy = list_init(list_size(o_shape), (free_func_t)vec_free);
  for (size_t i = 0; i < list_size(o_shape); i++) {
    vector_t *nv = malloc(sizeof(vector_t));
    assert(nv);
    vector_t *ov = list_get(o_shape, i);
    nv->x = ov->x;
    nv->y = ov->y;
    list_add(shape_copy, nv);
  }

  polygon_translate(body->poly, vec_negate(body->centroid));
  polygon_rotate(body->poly, -(body->rotation), VEC_ZERO);

  return shape_copy;
}

vector_t body_get_centroid(body_t *shape) { return shape->centroid; }

vector_t body_get_velocity(body_t *shape) { return shape->velo; }

rgb_color_t *body_get_color(body_t *body) {
  return polygon_get_color(body->poly);
}

polygon_t *body_get_polygon(body_t *body) { return body->poly; }

void *body_get_info(body_t *body) { return body->info; }

void body_set_centroid(body_t *body, vector_t x) { body->centroid = x; }

void body_set_velocity(body_t *body, vector_t v) { body->velo = v; }

double body_get_rotation(body_t *body) { return body->rotation; }

void body_set_rotation(body_t *body, double angle) { body->rotation = angle; }

void body_add_force(body_t *body, vector_t force) {
  body->force = vec_add(body->force, force);
}

void body_add_impulse(body_t *body, vector_t impulse) {
  body->impulse = vec_add(body->impulse, impulse);
}

double body_get_mass(body_t *body) { return body->mass; }

void body_tick(body_t *body, double dt) {
  // I = mdv => dv = I/m
  vector_t impulse_dv = vec_multiply(1.0 / body->mass, body->impulse);
  // a = dv/dt = F/m => dv = Fdt/m
  vector_t force_dv = vec_multiply(dt / body->mass, body->force);
  vector_t dv = vec_add(impulse_dv, force_dv);

  vector_t new_velo = vec_add(body->velo, dv);
  vector_t actual_velo = vec_multiply(1.0 / 2.0, vec_add(new_velo, body->velo));
  body->velo = new_velo;

  /* track centroid only, move attached points later */
  body_set_centroid(body,
                    vec_add(body->centroid, vec_multiply(dt, actual_velo)));

  body->impulse = VEC_ZERO;
  body->force = VEC_ZERO;
}

void body_remove(body_t *body) { body->removed = true; }

bool body_is_removed(body_t *body) { return body->removed; }