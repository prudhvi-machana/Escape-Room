#include "room_internal.h"

#include <cmath>

#include "../objects/object_utils.h"
#include "../utils/utils.h"

namespace room_internal {

namespace {

void drawBrassKeyModel(float keyX, float keyY, float keyZ) {
    const float shaftMinX = keyX - 0.24f;
    const float shaftMaxX = keyX + 0.06f;
    const float shaftMinY = keyY + 0.010f;
    const float shaftMaxY = keyY + 0.040f;
    const float shaftMinZ = keyZ - 0.018f;
    const float shaftMaxZ = keyZ + 0.018f;

    drawFloorShadowAABB(shaftMinX - 0.08f, keyX + 0.14f, keyZ - 0.10f, keyZ + 0.10f, keyY + 0.10f, 0.10f);

    drawBox(shaftMinX, shaftMinY, shaftMinZ, shaftMaxX, shaftMaxY, shaftMaxZ,
            0.93f, 0.74f, 0.23f, TEX_BRASS, 6.0f);

    drawBox(keyX - 0.30f, keyY + 0.002f, keyZ - 0.038f, keyX - 0.19f, keyY + 0.052f, keyZ + 0.038f,
            0.90f, 0.72f, 0.22f, TEX_BRASS, 5.5f);
    drawBox(keyX - 0.275f, keyY + 0.013f, keyZ - 0.020f, keyX - 0.215f, keyY + 0.041f, keyZ + 0.020f,
            0.22f, 0.17f, 0.10f, TEX_METAL, 4.0f);

    drawBox(keyX + 0.02f, keyY + 0.000f, keyZ - 0.048f, keyX + 0.12f, keyY + 0.058f, keyZ + 0.048f,
            0.92f, 0.74f, 0.24f, TEX_BRASS, 5.5f);
    drawBox(keyX + 0.095f, keyY + 0.012f, keyZ - 0.024f, keyX + 0.15f, keyY + 0.046f, keyZ + 0.024f,
            0.24f, 0.18f, 0.11f, TEX_METAL, 4.0f);

    drawBox(keyX - 0.15f, keyY + 0.040f, keyZ - 0.060f, keyX - 0.10f, keyY + 0.082f, keyZ - 0.010f,
            0.92f, 0.74f, 0.24f, TEX_BRASS, 6.0f);
    drawBox(keyX - 0.06f, keyY + 0.040f, keyZ + 0.006f, keyX + 0.00f, keyY + 0.086f, keyZ + 0.068f,
            0.92f, 0.74f, 0.24f, TEX_BRASS, 6.0f);
    drawBox(keyX - 0.01f, keyY + 0.040f, keyZ + 0.022f, keyX + 0.05f, keyY + 0.073f, keyZ + 0.092f,
            0.88f, 0.70f, 0.22f, TEX_BRASS, 6.0f);

    drawBox(keyX - 0.15f, keyY + 0.008f, keyZ - 0.018f, keyX - 0.10f, keyY + 0.022f, keyZ + 0.018f,
            0.70f, 0.56f, 0.20f, TEX_BRASS, 7.0f);
    drawBox(keyX - 0.02f, keyY + 0.008f, keyZ - 0.018f, keyX + 0.02f, keyY + 0.022f, keyZ + 0.018f,
            0.70f, 0.56f, 0.20f, TEX_BRASS, 7.0f);
}

}  // namespace

void drawCodeBox() {
    const float innerMinX = kChestMinX + kChestWall;
    const float innerMaxX = kChestMaxX - kChestWall;
    const float innerMinY = kChestMinY + kChestWall;
    const float innerMaxY = kChestMaxY - 0.16f;
    const float innerMinZ = kChestMinZ + kChestWall;
    const float innerMaxZ = kChestMaxZ - kChestWall;
    const float lidThickness = 0.10f;
    const float bodyTopY = kChestMaxY - 0.08f;
    const float bottomTopY = kChestMinY + 0.16f;
    const float openEase = codeBoxOpenProgress * codeBoxOpenProgress * (3.0f - 2.0f * codeBoxOpenProgress);
    const float settle = (1.0f - codeBoxOpenProgress) * std::sin(codeBoxOpenProgress * 7.2f) * 4.0f;
    const float lidAngle = 112.0f * openEase + settle;

    drawFloorShadowAABB(kChestMinX, kChestMaxX, kChestMinZ, kChestMaxZ, 1.70f, 0.24f);

    drawBox(kChestMinX, kChestMinY, kChestMinZ, kChestMaxX, bottomTopY, kChestMaxZ,
            0.30f, 0.18f, 0.08f, TEX_WOOD_DARK, 2.6f);

    drawBox(kChestMinX + 0.04f, kChestMinY + 0.04f, kChestMinZ + 0.04f,
            kChestMaxX - 0.04f, kChestMinY + 0.12f, kChestMaxZ - 0.04f,
            0.46f, 0.28f, 0.14f, TEX_WOOD, 2.3f);

    drawBox(kChestMinX, bottomTopY, kChestMinZ, innerMinX, bodyTopY, kChestMaxZ,
            0.25f, 0.14f, 0.07f, TEX_WOOD_DARK, 2.5f);
    drawBox(innerMaxX, bottomTopY, kChestMinZ, kChestMaxX, bodyTopY, kChestMaxZ,
            0.25f, 0.14f, 0.07f, TEX_WOOD_DARK, 2.5f);
    drawBox(innerMinX, bottomTopY, innerMaxZ, innerMaxX, bodyTopY, kChestMaxZ,
            0.27f, 0.15f, 0.08f, TEX_WOOD_DARK, 2.7f);
    drawBox(innerMinX, bottomTopY, kChestMinZ, innerMaxX, bodyTopY, innerMinZ,
            0.30f, 0.18f, 0.09f, TEX_WOOD, 2.7f);

    drawBox(kChestMinX + 0.10f, kChestMinY + 0.24f, kChestMinZ - 0.02f,
            kChestMaxX - 0.10f, kChestMaxY - 0.14f, kChestMinZ + 0.08f,
            0.22f, 0.18f, 0.12f, TEX_METAL, 4.0f);
    drawBox(kChestMinX + 0.16f, kChestMinY + 0.28f, kChestMinZ - 0.03f,
            kChestMaxX - 0.16f, kChestMinY + 0.52f, kChestMinZ + 0.02f,
            0.82f, 0.66f, 0.21f, TEX_BRASS, 5.0f);

    drawBox(kChestMinX + 0.12f, kChestMinY + 0.16f, kChestMaxZ - 0.10f,
            kChestMaxX - 0.12f, kChestMinY + 0.22f, kChestMaxZ + 0.01f,
            0.58f, 0.37f, 0.18f, TEX_WOOD, 3.2f);

    drawBox(kChestMinX + 0.08f, kChestMinY + 0.18f, kChestMinZ - 0.015f,
            kChestMinX + 0.18f, bodyTopY + 0.02f, kChestMinZ + 0.04f,
            0.82f, 0.66f, 0.22f, TEX_BRASS, 5.0f);
    drawBox(kChestMaxX - 0.18f, kChestMinY + 0.18f, kChestMinZ - 0.015f,
            kChestMaxX - 0.08f, bodyTopY + 0.02f, kChestMinZ + 0.04f,
            0.82f, 0.66f, 0.22f, TEX_BRASS, 5.0f);
    drawBox(kChestMinX + 0.12f, kChestMinY + 0.14f, kChestMaxZ - 0.02f,
            kChestMaxX - 0.12f, kChestMinY + 0.22f, kChestMaxZ + 0.04f,
            0.78f, 0.62f, 0.21f, TEX_BRASS, 5.0f);

    drawQuad(
        innerMinX, innerMinY + 0.02f, innerMinZ,
        innerMaxX, innerMinY + 0.02f, innerMinZ,
        innerMaxX, innerMinY + 0.02f, innerMaxZ,
        innerMinX, innerMinY + 0.02f, innerMaxZ,
        0.34f, 0.17f, 0.08f, TEX_WOOD, 3.0f
    );
    drawQuad(
        innerMinX + 0.03f, innerMinY + 0.024f, innerMinZ + 0.03f,
        innerMaxX - 0.03f, innerMinY + 0.024f, innerMinZ + 0.03f,
        innerMaxX - 0.03f, innerMinY + 0.024f, innerMaxZ - 0.03f,
        innerMinX + 0.03f, innerMinY + 0.024f, innerMaxZ - 0.03f,
        0.40f, 0.17f, 0.13f, TEX_FABRIC, 4.0f
    );

    drawQuad(
        innerMinX, innerMinY, innerMinZ,
        innerMinX, innerMinY, innerMaxZ,
        innerMinX, innerMaxY, innerMaxZ,
        innerMinX, innerMaxY, innerMinZ,
        0.13f, 0.07f, 0.05f, TEX_WOOD_DARK, 2.3f
    );
    drawQuad(
        innerMaxX, innerMinY, innerMaxZ,
        innerMaxX, innerMinY, innerMinZ,
        innerMaxX, innerMaxY, innerMinZ,
        innerMaxX, innerMaxY, innerMaxZ,
        0.14f, 0.07f, 0.05f, TEX_WOOD_DARK, 2.3f
    );
    drawQuad(
        innerMinX, innerMinY, innerMaxZ,
        innerMaxX, innerMinY, innerMaxZ,
        innerMaxX, innerMaxY, innerMaxZ,
        innerMinX, innerMaxY, innerMaxZ,
        0.15f, 0.08f, 0.05f, TEX_WOOD_DARK, 2.1f
    );
    drawQuad(
        innerMaxX, innerMinY, innerMinZ,
        innerMinX, innerMinY, innerMinZ,
        innerMinX, innerMaxY, innerMinZ,
        innerMaxX, innerMaxY, innerMinZ,
        0.17f, 0.09f, 0.05f, TEX_WOOD_DARK, 2.1f
    );

    glPushMatrix();
        glTranslatef(kChestMinX + 0.10f, bodyTopY, kChestMaxZ - 0.05f);
        glRotatef(lidAngle, 1.0f, 0.0f, 0.0f);

        drawBox(0.0f, -lidThickness * 0.5f, -(kChestMaxZ - kChestMinZ) + 0.12f,
                kChestMaxX - kChestMinX - 0.20f, lidThickness * 0.5f, 0.02f,
                0.48f, 0.28f, 0.14f, TEX_WOOD, 2.8f);
        drawBox(0.06f, -lidThickness * 0.22f, -(kChestMaxZ - kChestMinZ) + 0.18f,
                kChestMaxX - kChestMinX - 0.26f, -0.01f, -0.06f,
                0.18f, 0.08f, 0.05f, TEX_WOOD_DARK, 2.2f);
        drawBox(0.08f, -lidThickness * 0.18f, -(kChestMaxZ - kChestMinZ) + 0.22f,
                kChestMaxX - kChestMinX - 0.28f, -0.04f, -0.10f,
                0.52f, 0.20f, 0.16f, TEX_FABRIC, 4.0f);

        drawBox(0.01f, -lidThickness * 0.48f, -0.04f,
                0.10f, lidThickness * 0.34f, 0.03f,
                0.82f, 0.66f, 0.21f, TEX_BRASS, 5.0f);
        drawBox(kChestMaxX - kChestMinX - 0.30f, -lidThickness * 0.48f, -0.04f,
                kChestMaxX - kChestMinX - 0.21f, lidThickness * 0.34f, 0.03f,
                0.82f, 0.66f, 0.21f, TEX_BRASS, 5.0f);

        drawBox(0.14f, -lidThickness * 0.52f, -(kChestMaxZ - kChestMinZ) + 0.07f,
                kChestMaxX - kChestMinX - 0.34f, -lidThickness * 0.28f, -(kChestMaxZ - kChestMinZ) + 0.12f,
                0.78f, 0.62f, 0.20f, TEX_BRASS, 5.0f);
    glPopMatrix();

    drawBox(kChestMinX + 0.18f, bodyTopY - 0.02f, kChestMaxZ - 0.04f,
            kChestMinX + 0.30f, bodyTopY + 0.06f, kChestMaxZ + 0.03f,
            0.82f, 0.66f, 0.21f, TEX_BRASS, 5.0f);
    drawBox(kChestMaxX - 0.30f, bodyTopY - 0.02f, kChestMaxZ - 0.04f,
            kChestMaxX - 0.18f, bodyTopY + 0.06f, kChestMaxZ + 0.03f,
            0.82f, 0.66f, 0.21f, TEX_BRASS, 5.0f);

    drawBox((kChestMinX + kChestMaxX) * 0.5f - 0.12f, kChestMinY + 0.25f, kChestMinZ - 0.032f,
            (kChestMinX + kChestMaxX) * 0.5f + 0.12f, kChestMinY + 0.62f, kChestMinZ + 0.03f,
            0.82f, 0.66f, 0.21f, TEX_BRASS, 5.0f);
    drawBox((kChestMinX + kChestMaxX) * 0.5f - 0.04f, kChestMinY + 0.34f, kChestMinZ - 0.040f,
            (kChestMinX + kChestMaxX) * 0.5f + 0.04f, kChestMinY + 0.45f, kChestMinZ + 0.035f,
            0.16f, 0.16f, 0.16f, TEX_METAL, 5.0f);

    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glColor3f(codeBoxUnlocked ? 0.22f : 0.88f,
              codeBoxUnlocked ? 0.86f : 0.22f,
              codeBoxUnlocked ? 0.36f : 0.16f);
    glBegin(GL_QUADS);
        glVertex3f(kChestMaxX - 0.26f, kChestMinY + 0.40f, kChestMinZ - 0.026f);
        glVertex3f(kChestMaxX - 0.17f, kChestMinY + 0.40f, kChestMinZ - 0.026f);
        glVertex3f(kChestMaxX - 0.17f, kChestMinY + 0.49f, kChestMinZ - 0.026f);
        glVertex3f(kChestMaxX - 0.26f, kChestMinY + 0.49f, kChestMinZ - 0.026f);
    glEnd();
    glPopAttrib();

    if (codeBoxUnlocked && !hasInInventory("Brass Key")) {
        const float reveal = openEase < 0.35f ? 0.0f : (openEase - 0.35f) / 0.65f;
        const float bob = std::sin(glutGet(GLUT_ELAPSED_TIME) * 0.005f) * 0.01f * reveal;
        const float keyY = innerMinY + 0.08f + bob;
        const float keyX = (kChestMinX + kChestMaxX) * 0.5f + 0.04f;
        const float keyZ = (kChestMinZ + kChestMaxZ) * 0.5f + 0.12f;

        drawBrassKeyModel(keyX, keyY, keyZ);
    }
}

}  // namespace room_internal
