#include "vector.h"
#include <math.h>
#include <stdlib.h>

const vector_t VEC_ZERO = {0, 0};

vector_t vec_add(vector_t v1, vector_t v2) {
  return (vector_t){v1.x + v2.x, v1.y + v2.y};
}

vector_t vec_negate(vector_t v) {
  vector_t vneg = {-v.x, -v.y};
  return vneg;
}

vector_t vec_subtract(vector_t v1, vector_t v2) {
  return vec_add(v1, vec_negate(v2));
}

vector_t vec_multiply(double scalar, vector_t v) {
  return (vector_t){scalar * v.x, scalar * v.y};
}

double vec_dot(vector_t v1, vector_t v2) {
  // (x1, y1)dot(x2, y2) = x1 * x2 + y1 * y2
  return v1.x * v2.x + v1.y * v2.y;
}

double vec_cross(vector_t v1, vector_t v2) {
  // (x1, y1)x(x2, y2) = x1 * y2 - x2 * y1
  return v1.x * v2.y - v2.x * v1.y;
}

double vec_get_length(vector_t v) { return sqrt(pow(v.x, 2) + pow(v.y, 2)); }

vector_t vec_rotate(vector_t v, double angle) {
  // apply rotation matrix formula
  vector_t vrot = {v.x * cos(angle) - v.y * sin(angle),
                   v.x * sin(angle) + v.y * cos(angle)};
  return vrot;
}

double get_distance(vector_t v1, vector_t v2) {
  return sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2));
}
