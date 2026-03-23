#include <GL/glut.h>
#include <cmath>
#include "objects.h"
#include "../utils/utils.h"

namespace {

void drawBox(float x1, float y1, float z1,
             float x2, float y2, float z2,
             float r, float g, float b) {
    drawQuad(x1, y1, z2,  x2, y1, z2,  x2, y2, z2,  x1, y2, z2, r, g, b);
    drawQuad(x2, y1, z1,  x1, y1, z1,  x1, y2, z1,  x2, y2, z1, r * 0.92f, g * 0.92f, b * 0.92f);
    drawQuad(x1, y1, z1,  x1, y1, z2,  x1, y2, z2,  x1, y2, z1, r * 0.82f, g * 0.82f, b * 0.82f);
    drawQuad(x2, y1, z2,  x2, y1, z1,  x2, y2, z1,  x2, y2, z2, r * 0.75f, g * 0.75f, b * 0.75f);
    drawQuad(x1, y2, z2,  x2, y2, z2,  x2, y2, z1,  x1, y2, z1, r * 1.05f, g * 1.05f, b * 1.05f);
    drawQuad(x1, y1, z1,  x2, y1, z1,  x2, y1, z2,  x1, y1, z2, r * 0.6f, g * 0.6f, b * 0.6f);
}

void drawBookshelfBooks() {
    drawBox(4.24f, 0.45f, -3.55f, 4.42f, 1.05f, -3.30f, 0.75f, 0.15f, 0.15f);
    drawBox(4.24f, 0.45f, -3.25f, 4.40f, 1.02f, -3.02f, 0.15f, 0.35f, 0.75f);
    drawBox(4.24f, 0.45f, -2.95f, 4.41f, 1.08f, -2.73f, 0.85f, 0.70f, 0.20f);
    drawBox(4.24f, 0.45f, -2.66f, 4.38f, 0.96f, -2.46f, 0.15f, 0.60f, 0.35f);
    drawBox(4.24f, 0.45f, -2.39f, 4.43f, 1.10f, -2.12f, 0.55f, 0.25f, 0.70f);

    drawBox(4.24f, 1.45f, -3.58f, 4.39f, 2.02f, -3.35f, 0.90f, 0.45f, 0.18f);
    drawBox(4.24f, 1.45f, -2.92f, 4.37f, 1.92f, -2.72f, 0.80f, 0.25f, 0.25f);
    drawBox(4.24f, 1.45f, -2.66f, 4.40f, 2.08f, -2.42f, 0.25f, 0.70f, 0.45f);
    drawBox(4.24f, 1.45f, -2.34f, 4.42f, 1.98f, -2.08f, 0.78f, 0.72f, 0.28f);

    // Slightly pulled-out interactive book.
    drawBox(4.12f, 1.45f, -3.28f, 4.42f, 2.10f, -3.00f, 0.90f, 0.80f, 0.20f);

    drawBox(4.24f, 2.42f, -3.50f, 4.39f, 2.95f, -3.25f, 0.75f, 0.22f, 0.18f);
    drawBox(4.24f, 2.42f, -3.19f, 4.40f, 3.02f, -2.95f, 0.18f, 0.35f, 0.70f);
    drawBox(4.24f, 2.42f, -2.88f, 4.42f, 3.08f, -2.60f, 0.80f, 0.68f, 0.22f);
    drawBox(4.24f, 2.42f, -2.52f, 4.38f, 2.90f, -2.31f, 0.20f, 0.60f, 0.38f);
    drawBox(4.24f, 2.42f, -2.24f, 4.43f, 3.05f, -1.96f, 0.62f, 0.28f, 0.78f);
}

void drawDeskNote() {
    drawQuad(
        -3.22f, 1.215f, -2.88f,
        -2.75f, 1.215f, -2.98f,
        -2.66f, 1.215f, -3.22f,
        -3.14f, 1.215f, -3.12f,
        0.93f, 0.90f, 0.76f
    );

    drawQuad(
        -2.78f, 1.216f, -2.99f,
        -2.69f, 1.216f, -3.05f,
        -2.69f, 1.216f, -3.16f,
        -2.88f, 1.216f, -3.11f,
        0.40f, 0.40f, 0.45f
    );
}

void drawCrumpledPaper() {
    // A few brighter paper clumps that rise above the rim so they are easy to spot.
    drawBox(-1.38f, 0.36f, -4.30f, -1.21f, 0.56f, -4.12f, 0.92f, 0.90f, 0.84f);
    drawBox(-1.25f, 0.42f, -4.21f, -1.07f, 0.64f, -4.02f, 0.86f, 0.84f, 0.78f);
    drawBox(-1.43f, 0.48f, -4.17f, -1.28f, 0.71f, -4.01f, 0.96f, 0.94f, 0.88f);
    drawBox(-1.16f, 0.50f, -4.28f, -0.99f, 0.74f, -4.10f, 0.90f, 0.88f, 0.82f);
}

} // namespace

