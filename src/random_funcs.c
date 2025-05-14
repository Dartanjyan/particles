#include <SDL2/SDL.h>
#include <stdlib.h>
#include "random_funcs.h"

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
