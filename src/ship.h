#ifndef SHIP_H
#define SHIP_H

#include "main.h"

struct Ship {
        SDL_Renderer *renderer;
        SDL_Texture *image1;
        SDL_Texture *image2;
        Mix_Chunk *thruster;
        int thruster_chan;
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
        bool engine;
        const Uint8 *keystate;
        bool immunity;
        double immunity_counter;
};

bool ship_new(struct Ship **ship, SDL_Renderer *renderer, SDL_Texture *image1,
              SDL_Texture *image2, Mix_Chunk *thruster);
void ship_free(struct Ship **ship);
void ship_reset(struct Ship *s);
Uint32 ship_immunity_event(Uint32 interval, void *param);
void ship_set_immunity(struct Ship *s);
bool ship_check_collision(struct Ship *s, const struct A_Pos *a_pos);
void ship_thruster(struct Ship *s, bool enable);
void ship_update(struct Ship *s, double dt);
void ship_draw(const struct Ship *s);

#endif
