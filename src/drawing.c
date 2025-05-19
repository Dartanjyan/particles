#include "drawing.h"

void draw_circle(SDL_Renderer* renderer, int centerX, int centerY, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx*dx + dy*dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
}

void draw_circle1(SDL_Renderer* renderer, int center_x, int center_y, int radius) {
    for (int y = -radius; y <= radius; y++) {
        int width = (int)(sqrtf(radius * radius - y * y) + 0.5f);
        SDL_RenderDrawLine(
            renderer,
            center_x - width, center_y + y,
            center_x + width, center_y + y
        );
    }
}
