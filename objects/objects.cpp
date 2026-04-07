#include <GL/glut.h>
#include <cmath>
#include "objects.h"
#include "../utils/utils.h"
#include "../utils/lighting.h"

namespace {

struct ShelfBookSpec {
    float width;
    float height;
    float xFront;
    float xBack;
    float r;
    float g;
    float b;
};

void drawBox(float x1, float y1, float z1,
             float x2, float y2, float z2,
             float r, float g, float b,
             TextureId texture = TEX_GENERIC,
             float textureScale = 1.0f);

float measureShelfRowWidth(const ShelfBookSpec* books, int count, float gap) {
    float totalWidth = 0.0f;
    for (int i = 0; i < count; ++i) {
        totalWidth += books[i].width;
    }
    if (count > 1) {
        totalWidth += gap * static_cast<float>(count - 1);
    }
    return totalWidth;
}

void drawBook(float x1, float y1, float z1,
              float x2, float y2, float z2,
              float r, float g, float b)
{
    drawBox(x1, y1, z1, x2, y2, z2, r, g, b, TEX_BOOK, 1.8f);
}

void drawShelfRow(float yBottom, float centerZ, float gap,
                  const ShelfBookSpec* books, int count) {
    const float totalWidth = measureShelfRowWidth(books, count, gap);

    float zCursor = centerZ - totalWidth * 0.5f;
    for (int i = 0; i < count; ++i) {
        const ShelfBookSpec& book = books[i];
        const float z1 = zCursor;
        const float z2 = z1 + book.width;
        drawBook(
            book.xFront, yBottom, z1,
            book.xBack, yBottom + book.height, z2,
            book.r, book.g, book.b
        );
        zCursor = z2 + gap;
    }
}

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

void drawProjectedShadowQuad(
    float minX, float maxX,
    float minZ, float maxZ,
    float topY,
    float alpha,
    float lx, float ly, float lz)
{
    if (topY <= 0.02f) return;

    float sx1, sz1, sx2, sz2, sx3, sz3, sx4, sz4;
    projectPointToFloorFromLight(minX, topY, maxZ, lx, ly, lz, sx1, sz1);
    projectPointToFloorFromLight(maxX, topY, maxZ, lx, ly, lz, sx2, sz2);
    projectPointToFloorFromLight(maxX, topY, minZ, lx, ly, lz, sx3, sz3);
    projectPointToFloorFromLight(minX, topY, minZ, lx, ly, lz, sx4, sz4);

    glColor4f(0.0f, 0.0f, 0.0f, alpha);
    glBegin(GL_QUADS);
        glVertex3f(sx1, 0.012f, sz1);
        glVertex3f(sx2, 0.012f, sz2);
        glVertex3f(sx3, 0.012f, sz3);
        glVertex3f(sx4, 0.012f, sz4);
    glEnd();
}

void drawFloorShadowAABB(
    float minX, float maxX,
    float minZ, float maxZ,
    float topY,
    float alpha)
{
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    // Ceiling light contributes broad soft shadow.
    drawProjectedShadowQuad(
        minX, maxX, minZ, maxZ, topY, alpha,
        lighting::kCeilingLightX, lighting::kCeilingLightY, lighting::kCeilingLightZ
    );

    // Desk lamp adds a stronger local shadow around the desk area.
    bool nearDesk = (maxX < -1.5f && minX > -4.5f && maxZ < -1.8f && minZ > -4.8f);
    if (nearDesk) {
        drawProjectedShadowQuad(
            minX, maxX, minZ, maxZ, topY, alpha * 0.75f,
            lighting::kDeskLightX, lighting::kDeskLightY, lighting::kDeskLightZ
        );
    }

    glDepthMask(GL_TRUE);
    glPopAttrib();
}

void drawDeskLamp() {
    // Base and neck.
    drawBox(-2.98f, 1.20f, -3.35f, -2.74f, 1.24f, -3.12f, 0.14f, 0.14f, 0.16f);
    drawBox(-2.90f, 1.24f, -3.26f, -2.84f, 1.49f, -3.20f, 0.18f, 0.18f, 0.20f);
    drawBox(-2.90f, 1.49f, -3.24f, -2.70f, 1.54f, -3.18f, 0.18f, 0.18f, 0.20f);

    // Lamp head.
    drawBox(-2.72f, 1.48f, -3.36f, -2.58f, 1.60f, -3.14f, 0.22f, 0.22f, 0.24f);

    // Bulb glow (unlit geometry to always look emissive).
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    drawQuad(
        -2.69f, 1.50f, -3.33f,
        -2.61f, 1.50f, -3.33f,
        -2.61f, 1.50f, -3.17f,
        -2.69f, 1.50f, -3.17f,
        1.0f, 0.92f, 0.72f, TEX_NONE, 1.0f
    );
    glPopAttrib();
}

void drawBox(float x1, float y1, float z1,
             float x2, float y2, float z2,
             float r, float g, float b,
             TextureId texture,
             float textureScale) {
    drawQuad(x1, y1, z2,  x2, y1, z2,  x2, y2, z2,  x1, y2, z2, r, g, b, texture, textureScale);
    drawQuad(x2, y1, z1,  x1, y1, z1,  x1, y2, z1,  x2, y2, z1, r * 0.92f, g * 0.92f, b * 0.92f, texture, textureScale);
    drawQuad(x1, y1, z1,  x1, y1, z2,  x1, y2, z2,  x1, y2, z1, r * 0.82f, g * 0.82f, b * 0.82f, texture, textureScale);
    drawQuad(x2, y1, z2,  x2, y1, z1,  x2, y2, z1,  x2, y2, z2, r * 0.75f, g * 0.75f, b * 0.75f, texture, textureScale);
    drawQuad(x1, y2, z2,  x2, y2, z2,  x2, y2, z1,  x1, y2, z1, r * 1.05f, g * 1.05f, b * 1.05f, texture, textureScale);
    drawQuad(x1, y1, z1,  x2, y1, z1,  x2, y1, z2,  x1, y1, z2, r * 0.6f, g * 0.6f, b * 0.6f, texture, textureScale);
}

void drawBookshelfBooks() {
    static const ShelfBookSpec kBottomShelf[] = {
        {0.22f, 0.60f, 4.24f, 4.42f, 0.75f, 0.15f, 0.15f},
        {0.22f, 0.56f, 4.24f, 4.42f, 0.15f, 0.35f, 0.75f},
        {0.22f, 0.64f, 4.24f, 4.42f, 0.85f, 0.70f, 0.20f},
        {0.22f, 0.52f, 4.24f, 4.42f, 0.15f, 0.60f, 0.35f},
        {0.22f, 0.66f, 4.24f, 4.42f, 0.55f, 0.25f, 0.70f}
    };

    static const ShelfBookSpec kMiddleShelf[] = {
        {0.18f, 0.56f, 4.24f, 4.40f, 0.90f, 0.45f, 0.18f},
        {0.18f, 0.47f, 4.24f, 4.40f, 0.80f, 0.25f, 0.25f},
        {0.26f, 0.65f, 4.12f, 4.42f, 0.90f, 0.80f, 0.20f},
        {0.18f, 0.63f, 4.24f, 4.40f, 0.25f, 0.70f, 0.45f},
        {0.18f, 0.53f, 4.24f, 4.40f, 0.78f, 0.72f, 0.28f}
    };

    static const ShelfBookSpec kTopShelf[] = {
        {0.22f, 0.53f, 4.24f, 4.42f, 0.75f, 0.22f, 0.18f},
        {0.22f, 0.60f, 4.24f, 4.42f, 0.18f, 0.35f, 0.70f},
        {0.22f, 0.66f, 4.24f, 4.42f, 0.80f, 0.68f, 0.22f},
        {0.22f, 0.49f, 4.24f, 4.42f, 0.20f, 0.60f, 0.38f},
        {0.22f, 0.63f, 4.24f, 4.42f, 0.62f, 0.28f, 0.78f}
    };

    const float shelfCenterZ = -2.90f;
    const float shelfGap = 0.045f;
    const float middleShelfGap = 0.035f;

    drawShelfRow(0.45f, shelfCenterZ, shelfGap, kBottomShelf, 5);
    drawShelfRow(1.45f, shelfCenterZ, middleShelfGap, kMiddleShelf, 5);
    drawShelfRow(2.42f, shelfCenterZ, shelfGap, kTopShelf, 5);
}

void drawDrawerNote() {
    if (!drawerOpen) return;

    const float drawerOffset = 0.42f;

    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glColor3f(0.95f, 0.92f, 0.80f);
    glBegin(GL_QUADS);
        glVertex3f(-3.18f, 0.815f, -2.54f + drawerOffset);
        glVertex3f(-2.64f, 0.815f, -2.54f + drawerOffset);
        glVertex3f(-2.70f, 0.815f, -2.76f + drawerOffset);
        glVertex3f(-3.12f, 0.815f, -2.76f + drawerOffset);
    glEnd();
    glColor3f(0.40f, 0.38f, 0.32f);
    glBegin(GL_LINES);
        glVertex3f(-3.10f, 0.816f, -2.59f + drawerOffset); glVertex3f(-2.76f, 0.816f, -2.59f + drawerOffset);
        glVertex3f(-3.08f, 0.816f, -2.65f + drawerOffset); glVertex3f(-2.79f, 0.816f, -2.65f + drawerOffset);
        glVertex3f(-3.03f, 0.816f, -2.71f + drawerOffset); glVertex3f(-2.83f, 0.816f, -2.71f + drawerOffset);
    glEnd();
    glPopAttrib();
}

void drawDeskDrawer() {
    const float drawerOffset = drawerOpen ? 0.42f : 0.0f;
    const float xLeft = -3.38f;
    const float xRight = -2.42f;
    const float yBottom = 0.78f;
    const float yTop = 0.94f;
    const float wallThickness = 0.05f;
    const float zBack = -2.90f + drawerOffset;
    const float zFront = -2.50f + drawerOffset;

    // Drawer cavity under the desk.
    drawQuad(
        -3.44f, 0.76f, -2.92f,
        -2.36f, 0.76f, -2.92f,
        -2.36f, 0.96f, -2.92f,
        -3.44f, 0.96f, -2.92f,
        0.10f, 0.06f, 0.04f, TEX_WOOD_DARK, 1.4f
    );

    // Drawer body as a hollow tray.
    drawBox(xLeft, yBottom, zBack, xRight, yBottom + 0.03f, zFront, 0.44f, 0.27f, 0.13f, TEX_WOOD, 1.8f);
    drawBox(xLeft, yBottom + 0.03f, zBack, xLeft + wallThickness, yTop, zFront, 0.46f, 0.28f, 0.14f, TEX_WOOD, 1.8f);
    drawBox(xRight - wallThickness, yBottom + 0.03f, zBack, xRight, yTop, zFront, 0.40f, 0.24f, 0.12f, TEX_WOOD, 1.8f);
    drawBox(xLeft + wallThickness, yBottom + 0.03f, zBack, xRight - wallThickness, yTop, zBack + wallThickness, 0.38f, 0.23f, 0.11f, TEX_WOOD, 1.8f);

    // Front panel and pull.
    drawQuad(
        xLeft, yBottom, zFront,
        xRight, yBottom, zFront,
        xRight, yTop, zFront,
        xLeft, yTop, zFront,
        0.56f, 0.34f, 0.18f, TEX_WOOD_DARK, 1.5f
    );
    drawBox(-2.96f, 0.84f, zFront - 0.01f, -2.84f, 0.88f, zFront + 0.03f, 0.74f, 0.68f, 0.42f, TEX_METAL, 4.0f);

    drawDrawerNote();
}

void drawCrumpledPaper() {
    drawBox(-1.34f, 0.28f, -4.28f, -1.16f, 0.44f, -4.10f, 0.86f, 0.84f, 0.78f);
    drawBox(-1.28f, 0.39f, -4.20f, -1.09f, 0.52f, -4.03f, 0.80f, 0.78f, 0.70f);
    drawBox(-1.41f, 0.38f, -4.24f, -1.24f, 0.50f, -4.08f, 0.90f, 0.88f, 0.84f);
}

void drawBedPillow() {
    drawBox(0.95f, 0.56f, -4.10f, 1.80f, 0.72f, -3.35f, 0.86f, 0.86f, 0.82f, TEX_FABRIC, 2.4f);
    drawBox(1.88f, 0.56f, -4.10f, 2.72f, 0.72f, -3.35f, 0.84f, 0.84f, 0.80f, TEX_FABRIC, 2.4f);
}

void drawBedBlanket() {
    drawBox(0.78f, 0.42f, -3.40f, 3.00f, 0.64f, -1.20f, 0.34f, 0.48f, 0.62f, TEX_FABRIC, 1.5f);
    drawBox(0.88f, 0.40f, -1.55f, 2.88f, 0.58f, -1.10f, 0.58f, 0.47f, 0.32f, TEX_FABRIC, 2.2f);
}

} // namespace

