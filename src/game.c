#include "game.h"
#include "init_sdl.h"
#include "load_media.h"

bool game_reset(struct Game *g);
void game_shoot(struct Game *g);
Uint32 game_mode_event(Uint32 interval, void *param);
bool set_game_mode(struct Game *g);
bool check_ship_collisions(struct Game *g);
bool check_shot_collision(struct Game *g);
void game_toggle_fullscreen(struct Game *g);
void game_toggle_music(void);
bool game_events(struct Game *g);
bool game_update(struct Game *g);
void game_draw(const struct Game *g);

bool game_new(struct Game **game) {
    *game = calloc(1, sizeof(struct Game));
    if (*game == NULL) {
        fprintf(stderr, "Error in calloc of new game.\n");
        return false;
    }
    struct Game *g = *game;

    if (!game_init_sdl(g)) {
        return false;
    }

    if (!game_load_media(g)) {
        return false;
    }

    if (!ship_new(&g->ship, g->renderer, g->ship_image1, g->ship_image2,
                  g->thruster_sound)) {
        return false;
    }

    if (!shots_new(&g->shots, g->renderer, g->shot_image)) {
        return false;
    }

    if (!score_new(&g->score, g->renderer)) {
        return false;
    }

    if (!message_new(&g->message, g->renderer)) {
        return false;
    }

    if (!fps_new(&g->fps)) {
        return false;
    }

    g->is_running = true;
    g->life_rect.y = 10;
    g->lives = 3;

    srand((Uint32)time(NULL));

    return true;
}

void game_free(struct Game **game) {
    if (*game) {
        struct Game *g = *game;

        Mix_HaltMusic();
        Mix_HaltChannel(-1);

        if (g->fps) {
            fps_free(&g->fps);
        }
        if (g->message) {
            message_free(&g->message);
        }
        if (g->score) {
            score_free(&g->score);
        }
        if (g->shots) {
            shots_free(&g->shots);
        }
        if (g->asteroids) {
            asteroids_free(&g->asteroids);
        }
        if (g->ship) {
            ship_free(&g->ship);
        }

        if (g->thruster_sound) {
            Mix_FreeChunk(g->thruster_sound);
            g->thruster_sound = NULL;
        }
        if (g->laser_sound) {
            Mix_FreeChunk(g->laser_sound);
            g->laser_sound = NULL;
        }
        if (g->collide_sound) {
            Mix_FreeChunk(g->collide_sound);
            g->collide_sound = NULL;
        }
        if (g->boom_sound) {
            Mix_FreeChunk(g->boom_sound);
            g->boom_sound = NULL;
        }
        if (g->falling_music) {
            Mix_FreeMusic(g->falling_music);
            g->falling_music = NULL;
        }

        if (g->shot_image) {
            SDL_DestroyTexture(g->shot_image);
            g->shot_image = NULL;
        }
        if (g->life_image) {
            SDL_DestroyTexture(g->life_image);
            g->life_image = NULL;
        }
        for (unsigned i = 0; i < ASTEROID_SIZES; i++) {
            if (g->asteroid_images[i]) {
                SDL_DestroyTexture(g->asteroid_images[i]);
                g->asteroid_images[i] = NULL;
            }
        }
        if (g->ship_image1) {
            SDL_DestroyTexture(g->ship_image1);
            g->ship_image1 = NULL;
        }
        if (g->ship_image2) {
            SDL_DestroyTexture(g->ship_image2);
            g->ship_image2 = NULL;
        }
        if (g->background_image) {
            SDL_DestroyTexture(g->background_image);
            g->background_image = NULL;
        }

        if (g->renderer) {
            SDL_DestroyRenderer(g->renderer);
            g->renderer = NULL;
        }
        if (g->window) {
            SDL_DestroyWindow(g->window);
            g->window = NULL;
        }

        Mix_CloseAudio();
        Mix_Quit();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();

        free(g);
        g = NULL;
        *game = NULL;

        printf("All Clean!\n");
    }
}

bool game_reset(struct Game *g) {

    ship_reset(g->ship);

    shots_reset(g->shots);

    for (unsigned i = 0; i < g->level; i++) {
        if (!asteroid_new(&g->asteroids, g->renderer, g->asteroid_images,
                          g->asteroid_rects, g->asteroid_radii, true)) {
            return false;
        }
    }

    g->shot_count = 101;
    g->is_playing = true;

    return true;
}

void game_shoot(struct Game *g) {
    if (g->is_playing) {
        if (shots_fire(g->shots, g->ship->center_x, g->ship->center_y,
                       g->ship->angle, SHIP_RADIUS)) {
            Mix_PlayChannel(-1, g->laser_sound, 0);

            if (g->shot_count > 0) {
                g->shot_count--;
            }
        }
    }
}

Uint32 game_mode_event(Uint32 interval, void *param) {
    (void)interval;
    (void)param;
    SDL_Event event;
    event.type = GAME_MODE_EVENT;
    event.user.code = 0;
    event.user.data1 = NULL;
    event.user.data2 = NULL;
    SDL_PushEvent(&event);
    return 0;
}

