#include "polygon.h"
#include "color.h"
#include "list.h"
#include "vector.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct polygon {
  list_t *points;
  vector_t velocity;
  vector_t centroid;
  double rotation_speed;
  rgb_color_t *color;
} polygon_t;

// add centroid to the struct
//
size_t ROTATION_ANGLE = M_PI / 480;

polygon_t *polygon_init(list_t *points, vector_t initial_velocity,
                        double rotation_speed, double red, double green,
                        double blue) {
  polygon_t *polygon = malloc(sizeof(polygon_t));
  assert(polygon);
  polygon->points = points;
  polygon->velocity = initial_velocity;
  polygon->rotation_speed = rotation_speed;
  polygon->color = color_init(red, green, blue);
  polygon->centroid = polygon_centroid(polygon);
  return polygon;
}

list_t *polygon_get_points(polygon_t *polygon) { return polygon->points; }

void polygon_translate(polygon_t *polygon, vector_t translation) {
  size_t num_v = list_size(polygon->points);
  polygon->centroid = vec_add(polygon->centroid, translation);
  for (size_t i = 0; i < num_v; i++) {
    vector_t *vertex = list_get(polygon->points, i);
    vector_t v = vec_add(*vertex, translation);

    vertex->x = v.x;
    vertex->y = v.y;
  }
}

void polygon_rotate(polygon_t *polygon, double angle, vector_t point) {
  bool temp = false;
  size_t num_v = list_size(polygon->points);
  // recompute the centroid in the case where point doesn't equal the centroid
  if (point.x == polygon->centroid.x && point.y == polygon->centroid.y) {
    temp = true;
  }
  // translate polygon to new frame so that "point" is origin
  polygon_translate(polygon, vec_negate(point));
  for (size_t i = 0; i < num_v; i++) {
    // rotate point about origin
    vector_t *vertex = list_get(polygon->points, i);
    vector_t rotated = vec_rotate(*vertex, angle);

    vertex->x = rotated.x;
    vertex->y = rotated.y;
  }
  // translate polygon back to original frame
  polygon_translate(polygon, point);
  // recompute in event that point isn't the centroid
  if (temp) {
    polygon_centroid(polygon);
  }
}

vector_t polygon_centroid(polygon_t *polygon) {
  size_t num_v = list_size(polygon->points);

  vector_t centroid = {0, 0};
  for (size_t i = 0; i < num_v; i++) {
    vector_t *v1;
    vector_t *v2;
    // handles x_n=x_0 wraparound
    if (i + 1 == num_v) {
      v1 = list_get(polygon->points, i);
      v2 = list_get(polygon->points, 0);
    } else {
      v1 = list_get(polygon->points, i);
      v2 = list_get(polygon->points, i + 1);
    }
    centroid.x += (v1->x + v2->x) * (v1->x * v2->y - v2->x * v1->y);
    centroid.y += (v1->y + v2->y) * (v1->x * v2->y - v2->x * v1->y);
  }

  double area = polygon_area(polygon);
  return vec_multiply(1 / (6 * area), centroid);
}

vector_t polygon_get_center(polygon_t *polygon) { return polygon->centroid; }

void polygon_move(polygon_t *polygon, double time_elapsed) {
  polygon_translate(polygon, vec_multiply(time_elapsed, polygon->velocity));
  vector_t center = polygon_centroid(polygon);
  polygon_rotate(polygon, ROTATION_ANGLE, center);
}

void polygon_set_velocity(polygon_t *polygon, vector_t vel) {
  polygon->velocity = vel;
}

void polygon_free(polygon_t *polygon) {
  color_free(polygon->color);
  list_free(polygon->points);
  free(polygon);
}

double polygon_get_velocity_x(polygon_t *polygon) {
  return polygon->velocity.x;
}

double polygon_get_velocity_y(polygon_t *polygon) {
  return polygon->velocity.y;
}

double polygon_area(polygon_t *polygon) {
  double area = 0;
  size_t num_v = list_size(polygon->points);
  for (size_t i = 0; i < num_v; i++) {
    vector_t *v1;
    vector_t *v2;
    // handles x_n=x_0 wraparound
    if (i + 1 == num_v) {
      v1 = list_get(polygon->points, i);
      v2 = list_get(polygon->points, 0);
    } else {
      v1 = list_get(polygon->points, i);
      v2 = list_get(polygon->points, i + 1);
    }
    area += (v2->x + v1->x) * (v2->y - v1->y);
  }
  area /= 2;
  return fabs(area);
}

rgb_color_t *polygon_get_color(polygon_t *polygon) { return polygon->color; }

void polygon_set_color(polygon_t *polygon, rgb_color_t *color) {
  polygon->color = color;
}

void polygon_set_center(polygon_t *polygon, vector_t centroid) {
  polygon->centroid = centroid;
}

void polygon_set_rotation(polygon_t *polygon, double rot) {
  polygon->rotation_speed = rot;
}

double polygon_get_rotation(polygon_t *polygon) {
  return polygon->rotation_speed;
}
