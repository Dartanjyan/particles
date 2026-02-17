#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <chipmunk/chipmunk.h>

#include "particle.h"
#include "random_funcs.h"
#include "time_mgmt.h"
#include "drawing.h"

#define FPS 60
#define M_PI 3.14159265358979323846

#define MAX_X 800
#define MAX_Y 600

#define STEP 0.05
#define GRAVITY 0, 0
#define PARTICLES 200

// ----- Protons
#define PROTON_SIZE 5
#define PROTON_COLOR 255, 64, 64
#define PROTON_MASS 1.67262192e-0  // 1.67262192e-27

// ----- Electrons
#define ELECTRON_SIZE 2
#define ELECTRON_COLOR 128, 128, 255
#define ELECTRON_MASS 9.1093837e-4  // 9.1093837e-31
// Chance for charged particle to be electron instead of proton
#define ELECTRON_CHANCE 0.8

#define RAND_VEL_MAX 20

#define RAND_SIZE_MAX 5
#define RAND_SIZE_MIN 3

#define RAND_DENSITY_MAX 5
#define RAND_DENSITY_MIN 0.1
#define RANDOM_DENSITY 0
#define DEFAULT_DENSITY 5
#define ELASTICITY 0.8

#ifdef _WIN32
#define main() int WinMain(int argc, char** argv)
#else
#define main() int main(int argc, char** argv)
#endif

enum Command {
    HELP,
    CHARGED,
    DEFAULT
};

void PrintHelp() {
    printf(
        "Usage:\n\t./evolution [charged|help|-h|--help]\n\n"
        "Options:\n"
        "\tcharged\t\tChange particles behaviour to charged particles.\n"
        "\t\t\tThey'll behave somewhat like protons and electrons\n"
        "\n\t-h/--help/help\tShow this help and exit.\n\n"
    );
}

void createDefaultParticles(cpSpace *space, struct Particle *particles) {
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
}

void createChargedParticles(cpSpace *space, struct Particle *particles) {
    for(size_t i = 0; i < PARTICLES; ++i) {
        // false means proton
        bool isElectron = (float)(rand() % (100 - 0 + 1) + 0) / 100 < ELECTRON_CHANCE;
        float radius;
        double mass;
        if (isElectron) {
            radius = ELECTRON_SIZE;
            mass = ELECTRON_MASS;
            SDL_Color col = {ELECTRON_COLOR};
            particles[i].color = col;
        } else {
            radius = PROTON_SIZE;
            mass = PROTON_MASS;
            SDL_Color col = {PROTON_COLOR};
            particles[i].color = col;
        }
        
        cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);

        cpBody *body = cpBodyNew(mass, moment);
        cpShape *shape = cpCircleShapeNew(body, radius, cpvzero);
        
        cpBodySetPosition(body, cpv(rand()%MAX_X, rand()%MAX_Y));
        cpBodySetVelocity(body, cpv(random_float(-RAND_VEL_MAX, RAND_VEL_MAX), random_float(-RAND_VEL_MAX, RAND_VEL_MAX)));
        cpShapeSetElasticity(shape, ELASTICITY);

        particles[i].body = body;
        particles[i].shape = shape;
        particles[i].radius = radius;

        cpSpaceAddBody(space, body);
        cpSpaceAddShape(space, shape);
    }
}

main() {
    bool chargedParticles = false;

    srand(time(NULL));
    enum Command commandType = DEFAULT;

    for (size_t i = 1; i < argc; i++) {
        char *word = argv[i];

        if (strcmp(word, "charged") == false) {
            commandType = CHARGED;
        } else if (strcmp(word, "help") == false || strcmp(word, "--help") == false || strcmp(word, "-h") == false) {
            commandType = HELP;
            break;
        } else {
            commandType = HELP;
        }
    }

    switch (commandType) {
        case HELP:
            PrintHelp();
            return 0;
        case CHARGED:
            chargedParticles = true;
            break;
        case DEFAULT:
        default:
            break;
    }

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
        } else {
            printf("Using software (CPU) rendering.\n");
        }
    } else {
        printf("Using accelerated (GPU) rendering.\n");
    }

    // chipmunk setup
    cpSpace* space = cpSpaceNew();
    if (space == NULL) {
        printf("Couldn't create space\n");
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
    if (chargedParticles) {
        createChargedParticles(space, &particles[0]);
    } else {
        createDefaultParticles(space, &particles[0]);
    }

    // Main cycle
    bool running = true;
    bool lmbPressed = false;
    size_t counter;
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
            if (pos.x > MAX_X || pos.x < 0 || pos.y > MAX_Y || pos.y < 0) {
                cpBodySetPosition(particles[i].body, cpv(MAX_X / 2, MAX_Y / 2));
                cpBodySetVelocity(particles[i].body, cpvmult(cpBodyGetVelocity(particles[i].body), (1.0/5)));
            }
        }

        SDL_RenderPresent(renderer);
        
        if(lmbPressed) {
            for(size_t i = 0; i < PARTICLES; ++i) {
                char buffer[512];
                // F = G/(r^2)
                const float G = chargedParticles ? 1e1 : 1e3;  // chargedParticles ? 1e-25 : 1e3;

                cpVect bodyPos = cpBodyGetPosition(particles[i].body);
                cpVect vector = cpvsub(mousePos, bodyPos);
                cpVect direction = cpvnormalize(vector);
                float distance = cpvlength(vector);
                float distance2 = distance*distance;
                float divided = G; //G/distance2;
                vector = cpvmult(direction, divided);
                cpBodyApplyForceAtWorldPoint(
                    particles[i].body,
                    cpvmult(vector, 1),
                    bodyPos
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

        long delta = tick(FPS);
        if (++counter >= 60) {
            char fpsBuf[64];
            snprintf(fpsBuf, sizeof(fpsBuf), "Colliding particles! FPS: %.0f", (double)1e9 / delta);
            SDL_SetWindowTitle(window, fpsBuf);
        }
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