bool set_game_mode(struct Game *g) {
    g->game_mode++;
    if (g->game_mode > 4) {
        g->game_mode = 1;
    }
    switch (g->game_mode) {
    case 1:
        g->lives = 0;
        g->level = 0;
        if (!message_update(g->message, "Asteroids!")) {
            return false;
        }
        if (!score_reset(g->score)) {
            return false;
        }
        g->show_game = false;
        g->show_message = true;
        break;
    case 2:
        g->lives++;
        g->level++;
        if (!message_update_level(g->message, g->level)) {
            return false;
        }
        g->show_game = false;
        g->show_message = true;
        break;
    case 3:
        if (!game_reset(g)) {
            return false;
        }
        g->show_game = true;
        g->show_message = false;
        break;
    case 4:
        if (!message_update(g->message, "Game Over")) {
            return true;
        }
        g->show_game = true;
        g->show_message = true;
        break;
    default:
        break;
    }

    if (g->game_mode != 3) {
        SDL_AddTimer(5000, game_mode_event, NULL);
    }

    return true;
}

bool check_ship_collisions(struct Game *g) {
    if (g->ship->immunity) {
        return true;
    }
    struct Asteroid *a = g->asteroids;
    while (a != NULL) {
        if (ship_check_collision(g->ship, &a->pos)) {
            Mix_PlayChannel(-1, g->collide_sound, 0);
            if (g->lives > 0) {
                g->lives--;
                ship_set_immunity(g->ship);
            } else {
                g->is_playing = false;
                ship_thruster(g->ship, false);
                if (set_game_mode(g)) {
                    return false;
                }
            }
            return true;
        }

        a = a->next;
    }

    return true;
}

bool check_shot_collision(struct Game *g) {
    struct Asteroid *a = g->asteroids;
    struct Asteroid *last = g->asteroids;
    while (a != NULL) {
        bool hit = false;
        double angle = 0;
        if (shots_check_collision(g->shots, &a->pos, &angle)) {
            Mix_PlayChannel(-1, g->boom_sound, 0);
            if (!score_increment(g->score, g->level * g->shot_count)) {
                return false;
            }
            if (a->size == 2) {
                if (a == last) {
                    g->asteroids = a->next;
                } else {
                    last->next = a->next;
                }
                struct Asteroid *old_a = a;
                a = a->next;
                free(old_a);
                hit = true;
                if (g->asteroids == NULL) {
                    g->game_mode = 1;
                    g->is_playing = false;
                    ship_thruster(g->ship, false);
                    if (!set_game_mode(g)) {
                        return false;
                    }
                }
            } else {
                if (!asteroid_split(&g->asteroids, a, angle)) {
                    return false;
                }
            }
        }
        if (!hit) {
            last = a;
            a = a->next;
        }
    }

    return true;
}

void game_toggle_fullscreen(struct Game *g) {
    Uint32 flags = SDL_GetWindowFlags(g->window);
    if (flags & SDL_WINDOW_FULLSCREEN ||
        flags & SDL_WINDOW_FULLSCREEN_DESKTOP) {
        SDL_SetWindowFullscreen(g->window, 0);
    } else {
        SDL_SetWindowFullscreen(g->window, SDL_WINDOW_FULLSCREEN);
    }
}

void game_toggle_music(void) {
    if (Mix_PausedMusic()) {
        Mix_ResumeMusic();
    } else {
        Mix_PauseMusic();
    }
}

bool game_events(struct Game *g) {
    while (SDL_PollEvent(&g->event)) {
        switch (g->event.type) {
        case SDL_QUIT:
            g->is_running = false;
            break;
        case IMMUNITY_EVENT:
            ship_set_immunity(g->ship);
            break;
        case GAME_MODE_EVENT:
            if (!set_game_mode(g)) {
                return false;
            }
            break;
        case SDL_KEYDOWN:
            switch (g->event.key.keysym.scancode) {
            case SDL_SCANCODE_ESCAPE:
                g->is_running = false;
                break;
            case SDL_SCANCODE_SPACE:
                game_shoot(g);
                break;
            case SDL_SCANCODE_F11:
                game_toggle_fullscreen(g);
                break;
            case SDL_SCANCODE_M:
                game_toggle_music();
                break;
            case SDL_SCANCODE_F:
                fps_toggle_display(g->fps);
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }

    return true;
}

bool game_update(struct Game *g) {
    if (!g->show_message) {
        ship_update(g->ship, g->delta_time);
        shots_update(g->shots, g->delta_time);
        asteroids_update(g->asteroids, g->delta_time);

        if (!check_ship_collisions(g)) {
            return false;
        }

        if (!check_shot_collision(g)) {
            return false;
        }
    }

    return true;
}

void game_draw(const struct Game *g) {
    SDL_RenderClear(g->renderer);

    SDL_RenderCopy(g->renderer, g->background_image, NULL, NULL);

    if (g->show_game) {
        asteroids_draw(g->asteroids);
        shots_draw(g->shots);

        if (g->lives > 0) {
            SDL_Rect rect = g->life_rect;
            for (Uint32 i = 0; i < g->lives; i++) {
                rect.x = (int)(i * 30 + 10);
                SDL_RenderCopy(g->renderer, g->life_image, NULL, &rect);
            }
        }

        ship_draw(g->ship);

        score_draw(g->score);
    }

    if (g->show_message) {
        message_draw(g->message);
    }

    SDL_RenderPresent(g->renderer);
}

bool game_run(struct Game *g) {
    if (!set_game_mode(g)) {
        return false;
    }

    if (Mix_PlayMusic(g->falling_music, -1)) {
        fprintf(stderr, "Error while playing music: %s\n", Mix_GetError());
        return false;
    }

    while (g->is_running) {

        if (!game_events(g)) {
            return false;
        }

        game_update(g);

        game_draw(g);

        g->delta_time = fps_update(g->fps);
    }

    return true;
}
