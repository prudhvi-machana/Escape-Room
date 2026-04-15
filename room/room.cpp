#include <GL/glut.h>
#include <cmath>
#include "room.h"
#include "../utils/utils.h"
#include "../utils/lighting.h"
#include "../objects/object_utils.h"

namespace {

void projectPointToFloorFromLight(
    float x, float y, float z,
    float lx, float ly, float lz,
    float& outX, float& outZ) {
    float denom = y - ly;
    if (fabsf(denom) < 1e-5f) {
        outX = x;
        outZ = z;
        return;
    }
    float t = (0.01f - ly) / denom;
    outX = lx + (x - lx) * t;
    outZ = lz + (z - lz) * t;
}

void drawDoorShadow(bool isOpen) {
    float minX = -1.0f;
    float maxX =  1.0f;
    float minZ =  4.99f;
    float maxZ =  5.01f;
    if (isOpen) {
        minX = -1.02f;
        maxX = -0.94f;
        minZ =  5.0f;
        maxZ =  7.0f;
    }

    const float lx = lighting::kCeilingLightX;
    const float ly = lighting::kCeilingLightY;
    const float lz = lighting::kCeilingLightZ;

    float sx1, sz1, sx2, sz2, sx3, sz3, sx4, sz4;
    projectPointToFloorFromLight(minX, 2.5f, maxZ, lx, ly, lz, sx1, sz1);
    projectPointToFloorFromLight(maxX, 2.5f, maxZ, lx, ly, lz, sx2, sz2);
    projectPointToFloorFromLight(maxX, 2.5f, minZ, lx, ly, lz, sx3, sz3);
    projectPointToFloorFromLight(minX, 2.5f, minZ, lx, ly, lz, sx4, sz4);

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    glColor4f(0.0f, 0.0f, 0.0f, 0.18f);
    glBegin(GL_QUADS);
        glVertex3f(sx1, 0.012f, sz1);
        glVertex3f(sx2, 0.012f, sz2);
        glVertex3f(sx3, 0.012f, sz3);
        glVertex3f(sx4, 0.012f, sz4);
    glEnd();

    glDepthMask(GL_TRUE);
    glPopAttrib();
}

void drawCeilingLightFixture() {
    // Metal mount.
    drawQuad(
        -0.28f, 3.98f, -0.28f,   0.28f, 3.98f, -0.28f,
         0.28f, 3.98f,  0.28f,  -0.28f, 3.98f,  0.28f,
        0.28f, 0.28f, 0.30f, TEX_METAL, 2.0f
    );

    drawQuad(
        -0.10f, 3.98f, -0.10f,   0.10f, 3.98f, -0.10f,
         0.10f, 3.76f, -0.10f,  -0.10f, 3.76f, -0.10f,
        0.20f, 0.20f, 0.22f, TEX_METAL, 2.5f
    );

    // Emissive panel where the ceiling light actually is.
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    if (roomLightOn) glColor3f(1.0f, 0.93f, 0.78f);
    else             glColor3f(0.20f, 0.19f, 0.16f);
    glBegin(GL_QUADS);
        glVertex3f(-0.24f, 3.74f, -0.24f);
        glVertex3f( 0.24f, 3.74f, -0.24f);
        glVertex3f( 0.24f, 3.74f,  0.24f);
        glVertex3f(-0.24f, 3.74f,  0.24f);
    glEnd();
    glPopAttrib();
}

void drawLightSwitch() {
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);

    // Soft backplate glow so the switch can be found before the room light is on.
    glColor3f(roomLightOn ? 0.42f : 0.26f, roomLightOn ? 0.40f : 0.24f, roomLightOn ? 0.36f : 0.20f);
    glBegin(GL_QUADS);
        glVertex3f(-4.985f, 0.98f, 1.06f);
        glVertex3f(-4.985f, 0.98f, 1.46f);
        glVertex3f(-4.985f, 1.57f, 1.46f);
        glVertex3f(-4.985f, 1.57f, 1.06f);
    glEnd();
    glPopAttrib();

    drawQuad(
        -4.99f, 1.05f, 1.10f,
        -4.99f, 1.05f, 1.42f,
        -4.99f, 1.50f, 1.42f,
        -4.99f, 1.50f, 1.10f,
        roomLightOn ? 0.82f : 0.92f,
        roomLightOn ? 0.82f : 0.90f,
        roomLightOn ? 0.80f : 0.78f,
        TEX_PLASTIC, 2.0f
    );

