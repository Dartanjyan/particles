#include <SDL2/SDL.h>
#include <stdlib.h>
#include "funcs.h"

float random_float(float min, float max) {
    float scale = (float)rand() / (float)RAND_MAX; // [0, 1]
    return min + scale * (max - min); // [min, max]
}

SDL_Color random_color() {
    SDL_Color color;
    color.r = rand()%255;
    color.g = rand()%255;
    color.b = rand()%255;
    color.a = 255;
    return color;
}

float absf(float a) {
    return a > 0 ? a : -a;
}

float falloff(float x) {
    // TODO: Find good epsilon
    const float epsilon = 0.01;
    return 1.0f / (absf(powf(x, 1.0)) + epsilon);
}
