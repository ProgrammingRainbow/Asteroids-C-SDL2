#ifndef ASTEROID_H
#define ASTEROID_H

#include "main.h"

struct Asteroid {
        SDL_Renderer *renderer;
        SDL_Texture *images[ASTEROID_SIZES];
        SDL_Rect rects[ASTEROID_SIZES];
        double radii[ASTEROID_SIZES];
        SDL_Texture *image;
        SDL_Rect rect;
        double x_pos;
        double y_pos;
        double x_vel;
        double y_vel;
        struct A_Pos pos;
        double angle;
        double angle_rate;
        int size;
        struct Asteroid *next;
};

bool asteroid_new(struct Asteroid **asteroids, SDL_Renderer *renderer,
                  SDL_Texture *const images[], const SDL_Rect rects[],
                  const double radii[], bool reset);
bool asteroid_split(struct Asteroid **asteroids, struct Asteroid *a,
                    double angle);
void asteroids_free(struct Asteroid **asteroids);
void asteroids_update(struct Asteroid *a, double dt);
void asteroids_draw(const struct Asteroid *a);

#endif
