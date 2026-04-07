#ifndef OBJECT_UTILS_H
#define OBJECT_UTILS_H

#include "../utils/utils.h"

void drawBox(float x1, float y1, float z1,
             float x2, float y2, float z2,
             float r, float g, float b,
             TextureId texture = TEX_GENERIC,
             float textureScale = 1.0f);

void drawFloorShadowAABB(float minX, float maxX,
                         float minZ, float maxZ,
                         float topY,
                         float alpha);

#endif
