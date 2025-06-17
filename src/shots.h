#ifndef SHOT_H
#define SHOT_H

#include "main.h"

struct Shot {
        SDL_Rect rect;
        double x_pos;
        double y_pos;
        double x_vel;
        double y_vel;
        double left;
        double right;
        double top;
        double bottom;
        double center_x;
        double center_y;
        double angle;
        double life_time;
        bool alive;
};

struct Shots {
        SDL_Renderer *renderer;
        SDL_Texture *image;
        struct Shot shots[SHOT_COUNT];
};

bool shots_new(struct Shots **shots, SDL_Renderer *renderer,
               SDL_Texture *image);
void shots_free(struct Shots **shots);
bool shots_fire(struct Shots *s, double x, double y, double angle,
                double radius);
void shots_reset(struct Shots *s);
bool shots_check_collision(struct Shots *s, const struct A_Pos *a_pos,
                           double *angle);
void shots_update(struct Shots *s, double dt);
void shots_draw(const struct Shots *s);

#endif
