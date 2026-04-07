#include "objects.h"
#include "object_utils.h"
#include "../utils/utils.h"

namespace {

void drawCrumpledPaper() {
    drawBox(-1.34f, 0.28f, -4.28f, -1.16f, 0.44f, -4.10f, 0.86f, 0.84f, 0.78f);
    drawBox(-1.28f, 0.39f, -4.20f, -1.09f, 0.52f, -4.03f, 0.80f, 0.78f, 0.70f);
    drawBox(-1.41f, 0.38f, -4.24f, -1.24f, 0.50f, -4.08f, 0.90f, 0.88f, 0.84f);
}

} // namespace

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
