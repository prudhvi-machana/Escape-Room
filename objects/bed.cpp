#include "objects.h"
#include "object_utils.h"

namespace {

void drawBedPillow() {
    drawBox(0.95f, 0.56f, -4.10f, 1.80f, 0.72f, -3.35f, 0.86f, 0.86f, 0.82f, TEX_FABRIC, 2.4f);
    drawBox(1.88f, 0.56f, -4.10f, 2.72f, 0.72f, -3.35f, 0.84f, 0.84f, 0.80f, TEX_FABRIC, 2.4f);
}

void drawBedBlanket() {
    drawBox(0.78f, 0.42f, -3.40f, 3.00f, 0.64f, -1.20f, 0.34f, 0.48f, 0.62f, TEX_FABRIC, 1.5f);
    drawBox(0.88f, 0.40f, -1.55f, 2.88f, 0.58f, -1.10f, 0.58f, 0.47f, 0.32f, TEX_FABRIC, 2.2f);
}

} // namespace

void drawBed() {
    drawFloorShadowAABB(0.55f, 3.20f, -4.60f, -0.85f, 1.05f, 0.22f);

    drawBox(0.62f, 0.22f, -4.48f, 3.10f, 0.32f, -0.96f, 0.30f, 0.18f, 0.10f, TEX_WOOD_DARK, 1.4f);
    drawBox(0.62f, 0.32f, -4.48f, 0.78f, 0.56f, -0.96f, 0.32f, 0.20f, 0.10f, TEX_WOOD_DARK, 1.1f);
    drawBox(2.94f, 0.32f, -4.48f, 3.10f, 0.44f, -0.96f, 0.32f, 0.20f, 0.10f, TEX_WOOD_DARK, 1.1f);
    drawBox(0.78f, 0.32f, -4.48f, 0.90f, 0.40f, -0.96f, 0.30f, 0.18f, 0.10f, TEX_WOOD_DARK, 1.0f);
    drawBox(2.82f, 0.32f, -4.48f, 2.94f, 0.40f, -0.96f, 0.30f, 0.18f, 0.10f, TEX_WOOD_DARK, 1.0f);

    drawBox(0.68f, 0.00f, -4.40f, 0.84f, 0.22f, -4.24f, 0.26f, 0.15f, 0.08f, TEX_WOOD_DARK, 1.2f);
    drawBox(2.88f, 0.00f, -4.40f, 3.04f, 0.22f, -4.24f, 0.26f, 0.15f, 0.08f, TEX_WOOD_DARK, 1.2f);
    drawBox(0.68f, 0.00f, -1.20f, 0.84f, 0.22f, -1.04f, 0.26f, 0.15f, 0.08f, TEX_WOOD_DARK, 1.2f);
    drawBox(2.88f, 0.00f, -1.20f, 3.04f, 0.22f, -1.04f, 0.26f, 0.15f, 0.08f, TEX_WOOD_DARK, 1.2f);

    drawBox(0.76f, 0.36f, -4.36f, 2.96f, 0.56f, -1.08f, 0.80f, 0.80f, 0.76f, TEX_FABRIC, 1.8f);

    drawBedBlanket();
    drawBedPillow();
}
