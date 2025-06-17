#include "message.h"

bool message_new(struct Message **message, SDL_Renderer *renderer) {
    *message = calloc(1, sizeof(struct Message));
    if (*message == NULL) {
        fprintf(stderr, "Error in calloc of message!\n");
        return false;
    }
    struct Message *m = *message;

    m->renderer = renderer;
    m->color = (SDL_Color){255, 255, 255, 255};

    m->font = TTF_OpenFont("fonts/freesansbold.ttf", MESSAGE_FONT_SIZE);
    if (!m->font) {
        fprintf(stderr, "Error creating font: %s\n", TTF_GetError());
        return false;
    }

    return true;
}

void message_free(struct Message **message) {
    if (*message) {
        struct Message *m = *message;

        if (m->font) {
            TTF_CloseFont(m->font);
            m->font = NULL;
        }
        if (m->image) {
            SDL_DestroyTexture(m->image);
            m->image = NULL;
        }

        m->renderer = NULL;

        free(m);
        m = NULL;
        *message = NULL;

        printf("Free Message.\n");
    }
}

bool message_update(struct Message *m, const char *text) {
    if (m->image) {
        SDL_DestroyTexture(m->image);
        m->image = NULL;
    }

    SDL_Surface *surface = TTF_RenderText_Blended(m->font, text, m->color);
    if (!surface) {
        fprintf(stderr, "Error creating a surface: %s\n", SDL_GetError());
        return false;
    }

    m->image = SDL_CreateTextureFromSurface(m->renderer, surface);
    SDL_FreeSurface(surface);
    surface = NULL;
    if (!m->image) {
        fprintf(stderr, "Error creating a texture: %s\n", SDL_GetError());
        return false;
    }

    if (SDL_QueryTexture(m->image, NULL, NULL, &m->rect.w, &m->rect.h)) {
        fprintf(stderr, "Error while querying texture: %s\n", SDL_GetError());
        return false;
    }

    m->rect.x = (WINDOW_WIDTH - m->rect.w) / 2;
    m->rect.y = (WINDOW_HEIGHT - m->rect.h) / 2;

    return true;
}

bool message_update_level(struct Message *m, Uint32 level) {
    int length = snprintf(NULL, 0, "Level: %d", level) + 1;
    char level_str[length];
    snprintf(level_str, (size_t)length, "Level: %d", level);

    if (!message_update(m, level_str)) {
        return false;
    }
    return true;
}

void message_draw(const struct Message *m) {
    SDL_RenderCopy(m->renderer, m->image, NULL, &m->rect);
}
