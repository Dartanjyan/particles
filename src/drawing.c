#include "drawing.h"
#include <math.h>

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

/*
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
*/

void draw_vector_arrow(
    SDL_Renderer *renderer,
    float x, float y,
    float vx, float vy,
    float arrow_len
) {
    const float MAX_MAGNITUDE = 5;

    float mag = sqrtf(vx * vx + vy * vy);
    // printf("mag = %f\n", mag);
    if (mag <= 1e-6f) {
        return; // нулевой вектор — ничего не рисуем
    } else {
    }

    // --- нормализация направления ---
    float nx = vx / mag;
    float ny = vy / mag;

    // --- конец основной линии ---
    float ex = x + nx * arrow_len;
    float ey = y + ny * arrow_len;

    // --- цвет по модулю ---
    float t = mag / MAX_MAGNITUDE;
    if (t > 1.0f) t = 1.0f;

    Uint8 c = 255 - (Uint8)(t * 255.0f);
    SDL_SetRenderDrawColor(renderer, c, c, c, 255);

    // --- основная линия ---
    SDL_RenderDrawLineF(renderer, x, y, ex, ey);

    // --- наконечник стрелки ---
    const float head_len = arrow_len * 0.25f;
    const float head_angle = 0.5f; // радианы

    float sin_a = sinf(head_angle);
    float cos_a = cosf(head_angle);

    // левая линия
    float lx =  cos_a * nx - sin_a * ny;
    float ly =  sin_a * nx + cos_a * ny;

    // правая линия
    float rx =  cos_a * nx + sin_a * ny;
    float ry = -sin_a * nx + cos_a * ny;

    SDL_RenderDrawLineF(
        renderer,
        ex, ey,
        ex - lx * head_len,
        ey - ly * head_len
    );

    SDL_RenderDrawLineF(
        renderer,
        ex, ey,
        ex - rx * head_len,
        ey - ry * head_len
    );
}
