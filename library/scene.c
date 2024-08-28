#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "forces.h"
#include "scene.h"

struct scene {
  size_t num_bodies;
  list_t *bodies;
  list_t *force_creators;
};

scene_t *scene_init(void) {
  scene_t *scene = malloc(sizeof(scene_t));
  assert(scene);

  scene->bodies = list_init(0, (free_func_t)body_free);
  scene->force_creators = list_init(0, (free_func_t)force_free);

  return scene;
}

void scene_clear(scene_t *scene) {
  list_free(scene->bodies);
  list_free(scene->force_creators);

  scene->bodies = list_init(0, (free_func_t)body_free);
  scene->force_creators = list_init(0, (free_func_t)force_free);
}

void scene_free(scene_t *scene) {
  list_free(scene->bodies);
  list_free(scene->force_creators);
  free(scene);
}

size_t scene_bodies(scene_t *scene) { return list_size(scene->bodies); }

body_t *scene_get_body(scene_t *scene, size_t index) {
  assert(index < list_size(scene->bodies));
  return list_get(scene->bodies, index);
}

void scene_add_body(scene_t *scene, body_t *body) {
  list_add(scene->bodies, body);
}

void scene_remove_body(scene_t *scene, size_t index) {
  assert(index < list_size(scene->bodies));
  body_remove(list_get(scene->bodies, index));
}

void scene_tick(scene_t *scene, double dt) {
  for (ssize_t i = 0; i < (ssize_t)list_size(scene->force_creators); i++) {
    force_t *force = list_get(scene->force_creators, i);

    force->force_creator(force->aux);
  }

  for (ssize_t i = 0; i < (ssize_t)scene_bodies(scene); i++) {
    body_t *body = list_get(scene->bodies, i);
    if (body_is_removed(body)) {
      for (ssize_t j = 0; j < (ssize_t)list_size(scene->force_creators); j++) {
        force_t *force = list_get(scene->force_creators, j);
        for (ssize_t k = 0; k < (ssize_t)list_size(force->bodies); k++) {
          body_t *new_body = list_get(force->bodies, k);
          if (new_body == body) {
            list_remove(scene->force_creators, j);
            force_free(force);
            j--;
            break;
          }
        }
      }
      list_remove(scene->bodies, i);
      body_free(body);
      i--;
    } else {
      body_tick(body, dt);
    }
  }
}

void scene_add_force_creator(scene_t *scene, force_creator_t force_creator,
                             void *aux) {
  scene_add_bodies_force_creator(scene, force_creator, aux, list_init(0, free));
}

void scene_add_bodies_force_creator(scene_t *scene, force_creator_t forcer,
                                    void *aux, list_t *bodies) {
  force_t *force = force_init(forcer, aux, bodies);
  list_add(scene->force_creators, force);
}
