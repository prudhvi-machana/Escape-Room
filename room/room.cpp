#include <GL/glut.h>
#include <cmath>
#include "room.h"
#include "../utils/utils.h"

void initRoom() {
    worldItems.clear();

    worldItems.push_back(Item(
        "Door", 0.0f, 1.25f, 5.0f, 2.0f,
        "open",
        "The door is locked tight for now."
    ));
    worldItems.push_back(Item(
        "Desk Note", -2.95f, 1.22f, -3.05f, 1.0f,
        "read",
        "Note: Check the shelf. One book feels important."
    ));
    worldItems.push_back(Item(
        "Highlighted Book", 4.15f, 1.85f, -3.14f, 1.1f,
        "read",
        "Book note: 'The room rewards careful observation.'"
    ));
    worldItems.push_back(Item(
        "Crumpled Paper", -1.26f, 0.46f, -4.17f, 0.9f,
        "inspect",
        "A thrown paper says: 'Draft idea discarded.'"
    ));
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
    bool  isOpen = door && door->pickedUp;

    glPushMatrix();
        glTranslatef(-1.0f, 0.0f, 5.0f);
        glRotatef(isOpen ? -90.0f : 0.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(1.0f, 0.0f, 0.0f);

        float brightness = isOpen ? 0.75f : 0.6f;
        drawQuad(
            -1,0,0,   1,0,0,   1,2.5,0,   -1,2.5,0,
            brightness, 0.3f, 0.1f
        );

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

        drawQuad(
            0.67f, 0.85f, 0.01f,
            0.73f, 0.85f, 0.01f,
            0.73f, 0.95f, 0.01f,
            0.67f, 0.95f, 0.01f,
            0.2f, 0.2f, 0.2f
        );

    glPopMatrix();

    drawQuad(
        -1.15f,0,4.99f,  -1.0f,0,4.99f,
        -1.0f,2.65f,4.99f,  -1.15f,2.65f,4.99f,
        0.3f, 0.18f, 0.05f
    );
    drawQuad(
        1.0f,0,4.99f,   1.15f,0,4.99f,
        1.15f,2.65f,4.99f,   1.0f,2.65f,4.99f,
        0.3f, 0.18f, 0.05f
    );
    drawQuad(
        -1.15f,2.5f,4.99f,   1.15f,2.5f,4.99f,
        1.15f,2.65f,4.99f,  -1.15f,2.65f,4.99f,
        0.3f, 0.18f, 0.05f
    );
}
