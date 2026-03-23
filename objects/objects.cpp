#include <GL/glut.h>
#include<cmath>
#include "objects.h"
#include "../utils/utils.h"

// --- Table ---
void drawTable() {
    // unchanged — table is not an interactable item
    drawQuad(
        -1.5f,1.2f,-1.5f,   1.5f,1.2f,-1.5f,
        1.5f,1.2f, 1.5f,   -1.5f,1.2f, 1.5f,
        0.6f, 0.4f, 0.2f
    );
    drawQuad(
        -1.5f,1.0f, 1.5f,   1.5f,1.0f, 1.5f,
        1.5f,1.2f, 1.5f,   -1.5f,1.2f, 1.5f,
        0.5f, 0.3f, 0.1f
    );
    drawQuad(
        -1.5f,1.0f,-1.5f,   1.5f,1.0f,-1.5f,
        1.5f,1.2f,-1.5f,   -1.5f,1.2f,-1.5f,
        0.5f, 0.3f, 0.1f
    );
    drawQuad(
        -1.5f,1.0f,-1.5f,  -1.5f,1.0f, 1.5f,
        -1.5f,1.2f, 1.5f,  -1.5f,1.2f,-1.5f,
        0.5f, 0.3f, 0.1f
    );
    drawQuad(
        1.5f,1.0f,-1.5f,   1.5f,1.0f, 1.5f,
        1.5f,1.2f, 1.5f,   1.5f,1.2f,-1.5f,
        0.5f, 0.3f, 0.1f
    );
    drawQuad(
        -1.3f,0.0f, 1.3f,  -1.1f,0.0f, 1.3f,
        -1.1f,1.0f, 1.3f,  -1.3f,1.0f, 1.3f,
        0.4f, 0.25f, 0.1f
    );
    drawQuad(
        1.1f,0.0f, 1.3f,   1.3f,0.0f, 1.3f,
        1.3f,1.0f, 1.3f,   1.1f,1.0f, 1.3f,
        0.4f, 0.25f, 0.1f
    );
    drawQuad(
        -1.3f,0.0f,-1.3f,  -1.1f,0.0f,-1.3f,
        -1.1f,1.0f,-1.3f,  -1.3f,1.0f,-1.3f,
        0.4f, 0.25f, 0.1f
    );
    drawQuad(
        1.1f,0.0f,-1.3f,   1.3f,0.0f,-1.3f,
        1.3f,1.0f,-1.3f,   1.1f,1.0f,-1.3f,
        0.4f, 0.25f, 0.1f
    );
}

// --- Key (sitting on table) ---
void drawKey() {
    // ✅ Skip rendering if already picked up
    Item* key = getItemByName("Key");
    if (key && key->pickedUp) return;

    // ✅ Gentle bob animation so it's visually obvious it's interactable
    float t = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float bob = sinf(t * 2.0f) * 0.02f;   // ±0.02 units vertical oscillation

    glPushMatrix();
    glTranslatef(0.0f, bob, 0.0f);

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
    bool  isUnlocked = chest && chest->pickedUp;   // "picked up" = opened/unlocked

    // Front face
    drawQuad(
        -4.5f,0.0f, 4.5f,  -3.0f,0.0f, 4.5f,
        -3.0f,0.8f, 4.5f,  -4.5f,0.8f, 4.5f,
        0.4f, 0.2f, 0.05f
    );
    // Back face
    drawQuad(
        -4.5f,0.0f, 3.5f,  -3.0f,0.0f, 3.5f,
        -3.0f,0.8f, 3.5f,  -4.5f,0.8f, 3.5f,
        0.4f, 0.2f, 0.05f
    );
    // Left face
    drawQuad(
        -4.5f,0.0f, 3.5f,  -4.5f,0.0f, 4.5f,
        -4.5f,0.8f, 4.5f,  -4.5f,0.8f, 3.5f,
        0.35f, 0.18f, 0.05f
    );
    // Right face
    drawQuad(
        -3.0f,0.0f, 3.5f,  -3.0f,0.0f, 4.5f,
        -3.0f,0.8f, 4.5f,  -3.0f,0.8f, 3.5f,
        0.35f, 0.18f, 0.05f
    );

    // ✅ Lid: tilts open 90° when unlocked
    glPushMatrix();
        glTranslatef(-3.75f, 0.8f, 3.5f);          // hinge point (back-top edge)
        glRotatef(isUnlocked ? -90.0f : 0.0f,       // rotate around X when open
                  1.0f, 0.0f, 0.0f);
        glTranslatef(0.0f, 0.0f, -0.5f);            // offset to lid centre
        drawQuad(
            -0.75f, 0.0f,-0.5f,   0.75f, 0.0f,-0.5f,
            0.75f, 0.0f, 0.5f,  -0.75f, 0.0f, 0.5f,
            0.5f, 0.28f, 0.08f
        );
    glPopMatrix();

    // ✅ Lock: gold when locked, grey when opened
    float lr = isUnlocked ? 0.5f : 1.0f;
    float lg = isUnlocked ? 0.5f : 0.8f;
    float lb = isUnlocked ? 0.5f : 0.0f;
    drawQuad(
        -3.85f,0.35f, 4.51f,  -3.65f,0.35f, 4.51f,
        -3.65f,0.55f, 4.51f,  -3.85f,0.55f, 4.51f,
        lr, lg, lb
    );
}