void drawTable() {
    drawBox(-4.20f, 1.02f, -4.35f, -1.80f, 1.20f, -2.45f, 0.52f, 0.33f, 0.16f);

    drawBox(-4.10f, 0.0f, -4.25f, -3.90f, 1.02f, -4.05f, 0.36f, 0.22f, 0.10f);
    drawBox(-2.10f, 0.0f, -4.25f, -1.90f, 1.02f, -4.05f, 0.36f, 0.22f, 0.10f);
    drawBox(-4.10f, 0.0f, -2.75f, -3.90f, 1.02f, -2.55f, 0.36f, 0.22f, 0.10f);
    drawBox(-2.10f, 0.0f, -2.75f, -1.90f, 1.02f, -2.55f, 0.36f, 0.22f, 0.10f);

    drawBox(-3.55f, 0.60f, -4.10f, -2.25f, 0.98f, -3.70f, 0.43f, 0.27f, 0.12f);
    drawQuad(
        -3.45f, 0.72f, -3.69f,
        -2.35f, 0.72f, -3.69f,
        -2.35f, 0.90f, -3.69f,
        -3.45f, 0.90f, -3.69f,
        0.48f, 0.30f, 0.14f
    );
    drawBox(-2.95f, 0.78f, -3.67f, -2.85f, 0.84f, -3.61f, 0.85f, 0.75f, 0.35f);

    drawDeskNote();
}

void drawChair() {
    // Chair tucked closer to the desk and facing it.
    drawBox(-3.68f, 0.55f, -2.95f, -2.50f, 0.70f, -1.90f, 0.42f, 0.25f, 0.13f);

    drawBox(-3.60f, 0.0f, -2.87f, -3.46f, 0.55f, -2.73f, 0.30f, 0.18f, 0.08f);
    drawBox(-2.72f, 0.0f, -2.87f, -2.58f, 0.55f, -2.73f, 0.30f, 0.18f, 0.08f);
    drawBox(-3.60f, 0.0f, -2.12f, -3.46f, 0.55f, -1.98f, 0.30f, 0.18f, 0.08f);
    drawBox(-2.72f, 0.0f, -2.12f, -2.58f, 0.55f, -1.98f, 0.30f, 0.18f, 0.08f);

    // Backrest on the room-facing side so the chair points toward the desk.
    drawBox(-3.60f, 0.70f, -2.08f, -3.44f, 1.60f, -1.94f, 0.30f, 0.18f, 0.08f);
    drawBox(-2.74f, 0.70f, -2.08f, -2.58f, 1.60f, -1.94f, 0.30f, 0.18f, 0.08f);
    drawBox(-3.56f, 1.08f, -2.10f, -2.62f, 1.48f, -1.92f, 0.48f, 0.29f, 0.15f);
}

void drawDustbin() {
    // Open-top bin with a slightly tapered silhouette.
    drawQuad(
        -1.58f, 0.0f, -4.58f,
        -0.92f, 0.0f, -4.58f,
        -0.98f, 0.0f, -3.92f,
        -1.52f, 0.0f, -3.92f,
        0.16f, 0.18f, 0.22f
    );

    // Back wall
    drawQuad(
        -1.55f, 0.0f, -4.55f,
        -0.95f, 0.0f, -4.55f,
        -1.02f, 0.88f, -4.48f,
        -1.48f, 0.88f, -4.48f,
        0.24f, 0.27f, 0.32f
    );
    // Left wall
    drawQuad(
        -1.55f, 0.0f, -4.55f,
        -1.52f, 0.0f, -3.92f,
        -1.45f, 0.82f, -4.00f,
        -1.48f, 0.88f, -4.48f,
        0.20f, 0.23f, 0.28f
    );
    // Right wall
    drawQuad(
        -0.98f, 0.0f, -3.92f,
        -0.95f, 0.0f, -4.55f,
        -1.02f, 0.88f, -4.48f,
        -1.05f, 0.82f, -4.00f,
        0.14f, 0.16f, 0.20f
    );
    // Front lip kept lower so the paper remains visible.
    drawQuad(
        -1.52f, 0.0f, -3.92f,
        -0.98f, 0.0f, -3.92f,
        -1.05f, 0.46f, -4.00f,
        -1.45f, 0.46f, -4.00f,
        0.19f, 0.21f, 0.25f
    );

    // Rim strips
    drawBox(-1.50f, 0.84f, -4.51f, -1.00f, 0.90f, -4.43f, 0.34f, 0.38f, 0.45f);
    drawBox(-1.50f, 0.76f, -4.05f, -1.42f, 0.82f, -4.45f, 0.30f, 0.34f, 0.40f);
    drawBox(-1.08f, 0.76f, -4.05f, -1.00f, 0.82f, -4.45f, 0.30f, 0.34f, 0.40f);

    // Dark inner cavity behind the visible paper.
    drawQuad(
        -1.42f, 0.18f, -4.42f,
        -1.08f, 0.18f, -4.42f,
        -1.10f, 0.70f, -4.36f,
        -1.40f, 0.70f, -4.36f,
        0.05f, 0.06f, 0.08f
    );

    drawCrumpledPaper();
}

void drawBookshelf() {
    drawBox(4.30f, 0.10f, -3.80f, 4.75f, 3.20f, -1.00f, 0.40f, 0.24f, 0.12f);

    drawQuad(
        4.31f, 0.20f, -1.10f,
        4.31f, 0.20f, -3.70f,
        4.31f, 3.00f, -3.70f,
        4.31f, 3.00f, -1.10f,
        0.14f, 0.08f, 0.04f
    );

    drawBox(4.33f, 0.35f, -3.72f, 4.72f, 0.45f, -1.08f, 0.46f, 0.28f, 0.14f);
    drawBox(4.33f, 1.35f, -3.72f, 4.72f, 1.45f, -1.08f, 0.46f, 0.28f, 0.14f);
    drawBox(4.33f, 2.32f, -3.72f, 4.72f, 2.42f, -1.08f, 0.46f, 0.28f, 0.14f);
    drawBox(4.33f, 3.05f, -3.72f, 4.72f, 3.15f, -1.08f, 0.46f, 0.28f, 0.14f);

    drawBookshelfBooks();
}
