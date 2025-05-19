#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <chipmunk/chipmunk.h>

#include "particle.h"
#include "random_funcs.h"
#include "time_mgmt.h"
#include "drawing.h"

#define FPS 60

#define MAX_X 800
#define MAX_Y 600

#define STEP 0.05
#define GRAVITY 0, 0
#define PARTICLES 100

#define RAND_VEL_MAX 20

#define RAND_SIZE_MAX 5
#define RAND_SIZE_MIN 3

#define RAND_DENSITY_MAX 5
#define RAND_DENSITY_MIN 0.1
#define RANDOM_DENSITY 1
#define DEFAULT_DENSITY 5
#define ELASTICITY 0.8

int main(int argc, char** argv) {
    printf("Hello world!\n");
    srand(time(NULL));

    // SDL2
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Couldn't initialize SDL: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window* window = SDL_CreateWindow(
        "Colliding particles!",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        MAX_X,
        MAX_Y,
        SDL_WINDOW_SHOWN
    );
    if (window == NULL) {
        printf("Couldn't open a window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_Renderer* renderer;  
    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );
    printf("Trying to run accelerated renderer...\n");
    if (renderer == NULL) {
        printf("Couldn't use accelerated rendering (GPU): %s\n\nTrying to use software rendering (CPU)...", SDL_GetError());
        renderer = SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_SOFTWARE
        );
        if (renderer == NULL) {
            printf("Couldn't create software renderer: %s\n", SDL_GetError());
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        } 
    } 

    // chipmunk setup
    cpSpace* space = cpSpaceNew();
    if (space == NULL) {
        printf("Couldn't create space for unknown reason\n");
        return -1;
    }
    cpSpaceSetGravity(space, cpv(GRAVITY));
    cpBody* static_body = cpSpaceGetStaticBody(space);
    cpShape* walls[4] = {
        cpSegmentShapeNew(static_body, cpv(0, 0), cpv(MAX_X, 0), 5),
        cpSegmentShapeNew(static_body, cpv(MAX_X, 0), cpv(MAX_X, MAX_Y), 5),
        cpSegmentShapeNew(static_body, cpv(MAX_X, MAX_Y), cpv(0, MAX_Y), 5),
        cpSegmentShapeNew(static_body, cpv(0, MAX_Y), cpv(0, 0), 5)
    };

    for(uint8_t i = 0; i < 4; ++i) {
        cpSpaceAddShape(space, walls[i]);
        cpShapeSetElasticity(walls[i], ELASTICITY);
    }

    // particles
    struct Particle particles[PARTICLES];
    for(size_t i = 0; i < PARTICLES; ++i) {
        float radius = rand() % (RAND_SIZE_MAX - RAND_SIZE_MIN) + RAND_SIZE_MIN;
        float density;
        
        if(!RANDOM_DENSITY) {
            density = random_float(RAND_DENSITY_MIN, RAND_DENSITY_MAX);
        } else {
            density = DEFAULT_DENSITY;
        }
        float area = radius * radius * M_PI;
        cpFloat mass = area * density;
        cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);

        cpBody *body = cpBodyNew(mass, moment);
        cpShape *shape = cpCircleShapeNew(body, radius, cpvzero);
        
        cpBodySetPosition(body, cpv(rand()%MAX_X, rand()%MAX_Y));
        cpBodySetVelocity(body, cpv(random_float(-RAND_VEL_MAX, RAND_VEL_MAX), random_float(-RAND_VEL_MAX, RAND_VEL_MAX)));
        cpShapeSetElasticity(shape, ELASTICITY);

        particles[i].body = body;
        particles[i].shape = shape;
        particles[i].color = random_color();
        particles[i].radius = radius;

        cpSpaceAddBody(space, body);
        cpSpaceAddShape(space, shape);
    }

    // Main cycle
    bool running = true;
    bool lmbPressed = false;
    cpVect mousePos;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        lmbPressed = true;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        lmbPressed = false;
                    }
                    break;
                case SDL_QUIT:
                    running = false;
                    break;
            }
        }

        int mx, my;
        SDL_GetMouseState(&mx, &my);
        mousePos.x = mx;
        mousePos.y = my;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for(size_t i = 0; i < PARTICLES; ++i) {
            SDL_Color clr = particles[i].color;
            SDL_SetRenderDrawColor(renderer, clr.r, clr.g, clr.b, clr.a);
            cpVect pos = cpBodyGetPosition(particles[i].body);
            draw_circle(renderer, pos.x, pos.y, particles[i].radius);
        }

        SDL_RenderPresent(renderer);
        
        if(lmbPressed) {
            for(size_t i = 0; i < PARTICLES; ++i) {
                char buffer[512];
                // F = G/(r^2)
                cpVect bodyPos = cpBodyGetPosition(particles[i].body);
                cpVect vector = cpvsub(mousePos, bodyPos);
                cpVect direction = cpvnormalize(vector);
                float distance = cpvlength(vector);
                float distance2 = distance*distance;
                float divided = 1.0f;
                vector = cpvmult(direction, divided);
                vector = cpvmult(vector, 1e4);
                cpBodyApplyForceAtWorldPoint(
                    particles[i].body,
                    cpvmult(vector, 1),
                    cpBodyGetPosition(particles[i].body)
                );
                /*
                int written = snprintf(buffer, sizeof(buffer),
                        "Mouse pos: (%.2f, %.2f)\nBody pos: (%.2f, %.2f)\nDistance: %.2f, ^2=%.2f\n1/(distance^2): %f\nDirection: (%.2f, %.2f)\nForce = (%f, %f)",
                        mousePos.x, mousePos.y, bodyPos.x, bodyPos.y, distance, distance2, divided, direction.x, direction.y, vector.x, vector.y
                        );
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Debug", buffer, NULL);
                return -1;
                */
            }
        }

        cpSpaceStep(space, STEP);
        tick(FPS);
    }
    
    // Cleaning chipmunk
    for(size_t i = 0; i < PARTICLES; ++i) {
        cpSpaceRemoveShape(space, particles[i].shape);
        cpSpaceRemoveBody(space, particles[i].body);
        cpShapeFree(particles[i].shape);
        cpBodyFree(particles[i].body);
    }

    for(uint8_t i = 0; i < 4; ++i) {
        cpSpaceRemoveShape(space, walls[i]);
        cpShapeFree(walls[i]);
    }
    cpSpaceFree(space);

    // Cleaning SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
