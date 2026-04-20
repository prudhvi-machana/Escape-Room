#include <cmath>
#include "objects.h"
#include "object_utils.h"

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

constexpr int kShelfBookCount = 5;

const ShelfBookSpec kBottomShelf[kShelfBookCount] = {
    {0.22f, 0.60f, 4.24f, 4.42f, 0.75f, 0.15f, 0.15f},
    {0.22f, 0.56f, 4.24f, 4.42f, 0.15f, 0.35f, 0.75f},
    {0.22f, 0.64f, 4.24f, 4.42f, 0.85f, 0.70f, 0.20f},
    {0.22f, 0.52f, 4.24f, 4.42f, 0.15f, 0.60f, 0.35f},
    {0.22f, 0.66f, 4.24f, 4.42f, 0.55f, 0.25f, 0.70f}
};

const ShelfBookSpec kMiddleShelf[kShelfBookCount] = {
    {0.18f, 0.56f, 4.24f, 4.40f, 0.90f, 0.45f, 0.18f},
    {0.18f, 0.47f, 4.24f, 4.40f, 0.80f, 0.25f, 0.25f},
    {0.26f, 0.65f, 4.12f, 4.42f, 0.90f, 0.80f, 0.20f},
    {0.18f, 0.63f, 4.24f, 4.40f, 0.25f, 0.70f, 0.45f},
    {0.18f, 0.53f, 4.24f, 4.40f, 0.78f, 0.72f, 0.28f}
};

const ShelfBookSpec kTopShelf[kShelfBookCount] = {
    {0.22f, 0.53f, 4.24f, 4.42f, 0.75f, 0.22f, 0.18f},
    {0.22f, 0.60f, 4.24f, 4.42f, 0.18f, 0.35f, 0.70f},
    {0.22f, 0.66f, 4.24f, 4.42f, 0.80f, 0.68f, 0.22f},
    {0.22f, 0.49f, 4.24f, 4.42f, 0.20f, 0.60f, 0.38f},
    {0.22f, 0.63f, 4.24f, 4.42f, 0.62f, 0.28f, 0.78f}
};

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

void drawBookshelfBooks() {
    const float shelfCenterZ = -2.90f;
    const float shelfGap = 0.045f;
    const float middleShelfGap = 0.035f;

    drawShelfRow(0.45f, shelfCenterZ, shelfGap, kBottomShelf, kShelfBookCount);
    drawShelfRow(1.45f, shelfCenterZ, middleShelfGap, kMiddleShelf, kShelfBookCount);
    drawShelfRow(2.42f, shelfCenterZ, shelfGap, kTopShelf, kShelfBookCount);
}

} // namespace

void drawBookshelf() {
    const float shelfGap = 0.045f;
    const float middleShelfGap = 0.035f;
    static const float widestRow = std::fmax(
        measureShelfRowWidth(kBottomShelf, kShelfBookCount, shelfGap),
        std::fmax(
            measureShelfRowWidth(kMiddleShelf, kShelfBookCount, middleShelfGap),
            measureShelfRowWidth(kTopShelf, kShelfBookCount, shelfGap)
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
