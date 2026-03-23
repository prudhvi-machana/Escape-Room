#include <GL/glut.h>
#include <cmath>
#include "objects.h"
#include "../utils/utils.h"

namespace {

void drawBox(float x1, float y1, float z1,
             float x2, float y2, float z2,
             float r, float g, float b) {
    // Front
    drawQuad(x1, y1, z2,  x2, y1, z2,  x2, y2, z2,  x1, y2, z2, r, g, b);
    // Back
    drawQuad(x2, y1, z1,  x1, y1, z1,  x1, y2, z1,  x2, y2, z1, r * 0.92f, g * 0.92f, b * 0.92f);
    // Left
    drawQuad(x1, y1, z1,  x1, y1, z2,  x1, y2, z2,  x1, y2, z1, r * 0.82f, g * 0.82f, b * 0.82f);
    // Right
    drawQuad(x2, y1, z2,  x2, y1, z1,  x2, y2, z1,  x2, y2, z2, r * 0.75f, g * 0.75f, b * 0.75f);
    // Top
    drawQuad(x1, y2, z2,  x2, y2, z2,  x2, y2, z1,  x1, y2, z1, r * 1.05f, g * 1.05f, b * 1.05f);
    // Bottom
    drawQuad(x1, y1, z1,  x2, y1, z1,  x2, y1, z2,  x1, y1, z2, r * 0.6f, g * 0.6f, b * 0.6f);
}

void drawBookshelfBooks() {
    // Bottom shelf
    drawBox(4.24f, 0.45f, -3.55f, 4.42f, 1.05f, -3.30f, 0.75f, 0.15f, 0.15f);
    drawBox(4.24f, 0.45f, -3.25f, 4.40f, 1.02f, -3.02f, 0.15f, 0.35f, 0.75f);
    drawBox(4.24f, 0.45f, -2.95f, 4.41f, 1.08f, -2.73f, 0.85f, 0.70f, 0.20f);
    drawBox(4.24f, 0.45f, -2.66f, 4.38f, 0.96f, -2.46f, 0.15f, 0.60f, 0.35f);
    drawBox(4.24f, 0.45f, -2.39f, 4.43f, 1.10f, -2.12f, 0.55f, 0.25f, 0.70f);

    // Middle shelf
    drawBox(4.24f, 1.45f, -3.58f, 4.39f, 2.02f, -3.35f, 0.90f, 0.45f, 0.18f);
    drawBox(4.24f, 1.45f, -3.28f, 4.42f, 2.10f, -3.00f, 0.20f, 0.55f, 0.80f);
    drawBox(4.24f, 1.45f, -2.92f, 4.37f, 1.92f, -2.72f, 0.80f, 0.25f, 0.25f);
    drawBox(4.24f, 1.45f, -2.66f, 4.40f, 2.08f, -2.42f, 0.25f, 0.70f, 0.45f);
    drawBox(4.24f, 1.45f, -2.34f, 4.42f, 1.98f, -2.08f, 0.78f, 0.72f, 0.28f);

    // Top shelf
    drawBox(4.24f, 2.42f, -3.50f, 4.39f, 2.95f, -3.25f, 0.75f, 0.22f, 0.18f);
    drawBox(4.24f, 2.42f, -3.19f, 4.40f, 3.02f, -2.95f, 0.18f, 0.35f, 0.70f);
    drawBox(4.24f, 2.42f, -2.88f, 4.42f, 3.08f, -2.60f, 0.80f, 0.68f, 0.22f);
    drawBox(4.24f, 2.42f, -2.52f, 4.38f, 2.90f, -2.31f, 0.20f, 0.60f, 0.38f);
    drawBox(4.24f, 2.42f, -2.24f, 4.43f, 3.05f, -1.96f, 0.62f, 0.28f, 0.78f);
}

} // namespace

