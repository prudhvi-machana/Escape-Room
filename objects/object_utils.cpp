#include <GL/glut.h>
#include <cmath>
#include "object_utils.h"
#include "../utils/lighting.h"

namespace {

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

} // namespace

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

    drawProjectedShadowQuad(
        minX, maxX, minZ, maxZ, topY, alpha,
        lighting::kCeilingLightX, lighting::kCeilingLightY, lighting::kCeilingLightZ
    );

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
