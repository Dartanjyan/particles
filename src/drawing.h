#ifndef DRAWING_H
#define DRAWING_H

#include <SDL2/SDL.h>

void draw_circle(SDL_Renderer* renderer, int center_x, int center_y, int radius);

void draw_vector_arrow(
    SDL_Renderer *renderer,
    float x, float y,          // позиция
    float vx, float vy,        // вектор
    float arrow_len            // длина стрелки (в пикселях)
);


#endif
