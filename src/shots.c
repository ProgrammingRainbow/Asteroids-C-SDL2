#include "shots.h"

bool shots_new(struct Shots **shots, SDL_Renderer *renderer,
               SDL_Texture *image) {
    *shots = calloc(1, sizeof(struct Shots));
    if (*shots == NULL) {
        fprintf(stderr, "Error in Calloc of New Shot.\n");
        return false;
    }
    struct Shots *s = *shots;

    s->renderer = renderer;
    s->image = image;

    SDL_Rect rect = {0};
    if (SDL_QueryTexture(s->image, NULL, NULL, &rect.w, &rect.h)) {
        fprintf(stderr, "Error while querying texture: %s\n", SDL_GetError());
        return false;
    }

    for (unsigned i = 0; i < SHOT_COUNT; i++) {
        s->shots[i].rect = rect;
        s->shots[i].life_time = 0;
        s->shots[i].alive = false;
    }

    return true;
}

void shots_free(struct Shots **shots) {
    if (*shots) {
        struct Shots *s = *shots;

        s->image = NULL;
        s->renderer = NULL;

        free(s);
        s = NULL;
        *shots = NULL;

        printf("Free Shots.\n");
    }
}

void shots_reset(struct Shots *s) {
    for (unsigned i = 0; i < SHOT_COUNT; i++) {
        s->shots[i].alive = false;
    }
}

bool shots_fire(struct Shots *s, double x, double y, double angle,
                double radius) {
    for (unsigned i = 0; i < SHOT_COUNT; i++) {
        if (!s->shots[i].alive) {
            s->shots[i].angle = angle;
            double radian = angle * M_PI / 180;
            s->shots[i].x_vel = sin(radian) * SHOT_SPEED;
            s->shots[i].y_vel = -cos(radian) * SHOT_SPEED;
            s->shots[i].x_pos = x - (s->shots[i].rect.w / 2.0);
            s->shots[i].y_pos = y - (s->shots[i].rect.h / 2.0);
            s->shots[i].x_pos += sin(radian) * radius;
            s->shots[i].y_pos -= cos(radian) * radius;
            s->shots[i].life_time = 0;
            s->shots[i].alive = true;
            return true;
        }
    }

    return false;
}

bool shots_check_collision(struct Shots *s, const struct A_Pos *a_pos,
                           double *angle) {
    for (unsigned i = 0; i < SHOT_COUNT; i++) {
        if (s->shots[i].alive) {
            if (s->shots[i].right > a_pos->left) {
                if (s->shots[i].left < a_pos->right) {
                    if (s->shots[i].bottom > a_pos->top) {
                        if (s->shots[i].top < a_pos->bottom) {
                            double distance =
                                hypot(s->shots[i].center_x - a_pos->center_x,
                                      s->shots[i].center_y - a_pos->center_y);
                            if (distance < a_pos->radius + SHOT_RADIUS) {
                                s->shots[i].alive = false;
                                *angle = s->shots[i].angle;
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}

void shots_update(struct Shots *s, double dt) {
    for (unsigned i = 0; i < SHOT_COUNT; i++) {
        if (s->shots[i].alive) {
            s->shots[i].life_time += 60 * dt;
            if (s->shots[i].life_time > SHOT_LIFE_TIME) {
                s->shots[i].alive = false;
                return;
            }

            s->shots[i].x_pos += s->shots[i].x_vel * dt;
            s->shots[i].y_pos += s->shots[i].y_vel * dt;

            if (s->shots[i].x_pos + s->shots[i].rect.w < 0) {
                s->shots[i].x_pos = WINDOW_WIDTH;
            } else if (s->shots[i].x_pos > WINDOW_WIDTH) {
                s->shots[i].x_pos = -s->shots[i].rect.w;
            }

            if (s->shots[i].y_pos + s->shots[i].rect.h < 0) {
                s->shots[i].y_pos = WINDOW_HEIGHT;
            } else if (s->shots[i].y_pos > WINDOW_HEIGHT) {
                s->shots[i].y_pos = -s->shots[i].rect.h;
            }

            s->shots[i].rect.x = (int)s->shots[i].x_pos;
            s->shots[i].rect.y = (int)s->shots[i].y_pos;
            s->shots[i].center_x =
                s->shots[i].x_pos + (s->shots[i].rect.w / 2.0);
            s->shots[i].center_y =
                s->shots[i].y_pos + (s->shots[i].rect.h / 2.0);
            s->shots[i].left = s->shots[i].center_x - SHIP_RADIUS;
            s->shots[i].right = s->shots[i].center_x + SHIP_RADIUS;
            s->shots[i].top = s->shots[i].center_y - SHIP_RADIUS;
            s->shots[i].bottom = s->shots[i].center_y + SHIP_RADIUS;
        }
    }
}

void shots_draw(const struct Shots *s) {
    for (unsigned i = 0; i < SHOT_COUNT; i++) {
        if (s->shots[i].alive) {
            SDL_RenderCopyEx(s->renderer, s->image, NULL, &s->shots[i].rect,
                             s->shots[i].angle, NULL, SDL_FLIP_NONE);
        }
    }
}
