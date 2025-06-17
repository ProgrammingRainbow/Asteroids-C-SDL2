#ifndef MAIN_H
#define MAIN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define SDL_FLAGS SDL_INIT_EVERYTHING
#define IMG_FLAGS IMG_INIT_PNG
#define MIX_FLAGS MIX_INIT_OGG
#define RENDERER_FLAGS SDL_RENDERER_ACCELERATED
#define CHUNK_SIZE 1024

#define WINDOW_TITLE "Asteroids!"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_SCALE 1.5

#define IMMUNITY_EVENT SDL_USEREVENT
#define GAME_MODE_EVENT SDL_USEREVENT + 1

#define SCORE_FONT_SIZE 32
#define MESSAGE_FONT_SIZE 120

#define SHOT_COUNT 5
#define SHOT_SPEED 600
#define SHOT_LIFE_TIME 40
#define SHOT_RADIUS 5

#define ASTEROID_SIZES 3

#define SHIP_ROT_RATE 300
#define SHIP_THRUST 360
#define SHIP_RADIUS 26

#define FPS_TARGET 60
#define FPS_MAX_DURATION 50

struct A_Pos {
        double left;
        double right;
        double top;
        double bottom;
        double center_x;
        double center_y;
        double radius;
};

#endif
