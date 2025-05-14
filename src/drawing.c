#include "drawing.h"

void draw_circle(SDL_Renderer* renderer, int center_x, int center_y, int radius) {
    for (int y = -radius; y <= radius; y++) {
        int width = (int)(sqrtf(radius * radius - y * y) + 0.5f);
        SDL_RenderDrawLine(
            renderer,
            center_x - width, center_y + y,
            center_x + width, center_y + y
        );
    }
}
