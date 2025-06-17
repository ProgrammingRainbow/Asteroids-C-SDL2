#include "ship.h"
#include <SDL2/SDL_mixer.h>

bool ship_new(struct Ship **ship, SDL_Renderer *renderer, SDL_Texture *image1,
              SDL_Texture *image2, Mix_Chunk *thruster) {
    *ship = calloc(1, sizeof(struct Ship));
    if (*ship == NULL) {
        fprintf(stderr, "Error in calloc of new ship.\n");
        return false;
    }
    struct Ship *s = *ship;

    s->renderer = renderer;
    s->image1 = image1;
    s->image2 = image2;
    s->thruster = thruster;
    s->thruster_chan = -1;

    if (SDL_QueryTexture(s->image1, NULL, NULL, &s->rect.w, &s->rect.h)) {
        fprintf(stderr, "Error while querying texture: %s\n", SDL_GetError());
        return false;
    }

    s->keystate = SDL_GetKeyboardState(NULL);

    return true;
}

void ship_free(struct Ship **ship) {
    if (*ship) {
        struct Ship *s = *ship;

        s->renderer = NULL;
        s->image1 = NULL;
        s->image2 = NULL;
        s->thruster = NULL;
        s->keystate = NULL;

        free(s);
        s = NULL;
        *ship = NULL;

        printf("Free Ship.\n");
    }
}

void ship_reset(struct Ship *s) {
    s->x_vel = 0;
    s->y_vel = 0;
    s->x_pos = (double)(WINDOW_WIDTH - s->rect.w) / 2;
    s->y_pos = (double)(WINDOW_HEIGHT - s->rect.h) / 2;
    s->rect.x = (int)(s->x_pos + 0.5);
    s->rect.y = (int)(s->y_pos + 0.5);
    s->angle = 0;
    ship_set_immunity(s);
}

Uint32 ship_immunity_event(Uint32 interval, void *param) {
    (void)interval;
    (void)param;
    SDL_Event event;
    event.type = IMMUNITY_EVENT;
    event.user.code = 0;
    event.user.data1 = NULL;
    event.user.data2 = NULL;
    SDL_PushEvent(&event);
    return 0;
}

void ship_set_immunity(struct Ship *s) {
    if (s->immunity) {
        s->immunity = false;
    } else {
        s->immunity = true;
        SDL_AddTimer(3000, ship_immunity_event, NULL);
    }
    s->immunity_counter = 0;
}

bool ship_check_collision(struct Ship *s, const struct A_Pos *a_pos) {
    if (s->right > a_pos->left) {
        if (s->left < a_pos->right) {
            if (s->bottom > a_pos->top) {
                if (s->top < a_pos->bottom) {
                    double distance = hypot(s->center_x - a_pos->center_x,
                                            s->center_y - a_pos->center_y);
                    if (distance < a_pos->radius + SHIP_RADIUS) {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

void ship_thruster(struct Ship *s, bool enable) {
    if (enable) {
        s->engine = true;
        if (s->thruster_chan == -1) {
            s->thruster_chan = Mix_FadeInChannel(-1, s->thruster, -1, 300);
        }
    } else {
        s->engine = false;
        if (s->thruster_chan != -1) {
            Mix_FadeOutChannel(s->thruster_chan, 300);
            s->thruster_chan = -1;
        }
    }
}

void ship_update(struct Ship *s, double dt) {
    if (s->keystate[SDL_SCANCODE_LEFT]) {
        s->angle -= SHIP_ROT_RATE * dt;
        if (s->angle < 0) {
            s->angle += 360;
        }
    }

    if (s->keystate[SDL_SCANCODE_RIGHT]) {
        s->angle += SHIP_ROT_RATE * dt;
        if (s->angle > 360) {
            s->angle -= 360;
        }
    }

    if (s->keystate[SDL_SCANCODE_UP]) {
        double radian = s->angle * M_PI / 180;
        s->x_vel += sin(radian) * SHIP_THRUST * dt;
        s->y_vel -= cos(radian) * SHIP_THRUST * dt;
        ship_thruster(s, true);
    } else {
        ship_thruster(s, false);
    }

    s->x_pos += s->x_vel * dt;
    s->y_pos += s->y_vel * dt;

    if (s->right < 0) {
        s->x_pos = WINDOW_WIDTH - (s->rect.w / 2.0) + SHIP_RADIUS;
    } else if (s->left > WINDOW_WIDTH) {
        s->x_pos = -(s->rect.w / 2.0) - SHIP_RADIUS;
    }

    if (s->bottom < 0) {
        s->y_pos = WINDOW_HEIGHT - (s->rect.h / 2.0) + SHIP_RADIUS;
    } else if (s->top > WINDOW_HEIGHT) {
        s->y_pos = -(s->rect.h / 2.0) - SHIP_RADIUS;
    }

    s->rect.x = (int)(s->x_pos + 0.5);
    s->rect.y = (int)(s->y_pos + 0.5);
    s->center_x = s->x_pos + (s->rect.w / 2.0);
    s->center_y = s->y_pos + (s->rect.h / 2.0);
    s->left = s->center_x - SHIP_RADIUS;
    s->right = s->center_x + SHIP_RADIUS;
    s->top = s->center_y - SHIP_RADIUS;
    s->bottom = s->center_y + SHIP_RADIUS;

    if (s->immunity) {
        s->immunity_counter += 60 * dt;
        if (s->immunity_counter > 9) {
            s->immunity_counter = 0;
        }
    }
}

void ship_draw(const struct Ship *s) {
    if (s->immunity_counter < 5) {
        SDL_Texture *image = NULL;
        if (s->engine) {
            image = s->image2;
        } else {
            image = s->image1;
        }
        SDL_RenderCopyEx(s->renderer, image, NULL, &s->rect, s->angle, NULL,
                         SDL_FLIP_NONE);
    }
}
