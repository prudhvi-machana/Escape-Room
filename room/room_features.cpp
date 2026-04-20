#include "room_internal.h"

#include <cmath>

#include "../objects/object_utils.h"
#include "../utils/lighting.h"
#include "../utils/utils.h"

namespace room_internal {

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

}  // namespace

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

void drawRoomTrim() {
    const float trimH = 0.14f;
    const float crownY = 3.82f;
    const float crownDepth = 0.18f;

    drawBox(-kRoomHalf, 0.0f, -kRoomHalf, kRoomHalf, trimH, -kRoomHalf + 0.12f,
            0.42f, 0.26f, 0.13f, TEX_WOOD_DARK, 1.8f);
    drawBox(-kRoomHalf, 0.0f, kRoomHalf - 0.12f, kRoomHalf, trimH, kRoomHalf,
            0.42f, 0.26f, 0.13f, TEX_WOOD_DARK, 1.8f);
    drawBox(-kRoomHalf, 0.0f, -kRoomHalf, -kRoomHalf + 0.12f, trimH, kRoomHalf,
            0.42f, 0.26f, 0.13f, TEX_WOOD_DARK, 1.8f);
    drawBox(kRoomHalf - 0.12f, 0.0f, -kRoomHalf, kRoomHalf, trimH, kRoomHalf,
            0.42f, 0.26f, 0.13f, TEX_WOOD_DARK, 1.8f);

    drawBox(-kRoomHalf, crownY, -kRoomHalf, kRoomHalf, kRoomHeight, -kRoomHalf + crownDepth,
            0.56f, 0.39f, 0.20f, TEX_WOOD, 2.0f);
    drawBox(-kRoomHalf, crownY, -kRoomHalf, -kRoomHalf + crownDepth, kRoomHeight, kRoomHalf,
            0.56f, 0.39f, 0.20f, TEX_WOOD, 2.0f);
    drawBox(kRoomHalf - crownDepth, crownY, -kRoomHalf, kRoomHalf, kRoomHeight, kRoomHalf,
            0.56f, 0.39f, 0.20f, TEX_WOOD, 2.0f);

    drawBox(-kRoomHalf, crownY, kFrontWallInnerZ - crownDepth, -1.0f, kRoomHeight, kFrontWallInnerZ,
            0.56f, 0.39f, 0.20f, TEX_WOOD, 2.0f);
    drawBox(1.0f, crownY, kFrontWallInnerZ - crownDepth, kRoomHalf, kRoomHeight, kFrontWallInnerZ,
            0.56f, 0.39f, 0.20f, TEX_WOOD, 2.0f);
    drawBox(-1.0f, crownY, kFrontWallInnerZ - crownDepth, 1.0f, kRoomHeight, kFrontWallInnerZ,
            0.56f, 0.39f, 0.20f, TEX_WOOD, 2.0f);
}

}  // namespace room_internal
