#include <GL/glut.h>
#include "room.h"
#include "../utils/utils.h"

void drawRoom() {
    // Floor - dark grey
    drawQuad(
        -5,0,-5,   5,0,-5,   5,0,5,   -5,0,5,
        0.3f, 0.3f, 0.3f
    );
    // Ceiling - slightly lighter grey
    drawQuad(
        -5,4,-5,   5,4,-5,   5,4,5,   -5,4,5,
        0.5f, 0.5f, 0.5f
    );
    // Back wall - blue-grey
    drawQuad(
        -5,0,-5,   5,0,-5,   5,4,-5,   -5,4,-5,
        0.4f, 0.4f, 0.6f
    );
    // Left wall - warm brown
    drawQuad(
        -5,0,-5,   -5,0,5,   -5,4,5,   -5,4,-5,
        0.5f, 0.35f, 0.2f
    );
    // Right wall - warm brown
    drawQuad(
        5,0,-5,   5,0,5,   5,4,5,   5,4,-5,
        0.5f, 0.35f, 0.2f
    );
    // Front wall left part
    drawQuad(
        -5,0,5,   -1,0,5,   -1,4,5,   -5,4,5,
        0.4f, 0.4f, 0.6f
    );
    // Front wall right part
    drawQuad(
        1,0,5,   5,0,5,   5,4,5,   1,4,5,
        0.4f, 0.4f, 0.6f
    );
    // Front wall top part (above door)
    drawQuad(
        -1,2.5,5,   1,2.5,5,   1,4,5,   -1,4,5,
        0.4f, 0.4f, 0.6f
    );
}

void drawDoor() {
    // Door - dark wood
    drawQuad(
        -1,0,5,   1,0,5,   1,2.5,5,   -1,2.5,5,
        0.6f, 0.3f, 0.1f
    );
    // Door handle - yellow
    drawQuad(
        0.6f, 1.1f, 5.01f,
        0.8f, 1.1f, 5.01f,
        0.8f, 1.3f, 5.01f,
        0.6f, 1.3f, 5.01f,
        1.0f, 0.8f, 0.0f
    );
}