// --- Table ---
void drawTable() {
    // Study table placed near the left-back corner.
    drawBox(-4.20f, 1.02f, -4.35f, -1.80f, 1.20f, -2.45f, 0.52f, 0.33f, 0.16f);

    // Table legs
    drawBox(-4.10f, 0.0f, -4.25f, -3.90f, 1.02f, -4.05f, 0.36f, 0.22f, 0.10f);
    drawBox(-2.10f, 0.0f, -4.25f, -1.90f, 1.02f, -4.05f, 0.36f, 0.22f, 0.10f);
    drawBox(-4.10f, 0.0f, -2.75f, -3.90f, 1.02f, -2.55f, 0.36f, 0.22f, 0.10f);
    drawBox(-2.10f, 0.0f, -2.75f, -1.90f, 1.02f, -2.55f, 0.36f, 0.22f, 0.10f);

    // Small drawer body under the tabletop
    drawBox(-3.55f, 0.60f, -4.10f, -2.25f, 0.98f, -3.70f, 0.43f, 0.27f, 0.12f);
    drawQuad(
        -3.45f, 0.72f, -3.69f,
        -2.35f, 0.72f, -3.69f,
        -2.35f, 0.90f, -3.69f,
        -3.45f, 0.90f, -3.69f,
        0.48f, 0.30f, 0.14f
    );
    drawBox(-2.95f, 0.78f, -3.67f, -2.85f, 0.84f, -3.61f, 0.85f, 0.75f, 0.35f);
}

void drawChair() {
    // Seat
    drawBox(-3.70f, 0.55f, -2.10f, -2.45f, 0.70f, -1.05f, 0.42f, 0.25f, 0.13f);

    // Chair legs
    drawBox(-3.62f, 0.0f, -2.02f, -3.48f, 0.55f, -1.88f, 0.30f, 0.18f, 0.08f);
    drawBox(-2.67f, 0.0f, -2.02f, -2.53f, 0.55f, -1.88f, 0.30f, 0.18f, 0.08f);
    drawBox(-3.62f, 0.0f, -1.27f, -3.48f, 0.55f, -1.13f, 0.30f, 0.18f, 0.08f);
    drawBox(-2.67f, 0.0f, -1.27f, -2.53f, 0.55f, -1.13f, 0.30f, 0.18f, 0.08f);

    // Backrest posts and panel
    drawBox(-3.62f, 0.70f, -2.02f, -3.46f, 1.60f, -1.88f, 0.30f, 0.18f, 0.08f);
    drawBox(-2.69f, 0.70f, -2.02f, -2.53f, 1.60f, -1.88f, 0.30f, 0.18f, 0.08f);
    drawBox(-3.58f, 1.10f, -2.04f, -2.57f, 1.48f, -1.86f, 0.48f, 0.29f, 0.15f);
}

void drawDustbin() {
    // Compact dustbin beside the table.
    drawBox(-1.55f, 0.0f, -4.55f, -0.95f, 0.72f, -3.95f, 0.18f, 0.20f, 0.24f);
    drawBox(-1.48f, 0.08f, -4.48f, -1.02f, 0.64f, -4.02f, 0.08f, 0.09f, 0.11f);

    // Front opening to keep it readable as a bin.
    drawQuad(
        -1.45f, 0.18f, -3.94f,
        -1.05f, 0.18f, -3.94f,
        -1.10f, 0.58f, -3.94f,
        -1.40f, 0.58f, -3.94f,
        0.04f, 0.05f, 0.06f
    );
}

void drawBookshelf() {
    // Bookshelf mounted against the right wall.
    drawBox(4.30f, 0.10f, -3.80f, 4.75f, 3.20f, -1.00f, 0.40f, 0.24f, 0.12f);

    // Dark back panel so the books read clearly against the wall.
    drawQuad(
        4.31f, 0.20f, -1.10f,
        4.31f, 0.20f, -3.70f,
        4.31f, 3.00f, -3.70f,
        4.31f, 3.00f, -1.10f,
        0.14f, 0.08f, 0.04f
    );

    // Shelves
    drawBox(4.33f, 0.35f, -3.72f, 4.72f, 0.45f, -1.08f, 0.46f, 0.28f, 0.14f);
    drawBox(4.33f, 1.35f, -3.72f, 4.72f, 1.45f, -1.08f, 0.46f, 0.28f, 0.14f);
    drawBox(4.33f, 2.32f, -3.72f, 4.72f, 2.42f, -1.08f, 0.46f, 0.28f, 0.14f);
    drawBox(4.33f, 3.05f, -3.72f, 4.72f, 3.15f, -1.08f, 0.46f, 0.28f, 0.14f);

    drawBookshelfBooks();
}