void drawTable() {
    drawFloorShadowAABB(-4.20f, -1.80f, -4.35f, -2.45f, 1.20f, 0.30f);

    drawBox(-4.20f, 1.02f, -4.35f, -1.80f, 1.20f, -2.45f, 0.52f, 0.33f, 0.16f, TEX_WOOD, 1.2f);

    drawBox(-4.10f, 0.0f, -4.25f, -3.90f, 1.02f, -4.05f, 0.36f, 0.22f, 0.10f, TEX_WOOD_DARK, 1.4f);
    drawBox(-2.10f, 0.0f, -4.25f, -1.90f, 1.02f, -4.05f, 0.36f, 0.22f, 0.10f, TEX_WOOD_DARK, 1.4f);
    drawBox(-4.10f, 0.0f, -2.75f, -3.90f, 1.02f, -2.55f, 0.36f, 0.22f, 0.10f, TEX_WOOD_DARK, 1.4f);
    drawBox(-2.10f, 0.0f, -2.75f, -1.90f, 1.02f, -2.55f, 0.36f, 0.22f, 0.10f, TEX_WOOD_DARK, 1.4f);

    drawBox(-3.55f, 0.60f, -4.10f, -2.25f, 0.98f, -3.70f, 0.43f, 0.27f, 0.12f, TEX_WOOD_DARK, 1.6f);
    drawQuad(
        -3.45f, 0.72f, -3.69f,
        -2.35f, 0.72f, -3.69f,
        -2.35f, 0.90f, -3.69f,
        -3.45f, 0.90f, -3.69f,
        0.48f, 0.30f, 0.14f, TEX_WOOD_DARK, 1.6f
    );
    drawDeskDrawer();

    drawDeskLamp();
}

