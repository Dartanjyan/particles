#ifndef PARTICLE_H
#define PARTICLE_H

#include <SDL2/SDL.h>
#include <chipmunk/chipmunk.h>
#include <stdint.h>

typedef enum ParticleType {
    PROTON,
    ELECTRON
} ParticleType;

struct Particle {
    cpBody *body;
    cpShape *shape;
    SDL_Color color;
    float radius;
    cpVect lastPos;
    uint8_t particleType;
};

#endif
