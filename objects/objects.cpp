#include <GL/glut.h>
#include "objects.h"
#include "../utils/utils.h"

// --- Table ---
void drawTable() {
    // Table top - light brown
    drawQuad(
        -1.5f,1.2f,-1.5f,   1.5f,1.2f,-1.5f,
        1.5f,1.2f, 1.5f,   -1.5f,1.2f, 1.5f,
        0.6f, 0.4f, 0.2f
    );
    // Table front edge
    drawQuad(
        -1.5f,1.0f, 1.5f,   1.5f,1.0f, 1.5f,
        1.5f,1.2f, 1.5f,   -1.5f,1.2f, 1.5f,
        0.5f, 0.3f, 0.1f
    );
    // Table back edge
    drawQuad(
        -1.5f,1.0f,-1.5f,   1.5f,1.0f,-1.5f,
        1.5f,1.2f,-1.5f,   -1.5f,1.2f,-1.5f,
        0.5f, 0.3f, 0.1f
    );
    // Table left edge
    drawQuad(
        -1.5f,1.0f,-1.5f,  -1.5f,1.0f, 1.5f,
        -1.5f,1.2f, 1.5f,  -1.5f,1.2f,-1.5f,
        0.5f, 0.3f, 0.1f
    );
    // Table right edge
    drawQuad(
        1.5f,1.0f,-1.5f,   1.5f,1.0f, 1.5f,
        1.5f,1.2f, 1.5f,   1.5f,1.2f,-1.5f,
        0.5f, 0.3f, 0.1f
    );

    // 4 legs
    // Front-left leg
    drawQuad(
        -1.3f,0.0f, 1.3f,  -1.1f,0.0f, 1.3f,
        -1.1f,1.0f, 1.3f,  -1.3f,1.0f, 1.3f,
        0.4f, 0.25f, 0.1f
    );
    // Front-right leg
    drawQuad(
        1.1f,0.0f, 1.3f,   1.3f,0.0f, 1.3f,
        1.3f,1.0f, 1.3f,   1.1f,1.0f, 1.3f,
        0.4f, 0.25f, 0.1f
    );
    // Back-left leg
    drawQuad(
        -1.3f,0.0f,-1.3f,  -1.1f,0.0f,-1.3f,
        -1.1f,1.0f,-1.3f,  -1.3f,1.0f,-1.3f,
        0.4f, 0.25f, 0.1f
    );
    // Back-right leg
    drawQuad(
        1.1f,0.0f,-1.3f,   1.3f,0.0f,-1.3f,
        1.3f,1.0f,-1.3f,   1.1f,1.0f,-1.3f,
        0.4f, 0.25f, 0.1f
    );
}

// --- Key (sitting on table) ---
void drawKey() {
    // Key head - gold circle-ish (using a quad for simplicity)
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
}

// --- Chest (in the corner) ---
void drawChest() {
    // Chest body - dark brown
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
    // Top face
    drawQuad(
        -4.5f,0.8f, 3.5f,  -3.0f,0.8f, 3.5f,
        -3.0f,0.8f, 4.5f,  -4.5f,0.8f, 4.5f,
        0.5f, 0.28f, 0.08f
    );
    // Chest lock - gold
    drawQuad(
        -3.85f,0.35f, 4.51f,  -3.65f,0.35f, 4.51f,
        -3.65f,0.55f, 4.51f,  -3.85f,0.55f, 4.51f,
        1.0f, 0.8f, 0.0f
    );
}