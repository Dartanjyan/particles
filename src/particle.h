#ifndef PARTICLE_H
#define PARTICLE_H

#include <SDL2/SDL.h>
struct Particle {
    cpBody *body;
    cpShape *shape;
    SDL_Color color;
    float radius;
};

#endif
