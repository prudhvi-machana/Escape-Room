#include "objects.h"
#include "object_utils.h"
#include "../utils/utils.h"
#include <cmath>

namespace {

constexpr float kPi = 3.14159265f;
constexpr int kSegments = 20;

constexpr float kBinCenterX = 4.53f;
constexpr float kBinCenterZ = 4.53f;
constexpr float kBinOuterRadius = 0.24f;
constexpr float kBinInnerRadius = 0.19f;
constexpr float kBinBaseRadius = 0.18f;
constexpr float kBinBottomY = 0.03f;
constexpr float kBinTopY = 0.84f;
constexpr float kLinerBottomY = 0.09f;
constexpr float kLinerTopY = 0.77f;
constexpr float kBinMinX = kBinCenterX - kBinOuterRadius;
constexpr float kBinMaxX = kBinCenterX + kBinOuterRadius;
constexpr float kBinMinZ = kBinCenterZ - kBinOuterRadius;
constexpr float kBinMaxZ = 4.86f;

void ringPoint(float radius, int segment, float& x, float& z) {
    const float angle = (2.0f * kPi * static_cast<float>(segment)) / static_cast<float>(kSegments);
    x = kBinCenterX + std::cos(angle) * radius;
    z = kBinCenterZ + std::sin(angle) * radius;
}

void drawDisc(float radius, float y, float r, float g, float b, TextureId texture, float textureScale, bool faceUp) {
    for (int i = 0; i < kSegments; ++i) {
        float x1, z1, x2, z2;
        ringPoint(radius, i, x1, z1);
        ringPoint(radius, i + 1, x2, z2);

        if (faceUp) {
            drawQuad(
                kBinCenterX, y, kBinCenterZ,
                x1, y, z1,
                x2, y, z2,
                kBinCenterX, y, kBinCenterZ,
                r, g, b, texture, textureScale
            );
        } else {
            drawQuad(
                kBinCenterX, y, kBinCenterZ,
                x2, y, z2,
                x1, y, z1,
                kBinCenterX, y, kBinCenterZ,
                r, g, b, texture, textureScale
            );
        }
    }
}

void drawCylinderSide(float bottomRadius, float topRadius,
                      float bottomY, float topY,
                      float r, float g, float b,
                      TextureId texture, float textureScale,
                      bool outward) {
    for (int i = 0; i < kSegments; ++i) {
        float bx1, bz1, bx2, bz2;
        float tx1, tz1, tx2, tz2;
        ringPoint(bottomRadius, i, bx1, bz1);
        ringPoint(bottomRadius, i + 1, bx2, bz2);
        ringPoint(topRadius, i, tx1, tz1);
        ringPoint(topRadius, i + 1, tx2, tz2);

        if (outward) {
            drawQuad(
                bx1, bottomY, bz1,
                bx2, bottomY, bz2,
                tx2, topY, tz2,
                tx1, topY, tz1,
                r, g, b, texture, textureScale
            );
        } else {
            drawQuad(
                bx2, bottomY, bz2,
                bx1, bottomY, bz1,
                tx1, topY, tz1,
                tx2, topY, tz2,
                r, g, b, texture, textureScale
            );
        }
    }
}

void drawTopRim() {
    for (int i = 0; i < kSegments; ++i) {
        float ox1, oz1, ox2, oz2;
        float ix1, iz1, ix2, iz2;
        ringPoint(kBinOuterRadius, i, ox1, oz1);
        ringPoint(kBinOuterRadius, i + 1, ox2, oz2);
        ringPoint(kBinInnerRadius, i, ix1, iz1);
        ringPoint(kBinInnerRadius, i + 1, ix2, iz2);

        drawQuad(
            ox1, kBinTopY, oz1,
            ix1, kBinTopY, iz1,
            ix2, kBinTopY, iz2,
            ox2, kBinTopY, oz2,
            0.30f, 0.33f, 0.36f, TEX_METAL, 4.0f
        );
    }
}

void drawPedal() {
    drawBox(4.44f, 0.03f, 4.78f, 4.62f, 0.07f, 4.86f,
            0.16f, 0.17f, 0.18f, TEX_METAL, 2.4f);
    drawBox(4.47f, 0.07f, 4.80f, 4.59f, 0.105f, 4.84f,
            0.10f, 0.11f, 0.12f, TEX_PLASTIC, 2.0f);
}

void drawPaperClue() {
    drawQuad(
        4.39f, 0.23f, 4.41f,
        4.61f, 0.25f, 4.43f,
        4.60f, 0.28f, 4.63f,
        4.38f, 0.26f, 4.60f,
        0.96f, 0.93f, 0.84f, TEX_PAPER, 3.4f
    );

    drawQuad(
        4.41f, 0.231f, 4.44f,
        4.58f, 0.248f, 4.45f,
        4.57f, 0.27f, 4.60f,
        4.40f, 0.255f, 4.58f,
        0.82f, 0.80f, 0.72f, TEX_PAPER, 7.0f
    );
}

void drawLoosePapers() {
    drawBox(4.45f, 0.14f, 4.49f, 4.58f, 0.20f, 4.60f, 0.84f, 0.82f, 0.76f, TEX_PAPER, 2.5f);
    drawBox(4.49f, 0.18f, 4.42f, 4.63f, 0.24f, 4.53f, 0.78f, 0.76f, 0.69f, TEX_PAPER, 2.8f);
    drawBox(4.36f, 0.16f, 4.48f, 4.49f, 0.22f, 4.58f, 0.90f, 0.87f, 0.82f, TEX_PAPER, 2.6f);
}

} // namespace

void drawDustbin() {
    drawFloorShadowAABB(kBinMinX, kBinMaxX, kBinMinZ, kBinMaxZ, kBinTopY, 0.24f);

    drawDisc(kBinBaseRadius, kBinBottomY, 0.17f, 0.19f, 0.21f, TEX_METAL, 2.4f, false);
    drawCylinderSide(kBinBaseRadius, kBinOuterRadius,
                     kBinBottomY, kBinTopY,
                     0.34f, 0.37f, 0.40f, TEX_METAL, 4.0f, true);
    drawCylinderSide(0.15f, kBinInnerRadius,
                     kLinerBottomY, kLinerTopY,
                     0.08f, 0.09f, 0.10f, TEX_PLASTIC, 4.8f, false);
    drawDisc(0.15f, kLinerBottomY, 0.07f, 0.08f, 0.09f, TEX_PLASTIC, 2.6f, true);
    drawTopRim();
    drawPedal();
    drawLoosePapers();
    drawPaperClue();
}
