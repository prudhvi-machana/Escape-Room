#include <GL/glut.h>
#include "objects.h"
#include "object_utils.h"
#include "../utils/utils.h"

namespace {

void drawDeskLamp() {
    drawBox(-2.98f, 1.20f, -3.35f, -2.74f, 1.24f, -3.12f, 0.14f, 0.14f, 0.16f);
    drawBox(-2.90f, 1.24f, -3.26f, -2.84f, 1.49f, -3.20f, 0.18f, 0.18f, 0.20f);
    drawBox(-2.90f, 1.49f, -3.24f, -2.70f, 1.54f, -3.18f, 0.18f, 0.18f, 0.20f);

    drawBox(-2.72f, 1.48f, -3.36f, -2.58f, 1.60f, -3.14f, 0.22f, 0.22f, 0.24f);

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

void drawDrawerBook() {
    if (!drawerOpen) return;

    const float drawerOffset = 0.42f;

    drawBox(-3.18f, 0.79f, -2.78f + drawerOffset,
            -2.66f, 0.90f, -2.48f + drawerOffset,
            0.38f, 0.12f, 0.08f, TEX_BOOK, 3.0f);
    drawBox(-3.15f, 0.90f, -2.75f + drawerOffset,
            -2.69f, 0.915f, -2.51f + drawerOffset,
            0.86f, 0.82f, 0.74f, TEX_PAPER, 5.0f);

    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glColor3f(0.92f, 0.84f, 0.60f);
    glBegin(GL_LINES);
        glVertex3f(-3.10f, 0.916f, -2.70f + drawerOffset); glVertex3f(-2.75f, 0.916f, -2.70f + drawerOffset);
        glVertex3f(-3.08f, 0.916f, -2.64f + drawerOffset); glVertex3f(-2.77f, 0.916f, -2.64f + drawerOffset);
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

    drawQuad(
        -3.44f, 0.76f, -2.92f,
        -2.36f, 0.76f, -2.92f,
        -2.36f, 0.96f, -2.92f,
        -3.44f, 0.96f, -2.92f,
        0.10f, 0.06f, 0.04f, TEX_WOOD_DARK, 1.4f
    );

    drawBox(xLeft, yBottom, zBack, xRight, yBottom + 0.03f, zFront, 0.44f, 0.27f, 0.13f, TEX_WOOD, 1.8f);
    drawBox(xLeft, yBottom + 0.03f, zBack, xLeft + wallThickness, yTop, zFront, 0.46f, 0.28f, 0.14f, TEX_WOOD, 1.8f);
    drawBox(xRight - wallThickness, yBottom + 0.03f, zBack, xRight, yTop, zFront, 0.40f, 0.24f, 0.12f, TEX_WOOD, 1.8f);
    drawBox(xLeft + wallThickness, yBottom + 0.03f, zBack, xRight - wallThickness, yTop, zBack + wallThickness, 0.38f, 0.23f, 0.11f, TEX_WOOD, 1.8f);

    drawQuad(
        xLeft, yBottom, zFront,
        xRight, yBottom, zFront,
        xRight, yTop, zFront,
        xLeft, yTop, zFront,
        0.56f, 0.34f, 0.18f, TEX_WOOD_DARK, 1.5f
    );
    drawBox(-2.96f, 0.84f, zFront - 0.01f, -2.84f, 0.88f, zFront + 0.03f, 0.74f, 0.68f, 0.42f, TEX_METAL, 4.0f);

    drawDrawerBook();
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
