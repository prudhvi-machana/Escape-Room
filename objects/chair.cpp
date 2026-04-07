#include "objects.h"
#include "object_utils.h"

void drawChair() {
    drawFloorShadowAABB(-3.68f, -2.50f, -2.95f, -1.90f, 1.60f, 0.24f);

    drawBox(-3.78f, 0.55f, -2.78f, -2.60f, 0.70f, -1.74f, 0.42f, 0.25f, 0.13f, TEX_FABRIC, 2.0f);

    drawBox(-3.70f, 0.0f, -2.70f, -3.56f, 0.55f, -2.56f, 0.30f, 0.18f, 0.08f, TEX_WOOD_DARK, 1.8f);
    drawBox(-2.82f, 0.0f, -2.70f, -2.68f, 0.55f, -2.56f, 0.30f, 0.18f, 0.08f, TEX_WOOD_DARK, 1.8f);
    drawBox(-3.70f, 0.0f, -1.96f, -3.56f, 0.55f, -1.82f, 0.30f, 0.18f, 0.08f, TEX_WOOD_DARK, 1.8f);
    drawBox(-2.82f, 0.0f, -1.96f, -2.68f, 0.55f, -1.82f, 0.30f, 0.18f, 0.08f, TEX_WOOD_DARK, 1.8f);

    drawBox(-3.70f, 0.70f, -1.98f, -3.54f, 1.60f, -1.84f, 0.30f, 0.18f, 0.08f, TEX_WOOD_DARK, 1.8f);
    drawBox(-2.84f, 0.70f, -1.98f, -2.68f, 1.60f, -1.84f, 0.30f, 0.18f, 0.08f, TEX_WOOD_DARK, 1.8f);
    drawBox(-3.66f, 1.08f, -2.00f, -2.72f, 1.48f, -1.82f, 0.48f, 0.29f, 0.15f, TEX_FABRIC, 2.0f);

    drawBox(-3.52f, 0.82f, -2.04f, -2.86f, 1.08f, -1.90f, 0.54f, 0.34f, 0.18f, TEX_FABRIC, 2.0f);
}
