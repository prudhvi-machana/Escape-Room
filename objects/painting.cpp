#include "objects.h"
#include "../utils/utils.h"

void drawPainting() {
    drawQuad(
        -1.35f, 1.35f, -4.99f,
        1.35f, 1.35f, -4.99f,
        1.35f, 3.15f, -4.99f,
        -1.35f, 3.15f, -4.99f,
        0.18f, 0.12f, 0.08f, TEX_WOOD_DARK, 1.1f
    );

    drawQuad(
        -1.12f, 1.58f, -4.98f,
        1.12f, 1.58f, -4.98f,
        1.12f, 2.92f, -4.98f,
        -1.12f, 2.92f, -4.98f,
        0.76f, 0.56f, 0.30f, TEX_BOOK, 0.9f
    );
}
