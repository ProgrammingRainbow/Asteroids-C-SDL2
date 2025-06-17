#ifndef GAME_H
#define GAME_H

#include "asteroid.h"
#include "fps.h"
#include "main.h"
#include "message.h"
#include "score.h"
#include "ship.h"
#include "shots.h"

struct Game {
        SDL_Event event;
        SDL_Window *window;
        SDL_Renderer *renderer;
        SDL_Texture *background_image;
        SDL_Texture *ship_image1;
        SDL_Texture *ship_image2;
        SDL_Texture *asteroid_images[ASTEROID_SIZES];
        SDL_Rect asteroid_rects[ASTEROID_SIZES];
        double asteroid_radii[ASTEROID_SIZES];
        SDL_Texture *life_image;
        SDL_Rect life_rect;
        SDL_Texture *shot_image;
        Mix_Music *falling_music;
        Mix_Chunk *boom_sound;
        Mix_Chunk *collide_sound;
        Mix_Chunk *laser_sound;
        Mix_Chunk *thruster_sound;
        struct Ship *ship;
        struct Asteroid *asteroids;
        struct Shots *shots;
        struct Score *score;
        struct Message *message;
        struct Fps *fps;
        bool is_playing;
        bool is_running;
        Uint32 lives;
        Uint32 shot_count;
        Uint32 level;
        int game_mode;
        bool show_game;
        bool show_message;
        double delta_time;
};

bool game_new(struct Game **game);
void game_free(struct Game **game);
bool game_run(struct Game *g);

#endif