    float toggleTop = roomLightOn ? 1.40f : 1.28f;
    float toggleBottom = roomLightOn ? 1.20f : 1.08f;
    drawQuad(
        -4.98f, toggleBottom, 1.22f,
        -4.98f, toggleBottom, 1.30f,
        -4.98f, toggleTop,    1.30f,
        -4.98f, toggleTop,    1.22f,
        roomLightOn ? 0.92f : 0.55f,
        roomLightOn ? 0.90f : 0.55f,
        roomLightOn ? 0.82f : 0.58f,
        TEX_PLASTIC, 3.0f
    );

    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glColor3f(roomLightOn ? 0.96f : 0.82f, roomLightOn ? 0.90f : 0.76f, roomLightOn ? 0.72f : 0.62f);
    glBegin(GL_LINES);
        glVertex3f(-4.984f, 1.05f, 1.10f); glVertex3f(-4.984f, 1.05f, 1.42f);
        glVertex3f(-4.984f, 1.05f, 1.42f); glVertex3f(-4.984f, 1.50f, 1.42f);
        glVertex3f(-4.984f, 1.50f, 1.42f); glVertex3f(-4.984f, 1.50f, 1.10f);
        glVertex3f(-4.984f, 1.50f, 1.10f); glVertex3f(-4.984f, 1.05f, 1.10f);
    glEnd();
    glPopAttrib();
}

