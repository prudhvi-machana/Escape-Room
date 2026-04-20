#include "objects.h"
#include "../utils/utils.h"

namespace {

unsigned int gPaintingTexture = 0;

} // namespace

void drawPainting() {
    constexpr float kFrameMinX = -1.35f;
    constexpr float kFrameMaxX = 1.35f;
    constexpr float kFrameMinY = 1.35f;
    constexpr float kFrameMaxY = 3.15f;
    constexpr float kFrameZ = -4.99f;
    constexpr float kImageInsetX = 0.25f;
    constexpr float kImageInsetY = 0.23f;
    constexpr float kImageAspect = 728.0f / 485.0f;

    drawQuad(
        kFrameMinX, kFrameMinY, kFrameZ,
        kFrameMaxX, kFrameMinY, kFrameZ,
        kFrameMaxX, kFrameMaxY, kFrameZ,
        kFrameMinX, kFrameMaxY, kFrameZ,
        0.18f, 0.12f, 0.08f, TEX_WOOD_DARK, 1.1f
    );

    const float availableWidth = (kFrameMaxX - kFrameMinX) - kImageInsetX * 2.0f;
    const float availableHeight = (kFrameMaxY - kFrameMinY) - kImageInsetY * 2.0f;
    float imageWidth = availableWidth;
    float imageHeight = imageWidth / kImageAspect;
    if (imageHeight > availableHeight) {
        imageHeight = availableHeight;
        imageWidth = imageHeight * kImageAspect;
    }

    const float imageMinX = -imageWidth * 0.5f;
    const float imageMaxX = imageWidth * 0.5f;
    const float imageMinY = (kFrameMinY + kFrameMaxY - imageHeight) * 0.5f;
    const float imageMaxY = imageMinY + imageHeight;
    const float imageZ = -4.975f;
    if (gPaintingTexture == 0) {
        gPaintingTexture = loadExternalTexture("resources/painting/1.ppm");
    }

    if (gPaintingTexture == 0) {
        drawQuad(
            imageMinX, imageMinY, imageZ,
            imageMaxX, imageMinY, imageZ,
            imageMaxX, imageMaxY, imageZ,
            imageMinX, imageMaxY, imageZ,
            0.76f, 0.56f, 0.30f, TEX_BOOK, 0.9f
        );
        return;
    }

    drawExternalImageQuad(
        imageMinX, imageMinY, imageZ,
        imageMaxX, imageMinY, imageZ,
        imageMaxX, imageMaxY, imageZ,
        imageMinX, imageMaxY, imageZ,
        1.0f, 1.0f, 1.0f, gPaintingTexture
    );
}