void drawChair() {
    drawFloorShadowAABB(-3.68f, -2.50f, -2.95f, -1.90f, 1.60f, 0.24f);

    // Chair tucked closer to the desk and facing it.
    drawBox(-3.78f, 0.55f, -2.78f, -2.60f, 0.70f, -1.74f, 0.42f, 0.25f, 0.13f, TEX_FABRIC, 2.0f);

    drawBox(-3.70f, 0.0f, -2.70f, -3.56f, 0.55f, -2.56f, 0.30f, 0.18f, 0.08f, TEX_WOOD_DARK, 1.8f);
    drawBox(-2.82f, 0.0f, -2.70f, -2.68f, 0.55f, -2.56f, 0.30f, 0.18f, 0.08f, TEX_WOOD_DARK, 1.8f);
    drawBox(-3.70f, 0.0f, -1.96f, -3.56f, 0.55f, -1.82f, 0.30f, 0.18f, 0.08f, TEX_WOOD_DARK, 1.8f);
    drawBox(-2.82f, 0.0f, -1.96f, -2.68f, 0.55f, -1.82f, 0.30f, 0.18f, 0.08f, TEX_WOOD_DARK, 1.8f);

    // Backrest on the room-facing side so the chair points toward the desk.
    drawBox(-3.70f, 0.70f, -1.98f, -3.54f, 1.60f, -1.84f, 0.30f, 0.18f, 0.08f, TEX_WOOD_DARK, 1.8f);
    drawBox(-2.84f, 0.70f, -1.98f, -2.68f, 1.60f, -1.84f, 0.30f, 0.18f, 0.08f, TEX_WOOD_DARK, 1.8f);
    drawBox(-3.66f, 1.08f, -2.00f, -2.72f, 1.48f, -1.82f, 0.48f, 0.29f, 0.15f, TEX_FABRIC, 2.0f);

    drawBox(-3.52f, 0.82f, -2.04f, -2.86f, 1.08f, -1.90f, 0.54f, 0.34f, 0.18f, TEX_FABRIC, 2.0f);
}