void drawCodeBox() {
    const float outerMinX = -4.42f;
    const float outerMaxX = -3.30f;
    const float outerMinY = 0.72f;
    const float outerMaxY = 1.18f;
    const float outerMinZ = 2.82f;
    const float outerMaxZ = 3.66f;

    const float innerMinX = -4.23f;
    const float innerMaxX = -3.49f;
    const float innerMinY = 0.84f;
    const float innerMaxY = 1.06f;
    const float innerMinZ = 3.02f;
    const float innerMaxZ = 3.40f;

    drawBox(outerMinX, outerMinY, outerMinZ, outerMaxX, outerMaxY, outerMaxZ,
            0.20f, 0.10f, 0.07f, TEX_WOOD_DARK, 3.2f);

    drawBox(outerMinX + 0.04f, outerMinY + 0.04f, outerMaxZ - 0.07f,
            outerMaxX - 0.04f, outerMaxY - 0.10f, outerMaxZ + 0.01f,
            0.14f, 0.12f, 0.10f, TEX_METAL, 4.0f);
    drawBox(outerMaxX - 0.12f, 0.86f, outerMaxZ - 0.06f,
            outerMaxX - 0.02f, 1.05f, outerMaxZ + 0.02f,
            0.60f, 0.48f, 0.18f, TEX_METAL, 4.0f);

    drawBox(outerMinX + 0.12f, outerMinY + 0.10f, outerMinZ + 0.14f,
            innerMinX, innerMaxY, innerMaxZ + 0.02f,
            0.08f, 0.04f, 0.02f, TEX_WOOD_DARK, 2.2f);
    drawBox(innerMaxX, outerMinY + 0.10f, outerMinZ + 0.14f,
            outerMaxX - 0.12f, innerMaxY, innerMaxZ + 0.02f,
            0.08f, 0.04f, 0.02f, TEX_WOOD_DARK, 2.2f);
    drawBox(innerMinX, outerMinY + 0.10f, outerMinZ + 0.14f,
            innerMaxX, innerMaxY, innerMinZ,
            0.08f, 0.04f, 0.02f, TEX_WOOD_DARK, 2.2f);
    drawBox(innerMinX, outerMinY + 0.10f, innerMaxZ,
            innerMaxX, innerMaxY, outerMaxZ - 0.18f,
            0.08f, 0.04f, 0.02f, TEX_WOOD_DARK, 2.2f);
    drawBox(innerMinX, outerMinY + 0.10f, innerMinZ,
            innerMaxX, innerMinY + 0.03f, innerMaxZ,
            0.16f, 0.05f, 0.03f, TEX_WOOD_DARK, 2.0f);

    drawQuad(
        innerMinX, innerMinY + 0.032f, innerMinZ,
        innerMaxX, innerMinY + 0.032f, innerMinZ,
        innerMaxX, innerMinY + 0.032f, innerMaxZ,
        innerMinX, innerMinY + 0.032f, innerMaxZ,
        0.10f, 0.05f, 0.03f, TEX_WOOD_DARK, 2.0f
    );

    if (codeBoxUnlocked) {
        glPushMatrix();
            glTranslatef(outerMinX + 0.12f, outerMaxY - 0.02f, outerMinZ + 0.14f);
            glRotatef(-108.0f, 1.0f, 0.0f, 0.0f);
            drawBox(0.0f, -0.02f, 0.0f, 0.88f, 0.05f, 0.66f,
                    0.62f, 0.22f, 0.14f, TEX_WOOD, 3.0f);
            drawBox(0.05f, 0.00f, 0.05f, 0.83f, 0.02f, 0.61f,
                    0.16f, 0.06f, 0.04f, TEX_WOOD_DARK, 2.6f);
        glPopMatrix();
    } else {
        drawBox(outerMinX + 0.12f, outerMaxY - 0.06f, outerMinZ + 0.12f,
                outerMaxX - 0.12f, outerMaxY, outerMaxZ - 0.12f,
                0.62f, 0.22f, 0.14f, TEX_WOOD, 3.0f);
    }

    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glColor3f(codeBoxUnlocked ? 0.22f : 0.88f,
              codeBoxUnlocked ? 0.86f : 0.22f,
              codeBoxUnlocked ? 0.36f : 0.16f);
    glBegin(GL_QUADS);
        glVertex3f(outerMaxX - 0.18f, 0.94f, outerMaxZ + 0.015f);
        glVertex3f(outerMaxX - 0.12f, 0.94f, outerMaxZ + 0.015f);
        glVertex3f(outerMaxX - 0.12f, 1.01f, outerMaxZ + 0.015f);
        glVertex3f(outerMaxX - 0.18f, 1.01f, outerMaxZ + 0.015f);
    glEnd();
    glPopAttrib();

    if (codeBoxUnlocked && !hasInInventory("Brass Key")) {
        drawBox(-4.10f, 0.89f, 3.16f, -3.80f, 0.92f, 3.21f,
                0.84f, 0.68f, 0.18f, TEX_METAL, 5.0f);
        drawBox(-3.82f, 0.90f, 3.155f, -3.73f, 0.94f, 3.215f,
                0.82f, 0.66f, 0.18f, TEX_METAL, 5.0f);
        drawBox(-3.80f, 0.905f, 3.170f, -3.75f, 0.93f, 3.200f,
                0.18f, 0.12f, 0.07f, TEX_NONE, 1.0f);
        drawBox(-4.02f, 0.92f, 3.13f, -3.98f, 0.95f, 3.16f,
                0.82f, 0.66f, 0.18f, TEX_METAL, 5.0f);
        drawBox(-4.08f, 0.92f, 3.205f, -4.03f, 0.95f, 3.255f,
                0.80f, 0.64f, 0.18f, TEX_METAL, 5.0f);
        drawBox(-4.15f, 0.92f, 3.205f, -4.10f, 0.95f, 3.280f,
                0.80f, 0.64f, 0.18f, TEX_METAL, 5.0f);
        drawBox(-4.18f, 0.885f, 3.155f, -4.10f, 0.925f, 3.215f,
                0.82f, 0.66f, 0.18f, TEX_METAL, 5.0f);
        drawBox(-4.165f, 0.895f, 3.170f, -4.115f, 0.915f, 3.200f,
                0.18f, 0.12f, 0.07f, TEX_NONE, 1.0f);
    }
}

} // namespace

void initRoom() {
    worldItems.clear();
    drawerOpen = false;
    codeBoxUnlocked = false;
    codeBoxInput.clear();

    worldItems.push_back(Item(
        "Door", 0.0f, 1.25f, 5.0f, 2.0f,
        "open",
        "The door is locked tight for now."
    ));
    worldItems.push_back(Item(
        "Highlighted Book", 4.15f, 1.85f, -3.14f, 1.1f,
        "read",
        "Book note: 'The room rewards careful observation.'"
    ));
    worldItems.push_back(Item(
        "Dustbin Paper", 4.50f, 0.38f, 4.52f, 1.0f,
        "inspect",
        "The paper says: 'Books belong on shelves... so why is one not?'"
    ));
    worldItems.push_back(Item(
        "Light Switch", -4.55f, 1.25f, 1.26f, 0.9f,
        "toggle",
        "Left click to flip the room light."
    ));
    worldItems.push_back(Item(
        "Desk Drawer", -2.90f, 0.84f, -2.30f, 1.15f,
        "open",
        "A shallow drawer is built into the table."
    ));
    worldItems.push_back(Item(
        "Drawer Book", -2.90f, 0.85f, -2.08f, 0.75f,
        "read",
        "The secret code to your destination is u4u03."
    ));
    worldItems.push_back(Item(
        "Code Box", -3.88f, 0.98f, 3.60f, 1.5f,
        "inspect",
        "A locked box waits for a three-letter code."
    ));
    worldItems.push_back(Item(
        "Brass Key", -3.87f, 1.12f, 3.16f, 1.05f,
        "pickup",
        ""
    ));
}

