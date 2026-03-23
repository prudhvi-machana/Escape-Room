#include <GL/glut.h>
#include <cmath>
#include "room.h"
#include "../utils/utils.h"

void initRoom() {
    //          name      X      Y      Z     radius
    worldItems.push_back(Item("Key",   0.0f,  1.35f,  0.0f,  1.2f));  // on table
    worldItems.push_back(Item("Chest", -3.75f, 0.4f,  4.0f,  1.5f));  // corner
    worldItems.push_back(Item("Door",   0.0f,  1.25f, 5.0f,  2.0f));  // front wall
}

void drawRoom() {
    // Floor - dark grey
    drawQuad(
        -5,0,-5,   5,0,-5,   5,0,5,   -5,0,5,
        0.3f, 0.3f, 0.3f
    );
    // Ceiling - lighter grey
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
    Item* door = getItemByName("Door");
    bool  isOpen = door && door->pickedUp;   // pickedUp = door has been opened

    // ✅ Swing door open around its left hinge (X = -1)
    glPushMatrix();
        glTranslatef(-1.0f, 0.0f, 5.0f);          // move to hinge point
        glRotatef(isOpen ? -90.0f : 0.0f,          // swing inward (negative Z direction)
                  0.0f, 1.0f, 0.0f);
        glTranslatef(1.0f, 0.0f, 0.0f);            // offset back to door centre

        // Door panel - dark wood, slightly brighter when open
        float brightness = isOpen ? 0.75f : 0.6f;
        drawQuad(
            -1,0,0,   1,0,0,   1,2.5,0,   -1,2.5,0,
            brightness, 0.3f, 0.1f
        );

        // Door handle - gold when locked, grey when open
        float hr = isOpen ? 0.5f : 1.0f;
        float hg = isOpen ? 0.5f : 0.8f;
        float hb = isOpen ? 0.5f : 0.0f;
        drawQuad(
            0.6f, 1.1f, 0.01f,
            0.8f, 1.1f, 0.01f,
            0.8f, 1.3f, 0.01f,
            0.6f, 1.3f, 0.01f,
            hr, hg, hb
        );

        // ✅ Keyhole below handle
        drawQuad(
            0.67f, 0.85f, 0.01f,
            0.73f, 0.85f, 0.01f,
            0.73f, 0.95f, 0.01f,
            0.67f, 0.95f, 0.01f,
            0.2f, 0.2f, 0.2f     // dark slot
        );

    glPopMatrix();

    // ✅ Door frame - always drawn, never moves
    // Left frame strip
    drawQuad(
        -1.15f,0,4.99f,  -1.0f,0,4.99f,
        -1.0f,2.65f,4.99f,  -1.15f,2.65f,4.99f,
        0.3f, 0.18f, 0.05f
    );
    // Right frame strip
    drawQuad(
        1.0f,0,4.99f,   1.15f,0,4.99f,
        1.15f,2.65f,4.99f,   1.0f,2.65f,4.99f,
        0.3f, 0.18f, 0.05f
    );
    // Top frame strip
    drawQuad(
        -1.15f,2.5f,4.99f,   1.15f,2.5f,4.99f,
        1.15f,2.65f,4.99f,  -1.15f,2.65f,4.99f,
        0.3f, 0.18f, 0.05f
    );
}