void drawDustbin() {
    drawFloorShadowAABB(-1.55f, -0.95f, -4.55f, -3.95f, 0.72f, 0.22f);

    drawBox(-1.55f, 0.0f, -4.55f, -0.95f, 0.72f, -3.95f, 0.18f, 0.20f, 0.24f, TEX_PLASTIC, 2.5f);
    drawBox(-1.48f, 0.08f, -4.48f, -1.02f, 0.64f, -4.02f, 0.08f, 0.09f, 0.11f, TEX_PLASTIC, 3.0f);

    drawQuad(
        -1.45f, 0.18f, -3.94f,
        -1.05f, 0.18f, -3.94f,
        -1.10f, 0.58f, -3.94f,
        -1.40f, 0.58f, -3.94f,
        0.04f, 0.05f, 0.06f, TEX_PLASTIC, 3.0f
    );

    drawCrumpledPaper();
}

void drawBookshelf() {
    static const ShelfBookSpec kBottomShelf[] = {
        {0.22f, 0.60f, 4.24f, 4.42f, 0.75f, 0.15f, 0.15f},
        {0.22f, 0.56f, 4.24f, 4.42f, 0.15f, 0.35f, 0.75f},
        {0.22f, 0.64f, 4.24f, 4.42f, 0.85f, 0.70f, 0.20f},
        {0.22f, 0.52f, 4.24f, 4.42f, 0.15f, 0.60f, 0.35f},
        {0.22f, 0.66f, 4.24f, 4.42f, 0.55f, 0.25f, 0.70f}
    };
    static const ShelfBookSpec kMiddleShelf[] = {
        {0.18f, 0.56f, 4.24f, 4.40f, 0.90f, 0.45f, 0.18f},
        {0.18f, 0.47f, 4.24f, 4.40f, 0.80f, 0.25f, 0.25f},
        {0.26f, 0.65f, 4.12f, 4.42f, 0.90f, 0.80f, 0.20f},
        {0.18f, 0.63f, 4.24f, 4.40f, 0.25f, 0.70f, 0.45f},
        {0.18f, 0.53f, 4.24f, 4.40f, 0.78f, 0.72f, 0.28f}
    };
    static const ShelfBookSpec kTopShelf[] = {
        {0.22f, 0.53f, 4.24f, 4.42f, 0.75f, 0.22f, 0.18f},
        {0.22f, 0.60f, 4.24f, 4.42f, 0.18f, 0.35f, 0.70f},
        {0.22f, 0.66f, 4.24f, 4.42f, 0.80f, 0.68f, 0.22f},
        {0.22f, 0.49f, 4.24f, 4.42f, 0.20f, 0.60f, 0.38f},
        {0.22f, 0.63f, 4.24f, 4.42f, 0.62f, 0.28f, 0.78f}
    };

    const float shelfGap = 0.045f;
    const float middleShelfGap = 0.035f;
    const float widestRow = std::fmax(
        measureShelfRowWidth(kBottomShelf, 5, shelfGap),
        std::fmax(
            measureShelfRowWidth(kMiddleShelf, 5, middleShelfGap),
            measureShelfRowWidth(kTopShelf, 5, shelfGap)
        )
    );

    const float sidePadding = 0.14f;
    const float outerFrame = 0.10f;
    const float shelfCenterZ = -2.90f;
    const float innerZ1 = shelfCenterZ - widestRow * 0.5f - sidePadding;
    const float innerZ2 = shelfCenterZ + widestRow * 0.5f + sidePadding;
    const float outerZ1 = innerZ1 - outerFrame;
    const float outerZ2 = innerZ2 + outerFrame;

    drawFloorShadowAABB(4.30f, 4.75f, outerZ1 - 0.06f, outerZ2 + 0.06f, 3.20f, 0.20f);

    drawBox(4.28f, 0.08f, outerZ1, 4.78f, 0.18f, outerZ2, 0.34f, 0.20f, 0.10f, TEX_WOOD_DARK, 1.5f);
    drawBox(4.28f, 3.12f, outerZ1, 4.78f, 3.22f, outerZ2, 0.34f, 0.20f, 0.10f, TEX_WOOD_DARK, 1.5f);
    drawBox(4.28f, 0.08f, outerZ1, 4.38f, 3.22f, outerZ2, 0.38f, 0.24f, 0.12f, TEX_WOOD_DARK, 1.5f);
    drawBox(4.68f, 0.08f, outerZ1, 4.78f, 3.22f, outerZ2, 0.38f, 0.24f, 0.12f, TEX_WOOD_DARK, 1.5f);

    drawQuad(
        4.31f, 0.18f, innerZ2,
        4.31f, 0.18f, innerZ1,
        4.31f, 3.12f, innerZ1,
        4.31f, 3.12f, innerZ2,
        0.16f, 0.09f, 0.05f, TEX_WOOD_DARK, 1.4f
    );

    drawBox(4.34f, 0.40f, innerZ1, 4.72f, 0.50f, innerZ2, 0.46f, 0.28f, 0.14f, TEX_WOOD, 1.8f);
    drawBox(4.34f, 1.28f, innerZ1, 4.72f, 1.38f, innerZ2, 0.46f, 0.28f, 0.14f, TEX_WOOD, 1.8f);
    drawBox(4.34f, 2.12f, innerZ1, 4.72f, 2.22f, innerZ2, 0.46f, 0.28f, 0.14f, TEX_WOOD, 1.8f);
    drawBox(4.34f, 2.94f, innerZ1, 4.72f, 3.04f, innerZ2, 0.46f, 0.28f, 0.14f, TEX_WOOD, 1.8f);

    drawBookshelfBooks();
}