// --- Key (sitting on table) ---
void drawKey() {
    Item* key = getItemByName("Key");
    if (key && key->pickedUp) return;

    float t = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float bob = sinf(t * 2.0f) * 0.02f;

    glPushMatrix();
    glTranslatef(-3.0f, bob, -3.35f);

    // Key head
    drawQuad(
        -0.15f,1.25f, 0.0f,   0.15f,1.25f, 0.0f,
        0.15f,1.45f,  0.0f,  -0.15f,1.45f, 0.0f,
        1.0f, 0.8f, 0.0f
    );
    // Key stem
    drawQuad(
        -0.03f,1.05f, 0.0f,   0.03f,1.05f, 0.0f,
        0.03f,1.25f,  0.0f,  -0.03f,1.25f, 0.0f,
        1.0f, 0.8f, 0.0f
    );
    // Key tooth 1
    drawQuad(
        0.03f,1.10f, 0.0f,   0.09f,1.10f, 0.0f,
        0.09f,1.15f, 0.0f,   0.03f,1.15f, 0.0f,
        1.0f, 0.8f, 0.0f
    );
    // Key tooth 2
    drawQuad(
        0.03f,1.18f, 0.0f,   0.09f,1.18f, 0.0f,
        0.09f,1.23f, 0.0f,   0.03f,1.23f, 0.0f,
        1.0f, 0.8f, 0.0f
    );

    glPopMatrix();
}

// --- Chest ---
void drawChest() {
    Item* chest = getItemByName("Chest");
    bool  isUnlocked = chest && chest->pickedUp;

    drawQuad(
        -4.5f,0.0f, 4.5f,  -3.0f,0.0f, 4.5f,
        -3.0f,0.8f, 4.5f,  -4.5f,0.8f, 4.5f,
        0.4f, 0.2f, 0.05f
    );
    drawQuad(
        -4.5f,0.0f, 3.5f,  -3.0f,0.0f, 3.5f,
        -3.0f,0.8f, 3.5f,  -4.5f,0.8f, 3.5f,
        0.4f, 0.2f, 0.05f
    );
    drawQuad(
        -4.5f,0.0f, 3.5f,  -4.5f,0.0f, 4.5f,
        -4.5f,0.8f, 4.5f,  -4.5f,0.8f, 3.5f,
        0.35f, 0.18f, 0.05f
    );
    drawQuad(
        -3.0f,0.0f, 3.5f,  -3.0f,0.0f, 4.5f,
        -3.0f,0.8f, 4.5f,  -3.0f,0.8f, 3.5f,
        0.35f, 0.18f, 0.05f
    );

    glPushMatrix();
        glTranslatef(-3.75f, 0.8f, 3.5f);
        glRotatef(isUnlocked ? -90.0f : 0.0f, 1.0f, 0.0f, 0.0f);
        glTranslatef(0.0f, 0.0f, -0.5f);
        drawQuad(
            -0.75f, 0.0f,-0.5f,   0.75f, 0.0f,-0.5f,
            0.75f, 0.0f, 0.5f,  -0.75f, 0.0f, 0.5f,
            0.5f, 0.28f, 0.08f
        );
    glPopMatrix();

    float lr = isUnlocked ? 0.5f : 1.0f;
    float lg = isUnlocked ? 0.5f : 0.8f;
    float lb = isUnlocked ? 0.5f : 0.0f;
    drawQuad(
        -3.85f,0.35f, 4.51f,  -3.65f,0.35f, 4.51f,
        -3.65f,0.55f, 4.51f,  -3.85f,0.55f, 4.51f,
        lr, lg, lb
    );
}
