#include "load_media.h"

bool game_load_media(struct Game *g) {
    g->background_image = IMG_LoadTexture(g->renderer, "images/background.png");
    if (!g->background_image) {
        fprintf(stderr, "Error creating Texture: %s\n", IMG_GetError());
        return false;
    }

    g->ship_image1 = IMG_LoadTexture(g->renderer, "images/ship1.png");
    if (!g->ship_image1) {
        fprintf(stderr, "Error creating Texture: %s\n", IMG_GetError());
        return false;
    }

    g->ship_image2 = IMG_LoadTexture(g->renderer, "images/ship2.png");
    if (!g->ship_image2) {
        fprintf(stderr, "Error creating Texture: %s\n", IMG_GetError());
        return false;
    }

    const char *asteroid_str[ASTEROID_SIZES] = {"images/asteroid-large.png",
                                                "images/asteroid-medium.png",
                                                "images/asteroid-small.png"};

    for (unsigned i = 0; i < ASTEROID_SIZES; i++) {
        g->asteroid_images[i] = IMG_LoadTexture(g->renderer, asteroid_str[i]);
        if (!g->asteroid_images[i]) {
            fprintf(stderr, "Error creating Texture: %s\n", IMG_GetError());
            return false;
        }

        if (SDL_QueryTexture(g->asteroid_images[i], NULL, NULL,
                             &g->asteroid_rects[i].w,
                             &g->asteroid_rects[i].h)) {
            fprintf(stderr, "Error while querying texture: %s\n",
                    SDL_GetError());
            return false;
        }

        g->asteroid_radii[i] = g->asteroid_rects[i].w / 2.0;
    }

    g->life_image = IMG_LoadTexture(g->renderer, "images/life.png");
    if (!g->life_image) {
        fprintf(stderr, "Error creating Texture: %s\n", IMG_GetError());
        return false;
    }

    if (SDL_QueryTexture(g->life_image, NULL, NULL, &g->life_rect.w,
                         &g->life_rect.h)) {
        fprintf(stderr, "Error querying Texture: %s\n", SDL_GetError());
        return false;
    }

    g->shot_image = IMG_LoadTexture(g->renderer, "images/laser.png");
    if (!g->shot_image) {
        fprintf(stderr, "Error creating Texture: %s\n", IMG_GetError());
        return false;
    }

    g->falling_music = Mix_LoadMUS("music/falling.ogg");
    if (!g->falling_music) {
        fprintf(stderr, "Error loading Music: %s\n", Mix_GetError());
        return false;
    }

    g->boom_sound = Mix_LoadWAV("sounds/hit.ogg");
    if (!g->boom_sound) {
        fprintf(stderr, "Error loading Chunk: %s\n", Mix_GetError());
        return false;
    }

    g->collide_sound = Mix_LoadWAV("sounds/collide.ogg");
    if (!g->collide_sound) {
        fprintf(stderr, "Error loading Chunk: %s\n", Mix_GetError());
        return false;
    }

    g->laser_sound = Mix_LoadWAV("sounds/laser.ogg");
    if (!g->laser_sound) {
        fprintf(stderr, "Error loading Chunk: %s\n", Mix_GetError());
        return false;
    }

    g->thruster_sound = Mix_LoadWAV("sounds/thrusters.ogg");
    if (!g->thruster_sound) {
        fprintf(stderr, "Error loading Chunk: %s\n", Mix_GetError());
        return false;
    }

    return true;
}