void drawRoom() {
    // Floor - dark grey
    drawQuad(
        -5,0,-5,   5,0,-5,   5,0,5,   -5,0,5,
        0.42f, 0.39f, 0.36f, TEX_FLOOR, 0.6f
    );
    // Ceiling - lighter grey
    drawQuad(
        -5,4,-5,   5,4,-5,   5,4,5,   -5,4,5,
        0.76f, 0.73f, 0.68f, TEX_PLASTER, 0.7f
    );
    // Back wall - blue-grey
    drawQuad(
        -5,0,-5,   5,0,-5,   5,4,-5,   -5,4,-5,
        0.58f, 0.60f, 0.72f, TEX_PLASTER, 0.8f
    );
    // Left wall - warm brown
    drawQuad(
        -5,0,-5,   -5,0,5,   -5,4,5,   -5,4,-5,
        0.62f, 0.48f, 0.34f, TEX_PLASTER, 0.8f
    );
    // Right wall - warm brown
    drawQuad(
        5,0,-5,   5,0,5,   5,4,5,   5,4,-5,
        0.62f, 0.48f, 0.34f, TEX_PLASTER, 0.8f
    );
    // Front wall left part
    drawQuad(
        -5,0,5,   -1,0,5,   -1,4,5,   -5,4,5,
        0.58f, 0.60f, 0.72f, TEX_PLASTER, 0.8f
    );
    // Front wall right part
    drawQuad(
        1,0,5,   5,0,5,   5,4,5,   1,4,5,
        0.58f, 0.60f, 0.72f, TEX_PLASTER, 0.8f
    );
    // Front wall top part (above door)
    drawQuad(
        -1,2.5,5,   1,2.5,5,   1,4,5,   -1,4,5,
        0.58f, 0.60f, 0.72f, TEX_PLASTER, 0.8f
    );

    drawCeilingLightFixture();
    drawLightSwitch();
    drawCodeBox();
}

void drawDoor() {
    Item* door = getItemByName("Door");
    bool  isOpen = door && door->pickedUp;
    drawDoorShadow(isOpen);

    glPushMatrix();
        glTranslatef(-1.0f, 0.0f, 5.0f);
        glRotatef(isOpen ? -90.0f : 0.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(1.0f, 0.0f, 0.0f);

        float brightness = isOpen ? 0.75f : 0.6f;
        drawQuad(
            -1,0,0,   1,0,0,   1,2.5,0,   -1,2.5,0,
            brightness, 0.3f, 0.1f, TEX_WOOD_DARK, 0.7f
        );

        float hr = isOpen ? 0.5f : 1.0f;
        float hg = isOpen ? 0.5f : 0.8f;
        float hb = isOpen ? 0.5f : 0.0f;
        drawQuad(
            0.6f, 1.1f, 0.01f,
            0.8f, 1.1f, 0.01f,
            0.8f, 1.3f, 0.01f,
            0.6f, 1.3f, 0.01f,
            hr, hg, hb, TEX_PLASTIC, 4.0f
        );

        drawQuad(
            0.67f, 0.85f, 0.01f,
            0.73f, 0.85f, 0.01f,
            0.73f, 0.95f, 0.01f,
            0.67f, 0.95f, 0.01f,
            0.2f, 0.2f, 0.2f, TEX_METAL, 4.0f
        );

    glPopMatrix();

    drawQuad(
        -1.15f,0,4.99f,  -1.0f,0,4.99f,
        -1.0f,2.65f,4.99f,  -1.15f,2.65f,4.99f,
        0.3f, 0.18f, 0.05f, TEX_WOOD_DARK, 0.8f
    );
    drawQuad(
        1.0f,0,4.99f,   1.15f,0,4.99f,
        1.15f,2.65f,4.99f,   1.0f,2.65f,4.99f,
        0.3f, 0.18f, 0.05f, TEX_WOOD_DARK, 0.8f
    );
    drawQuad(
        -1.15f,2.5f,4.99f,   1.15f,2.5f,4.99f,
        1.15f,2.65f,4.99f,  -1.15f,2.65f,4.99f,
        0.3f, 0.18f, 0.05f, TEX_WOOD_DARK, 0.8f
    );
}