void drawBed() {
    drawFloorShadowAABB(0.55f, 3.20f, -4.60f, -0.85f, 1.05f, 0.22f);

    // Raised frame with visible legs.
    drawBox(0.62f, 0.22f, -4.48f, 3.10f, 0.32f, -0.96f, 0.30f, 0.18f, 0.10f, TEX_WOOD_DARK, 1.4f);
    drawBox(0.62f, 0.32f, -4.48f, 0.78f, 0.56f, -0.96f, 0.32f, 0.20f, 0.10f, TEX_WOOD_DARK, 1.1f);
    drawBox(2.94f, 0.32f, -4.48f, 3.10f, 0.44f, -0.96f, 0.32f, 0.20f, 0.10f, TEX_WOOD_DARK, 1.1f);
    drawBox(0.78f, 0.32f, -4.48f, 0.90f, 0.40f, -0.96f, 0.30f, 0.18f, 0.10f, TEX_WOOD_DARK, 1.0f);
    drawBox(2.82f, 0.32f, -4.48f, 2.94f, 0.40f, -0.96f, 0.30f, 0.18f, 0.10f, TEX_WOOD_DARK, 1.0f);

    drawBox(0.68f, 0.00f, -4.40f, 0.84f, 0.22f, -4.24f, 0.26f, 0.15f, 0.08f, TEX_WOOD_DARK, 1.2f);
    drawBox(2.88f, 0.00f, -4.40f, 3.04f, 0.22f, -4.24f, 0.26f, 0.15f, 0.08f, TEX_WOOD_DARK, 1.2f);
    drawBox(0.68f, 0.00f, -1.20f, 0.84f, 0.22f, -1.04f, 0.26f, 0.15f, 0.08f, TEX_WOOD_DARK, 1.2f);
    drawBox(2.88f, 0.00f, -1.20f, 3.04f, 0.22f, -1.04f, 0.26f, 0.15f, 0.08f, TEX_WOOD_DARK, 1.2f);

    // Mattress.
    drawBox(0.76f, 0.36f, -4.36f, 2.96f, 0.56f, -1.08f, 0.80f, 0.80f, 0.76f, TEX_FABRIC, 1.8f);

    drawBedBlanket();
    drawBedPillow();
}

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
