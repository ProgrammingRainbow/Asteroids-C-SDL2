#include "score.h"

bool score_update(struct Score *s);

bool score_new(struct Score **score, SDL_Renderer *renderer) {

    *score = calloc(1, sizeof(struct Score));
    if (*score == NULL) {
        fprintf(stderr, "Error in calloc of score!\n");
        return false;
    }
    struct Score *s = *score;

    s->renderer = renderer;
    s->color = (SDL_Color){255, 255, 255, 255};

    s->font = TTF_OpenFont("fonts/freesansbold.ttf", SCORE_FONT_SIZE);
    if (!s->font) {
        fprintf(stderr, "Error creating font: %s\n", TTF_GetError());
        return true;
    }

    s->rect.y = 10;

    return true;
}

void score_free(struct Score **score) {
    if (*score) {
        struct Score *s = *score;

        if (s->font) {
            TTF_CloseFont(s->font);
            s->font = NULL;
        }
        if (s->image) {
            SDL_DestroyTexture(s->image);
            s->image = NULL;
        }

        s->renderer = NULL;

        free(s);
        s = NULL;
        *score = NULL;

        printf("Free Score.\n");
    }
}

bool score_update(struct Score *s) {
    if (s->image) {
        SDL_DestroyTexture(s->image);
        s->image = NULL;
    }

    int length = snprintf(NULL, 0, "Score: %d", s->score) + 1;
    char score_str[length];
    snprintf(score_str, (size_t)length, "Score: %d", s->score);

    SDL_Surface *surface = TTF_RenderText_Blended(s->font, score_str, s->color);
    if (!surface) {
        fprintf(stderr, "Error creating a surface: %s\n", SDL_GetError());
        return false;
    }

    s->image = SDL_CreateTextureFromSurface(s->renderer, surface);
    SDL_FreeSurface(surface);
    surface = NULL;
    if (!s->image) {
        fprintf(stderr, "Error creating a texture: %s\n", SDL_GetError());
        return false;
    }

    if (SDL_QueryTexture(s->image, NULL, NULL, &s->rect.w, &s->rect.h)) {
        fprintf(stderr, "Error while querying texture: %s\n", SDL_GetError());
        return false;
    }

    s->rect.x = WINDOW_WIDTH - s->rect.w - 20;

    return true;
}

bool score_reset(struct Score *s) {
    s->score = 0;
    if (!score_update(s)) {
        return false;
    }

    return true;
}

bool score_increment(struct Score *s, Uint32 new_score) {
    s->score += new_score;
    if (!score_update(s)) {
        return false;
    }

    return true;
}

void score_draw(const struct Score *s) {
    SDL_RenderCopy(s->renderer, s->image, NULL, &s->rect);
}
