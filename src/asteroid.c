#include "asteroid.h"

void asteroid_reset(struct Asteroid *a);

bool asteroid_new(struct Asteroid **asteroids, SDL_Renderer *renderer,
                  SDL_Texture *const images[], const SDL_Rect rects[],
                  const double radii[], bool reset) {
    struct Asteroid *new_a = calloc(1, sizeof(struct Asteroid));
    if (new_a == NULL) {
        fprintf(stderr, "Error in calloc of new asteroid.\n");
        return false;
    }

    new_a->renderer = renderer;

    for (unsigned i = 0; i < ASTEROID_SIZES; i++) {
        new_a->images[i] = images[i];
        new_a->rects[i] = rects[i];
        new_a->radii[i] = radii[i];
    }

    new_a->image = new_a->images[0];
    new_a->rect = new_a->rects[0];
    new_a->pos.radius = new_a->radii[0];

    if (reset) {
        double center_x = (WINDOW_WIDTH - (double)new_a->rect.w) / 2.0;
        double center_y = (WINDOW_HEIGHT - (double)new_a->rect.h) / 2.0;
        while (true) {
            new_a->x_pos = rand() % WINDOW_WIDTH;
            new_a->y_pos = rand() % WINDOW_HEIGHT;
            if (hypot(center_x - new_a->x_pos, center_y - new_a->y_pos) > 150) {
                break;
            }
        }

        new_a->angle = rand() % 360;

        asteroid_reset(new_a);
    }

    new_a->next = *asteroids;
    *asteroids = new_a;

    return true;
}

void asteroids_free(struct Asteroid **asteroids) {
    if (*asteroids) {
        struct Asteroid *a = *asteroids;

        while (a) {
            for (unsigned short i = 0; i < ASTEROID_SIZES; i++) {
                a->images[i] = NULL;
                a->renderer = NULL;
            }
            struct Asteroid *next = a->next;
            free(a);
            a = next;
        }

        *asteroids = NULL;

        printf("Free Asteroids.\n");
    }
}

void asteroid_reset(struct Asteroid *a) {
    a->angle_rate += (((rand() % (200 * (a->size + 1))) / 100.0) - 1) * 60;
    double radian = a->angle * M_PI / 180;
    double speed = ((rand() % 100) / 100.0) * 60;
    a->x_vel += sin(radian) * speed;
    a->y_vel -= cos(radian) * speed;
}

bool asteroid_split(struct Asteroid **asteroids, struct Asteroid *a,
                    double angle) {
    a->size++;
    a->image = a->images[a->size];
    a->rect = a->rects[a->size];
    a->pos.radius = a->radii[a->size];
    a->x_pos = a->pos.center_x - a->pos.radius;
    a->y_pos = a->pos.center_y - a->pos.radius;
    a->rect.x = (int)(a->x_pos + 0.5);
    a->rect.y = (int)(a->y_pos + 0.5);
    a->pos.left = a->pos.center_x - a->pos.radius;
    a->pos.right = a->pos.center_x + a->pos.radius;
    a->pos.top = a->pos.center_y - a->pos.radius;
    a->pos.bottom = a->pos.center_y + a->pos.radius;

    if (!asteroid_new(asteroids, a->renderer, a->images, a->rects, a->radii,
                      false)) {
        return false;
    }

    (*asteroids)->image = a->image;
    (*asteroids)->rect = a->rect;
    (*asteroids)->pos = a->pos;
    (*asteroids)->x_pos = a->x_pos;
    (*asteroids)->y_pos = a->y_pos;
    (*asteroids)->size = a->size;
    (*asteroids)->angle = a->angle;

    angle += 90;
    if (angle > 360) {
        angle -= 360;
    }
    (*asteroids)->angle = angle;
    asteroid_reset(*asteroids);

    angle += 180;
    if (angle > 360) {
        angle -= 360;
    }
    a->angle = angle;
    asteroid_reset(a);

    return true;
}

void asteroids_update(struct Asteroid *a, double dt) {
    while (a != NULL) {
        a->angle += a->angle_rate * dt;
        if (a->angle > 360) {
            a->angle -= 360;
        } else if (a->angle < 0) {
            a->angle += 360;
        }

        a->x_pos += a->x_vel * dt;
        a->y_pos += a->y_vel * dt;

        if (a->x_pos + a->rect.w < 0) {
            a->x_pos = WINDOW_WIDTH;
        } else if (a->x_pos > WINDOW_WIDTH) {
            a->x_pos = -a->rect.w;
        }

        if (a->y_pos + a->rect.h < 0) {
            a->y_pos = WINDOW_HEIGHT;
        } else if (a->y_pos > WINDOW_HEIGHT) {
            a->y_pos = -a->rect.h;
        }

        a->rect.x = (int)(a->x_pos + 0.5);
        a->rect.y = (int)(a->y_pos + 0.5);
        a->pos.center_x = a->x_pos + (a->rect.w / 2.0);
        a->pos.center_y = a->y_pos + (a->rect.h / 2.0);
        a->pos.left = a->pos.center_x - a->pos.radius;
        a->pos.right = a->pos.center_x + a->pos.radius;
        a->pos.top = a->pos.center_y - a->pos.radius;
        a->pos.bottom = a->pos.center_y + a->pos.radius;

        a = a->next;
    }
}

void asteroids_draw(const struct Asteroid *a) {
    while (a != NULL) {
        SDL_RenderCopyEx(a->renderer, a->image, NULL, &a->rect, a->angle, NULL,
                         SDL_FLIP_NONE);
        a = a->next;
    }
}
