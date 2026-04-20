#include <GL/glut.h>
#include <cmath>
#include <fstream>
#include <limits>
#include <string>
#include <vector>
#include "room.h"
#include "../utils/utils.h"
#include "../utils/lighting.h"
#include "../objects/object_utils.h"

namespace {

constexpr float kChestMinX = 3.18f;
constexpr float kChestMaxX = 4.58f;
constexpr float kChestMinY = 0.05f;
constexpr float kChestMaxY = 1.02f;
constexpr float kChestMinZ = 0.92f;
constexpr float kChestMaxZ = 2.28f;
constexpr float kChestWall = 0.10f;
constexpr float kRoomHalf = 5.0f;
constexpr float kRoomHeight = 4.0f;
constexpr float kFrontWallInnerZ = 4.82f;
constexpr float kFrontWallOuterZ = 5.0f;
constexpr float kDoorFrameInnerZ = 4.86f;
constexpr float kDoorFrameOuterZ = 4.96f;

enum SkyboxFace {
    SKYBOX_FRONT = 0,
    SKYBOX_BACK,
    SKYBOX_LEFT,
    SKYBOX_RIGHT,
    SKYBOX_TOP,
    SKYBOX_BOTTOM,
    SKYBOX_FACE_COUNT
};

GLuint gSkyboxTextures[SKYBOX_FACE_COUNT] = {};
bool gSkyboxLoaded = false;

bool readPpmHeader(std::ifstream& in, int& width, int& height, int& maxValue) {
    auto nextToken = [&in]() -> std::string {
        std::string token;
        char ch = '\0';
        while (in.get(ch)) {
            if (ch == '#') {
                in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }
            if (!std::isspace(static_cast<unsigned char>(ch))) {
                token.push_back(ch);
                break;
            }
        }

        while (in.get(ch)) {
            if (std::isspace(static_cast<unsigned char>(ch))) break;
            token.push_back(ch);
        }
        return token;
    };

    const std::string magic = nextToken();
    if (magic != "P6") return false;

    const std::string widthToken = nextToken();
    const std::string heightToken = nextToken();
    const std::string maxValueToken = nextToken();
    if (widthToken.empty() || heightToken.empty() || maxValueToken.empty()) return false;

    width = std::stoi(widthToken);
    height = std::stoi(heightToken);
    maxValue = std::stoi(maxValueToken);
    return width > 0 && height > 0 && maxValue == 255;
}

bool loadSkyboxFaceFromPpm(const char* path, GLuint& textureId) {
    std::ifstream in(path, std::ios::binary);
    if (!in) return false;

    int width = 0;
    int height = 0;
    int maxValue = 0;
    if (!readPpmHeader(in, width, height, maxValue)) return false;

    std::vector<unsigned char> pixels(static_cast<size_t>(width) * static_cast<size_t>(height) * 3u);
    in.read(reinterpret_cast<char*>(pixels.data()), static_cast<std::streamsize>(pixels.size()));
    if (in.gcount() != static_cast<std::streamsize>(pixels.size())) return false;

    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    return true;
}

void initSkyboxTextures() {
    if (gSkyboxLoaded) return;

    const char* facePaths[SKYBOX_FACE_COUNT] = {
        "resources/skybox/front.ppm",
        "resources/skybox/back.ppm",
        "resources/skybox/left.ppm",
        "resources/skybox/right.ppm",
        "resources/skybox/top.ppm",
        "resources/skybox/bottom.ppm"
    };

    glGenTextures(SKYBOX_FACE_COUNT, gSkyboxTextures);
    gSkyboxLoaded = true;

    for (int i = 0; i < SKYBOX_FACE_COUNT; ++i) {
        if (!loadSkyboxFaceFromPpm(facePaths[i], gSkyboxTextures[i])) {
            gSkyboxLoaded = false;
            break;
        }
    }
}

void drawSkyboxFace(GLuint textureId,
                    float x1, float y1, float z1, float u1, float v1,
                    float x2, float y2, float z2, float u2, float v2,
                    float x3, float y3, float z3, float u3, float v3,
                    float x4, float y4, float z4, float u4, float v4) {
    glBindTexture(GL_TEXTURE_2D, textureId);
    glBegin(GL_QUADS);
        glTexCoord2f(u1, v1); glVertex3f(x1, y1, z1);
        glTexCoord2f(u2, v2); glVertex3f(x2, y2, z2);
        glTexCoord2f(u3, v3); glVertex3f(x3, y3, z3);
        glTexCoord2f(u4, v4); glVertex3f(x4, y4, z4);
    glEnd();
}

void drawSkybox() {
    if (!gSkyboxLoaded) return;

    const float cx = 0.0f;
    const float cy = 7.5f;
    const float cz = 12.0f;
    const float half = 26.0f;

    const float minX = cx - half;
    const float maxX = cx + half;
    const float minY = cy - half;
    const float maxY = cy + half;
    const float minZ = cz - half;
    const float maxZ = cz + half;

    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);

    // Faces are submitted clockwise from the viewer's position inside the cube.
    drawSkyboxFace(gSkyboxTextures[SKYBOX_FRONT],
                   minX, minY, maxZ, 0.0f, 1.0f,
                   maxX, minY, maxZ, 1.0f, 1.0f,
                   maxX, maxY, maxZ, 1.0f, 0.0f,
                   minX, maxY, maxZ, 0.0f, 0.0f);

    drawSkyboxFace(gSkyboxTextures[SKYBOX_BACK],
                   maxX, minY, minZ, 0.0f, 1.0f,
                   minX, minY, minZ, 1.0f, 1.0f,
                   minX, maxY, minZ, 1.0f, 0.0f,
                   maxX, maxY, minZ, 0.0f, 0.0f);

    drawSkyboxFace(gSkyboxTextures[SKYBOX_LEFT],
                   minX, minY, minZ, 0.0f, 1.0f,
                   minX, minY, maxZ, 1.0f, 1.0f,
                   minX, maxY, maxZ, 1.0f, 0.0f,
                   minX, maxY, minZ, 0.0f, 0.0f);

    drawSkyboxFace(gSkyboxTextures[SKYBOX_RIGHT],
                   maxX, minY, maxZ, 0.0f, 1.0f,
                   maxX, minY, minZ, 1.0f, 1.0f,
                   maxX, maxY, minZ, 1.0f, 0.0f,
                   maxX, maxY, maxZ, 0.0f, 0.0f);

    drawSkyboxFace(gSkyboxTextures[SKYBOX_TOP],
                   minX, maxY, maxZ, 0.0f, 1.0f,
                   maxX, maxY, maxZ, 1.0f, 1.0f,
                   maxX, maxY, minZ, 1.0f, 0.0f,
                   minX, maxY, minZ, 0.0f, 0.0f);

    drawSkyboxFace(gSkyboxTextures[SKYBOX_BOTTOM],
                   minX, minY, minZ, 0.0f, 1.0f,
                   maxX, minY, minZ, 1.0f, 1.0f,
                   maxX, minY, maxZ, 1.0f, 0.0f,
                   minX, minY, maxZ, 0.0f, 0.0f);
}

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

void drawDoorShadow(bool isOpen) {
    float minX = -1.0f;
    float maxX =  1.0f;
    float minZ =  4.99f;
    float maxZ =  5.01f;
    if (isOpen) {
        minX = -1.02f;
        maxX = -0.94f;
        minZ =  5.0f;
        maxZ =  7.0f;
    }

    const float lx = lighting::kCeilingLightX;
    const float ly = lighting::kCeilingLightY;
    const float lz = lighting::kCeilingLightZ;

    float sx1, sz1, sx2, sz2, sx3, sz3, sx4, sz4;
    projectPointToFloorFromLight(minX, 2.5f, maxZ, lx, ly, lz, sx1, sz1);
    projectPointToFloorFromLight(maxX, 2.5f, maxZ, lx, ly, lz, sx2, sz2);
    projectPointToFloorFromLight(maxX, 2.5f, minZ, lx, ly, lz, sx3, sz3);
    projectPointToFloorFromLight(minX, 2.5f, minZ, lx, ly, lz, sx4, sz4);

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    glColor4f(0.0f, 0.0f, 0.0f, 0.18f);
    glBegin(GL_QUADS);
        glVertex3f(sx1, 0.012f, sz1);
        glVertex3f(sx2, 0.012f, sz2);
        glVertex3f(sx3, 0.012f, sz3);
        glVertex3f(sx4, 0.012f, sz4);
    glEnd();

    glDepthMask(GL_TRUE);
    glPopAttrib();
}

void drawCeilingLightFixture() {
    // Metal mount.
    drawQuad(
        -0.28f, 3.98f, -0.28f,   0.28f, 3.98f, -0.28f,
         0.28f, 3.98f,  0.28f,  -0.28f, 3.98f,  0.28f,
        0.28f, 0.28f, 0.30f, TEX_METAL, 2.0f
    );

    drawQuad(
        -0.10f, 3.98f, -0.10f,   0.10f, 3.98f, -0.10f,
         0.10f, 3.76f, -0.10f,  -0.10f, 3.76f, -0.10f,
        0.20f, 0.20f, 0.22f, TEX_METAL, 2.5f
    );

    // Emissive panel where the ceiling light actually is.
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    if (roomLightOn) glColor3f(1.0f, 0.93f, 0.78f);
    else             glColor3f(0.20f, 0.19f, 0.16f);
    glBegin(GL_QUADS);
        glVertex3f(-0.24f, 3.74f, -0.24f);
        glVertex3f( 0.24f, 3.74f, -0.24f);
        glVertex3f( 0.24f, 3.74f,  0.24f);
        glVertex3f(-0.24f, 3.74f,  0.24f);
    glEnd();
    glPopAttrib();
}

void drawLightSwitch() {
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);

    // Soft backplate glow so the switch can be found before the room light is on.
    glColor3f(roomLightOn ? 0.42f : 0.26f, roomLightOn ? 0.40f : 0.24f, roomLightOn ? 0.36f : 0.20f);
    glBegin(GL_QUADS);
        glVertex3f(-4.985f, 0.98f, 1.06f);
        glVertex3f(-4.985f, 0.98f, 1.46f);
        glVertex3f(-4.985f, 1.57f, 1.46f);
        glVertex3f(-4.985f, 1.57f, 1.06f);
    glEnd();
    glPopAttrib();

    drawQuad(
        -4.99f, 1.05f, 1.10f,
        -4.99f, 1.05f, 1.42f,
        -4.99f, 1.50f, 1.42f,
        -4.99f, 1.50f, 1.10f,
        roomLightOn ? 0.82f : 0.92f,
        roomLightOn ? 0.82f : 0.90f,
        roomLightOn ? 0.80f : 0.78f,
        TEX_PLASTIC, 2.0f
    );

    float toggleTop = roomLightOn ? 1.40f : 1.28f;
    float toggleBottom = roomLightOn ? 1.20f : 1.08f;
    drawQuad(
        -4.98f, toggleBottom, 1.22f,
        -4.98f, toggleBottom, 1.30f,
        -4.98f, toggleTop,    1.30f,
        -4.98f, toggleTop,    1.22f,
        roomLightOn ? 0.92f : 0.55f,
        roomLightOn ? 0.90f : 0.55f,
        roomLightOn ? 0.82f : 0.58f,
        TEX_PLASTIC, 3.0f
    );

    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glColor3f(roomLightOn ? 0.96f : 0.82f, roomLightOn ? 0.90f : 0.76f, roomLightOn ? 0.72f : 0.62f);
    glBegin(GL_LINES);
        glVertex3f(-4.984f, 1.05f, 1.10f); glVertex3f(-4.984f, 1.05f, 1.42f);
        glVertex3f(-4.984f, 1.05f, 1.42f); glVertex3f(-4.984f, 1.50f, 1.42f);
        glVertex3f(-4.984f, 1.50f, 1.42f); glVertex3f(-4.984f, 1.50f, 1.10f);
        glVertex3f(-4.984f, 1.50f, 1.10f); glVertex3f(-4.984f, 1.05f, 1.10f);
    glEnd();
    glPopAttrib();
}

void drawRoomTrim() {
    const float trimH = 0.14f;
    const float crownY = 3.82f;

    // ── Floor-level skirting boards (all four walls, full run) ────────────
    drawBox(-kRoomHalf, 0.0f, -kRoomHalf, kRoomHalf, trimH, -kRoomHalf + 0.12f,
            0.42f, 0.26f, 0.13f, TEX_WOOD_DARK, 1.8f);
    drawBox(-kRoomHalf, 0.0f, kRoomHalf - 0.12f, kRoomHalf, trimH, kRoomHalf,
            0.42f, 0.26f, 0.13f, TEX_WOOD_DARK, 1.8f);
    drawBox(-kRoomHalf, 0.0f, -kRoomHalf, -kRoomHalf + 0.12f, trimH, kRoomHalf,
            0.42f, 0.26f, 0.13f, TEX_WOOD_DARK, 1.8f);
    drawBox(kRoomHalf - 0.12f, 0.0f, -kRoomHalf, kRoomHalf, trimH, kRoomHalf,
            0.42f, 0.26f, 0.13f, TEX_WOOD_DARK, 1.8f);

    // ── Crown moulding — back wall ─────────────────────────────────────────
    drawBox(-kRoomHalf, crownY, -kRoomHalf, kRoomHalf, kRoomHeight, -kRoomHalf + 0.18f,
            0.56f, 0.39f, 0.20f, TEX_WOOD, 2.0f);

    // ── Crown moulding — left wall ─────────────────────────────────────────
    drawBox(-kRoomHalf, crownY, -kRoomHalf, -kRoomHalf + 0.18f, kRoomHeight, kRoomHalf,
            0.56f, 0.39f, 0.20f, TEX_WOOD, 2.0f);

    // ── Crown moulding — right wall ────────────────────────────────────────
    drawBox(kRoomHalf - 0.18f, crownY, -kRoomHalf, kRoomHalf, kRoomHeight, kRoomHalf,
            0.56f, 0.39f, 0.20f, TEX_WOOD, 2.0f);

    // ── Crown moulding — front (door) wall ────────────────────────────────
    // The front wall geometry is split into three pieces around the door
    // opening (-1 to +1 at Z = kFrontWallInnerZ..kFrontWallOuterZ).
    // We match those same X extents so the trim sits flush against the wall.

    // Left segment: full height wall from X=-5 to X=-1
    drawBox(-kRoomHalf, crownY, kFrontWallInnerZ, -1.0f, kRoomHeight, kFrontWallOuterZ,
            0.56f, 0.39f, 0.20f, TEX_WOOD, 2.0f);

    // Right segment: full height wall from X=+1 to X=+5
    drawBox(1.0f, crownY, kFrontWallInnerZ, kRoomHalf, kRoomHeight, kFrontWallOuterZ,
            0.56f, 0.39f, 0.20f, TEX_WOOD, 2.0f);

    // Above-door segment: wall from X=-1 to X=+1, above Y=2.5
    drawBox(-1.0f, crownY, kFrontWallInnerZ, 1.0f, kRoomHeight, kFrontWallOuterZ,
            0.56f, 0.39f, 0.20f, TEX_WOOD, 2.0f);

    // Also add the front-side face of the crown on the front wall so it is
    // visible from inside the room (the trim faces inward, i.e. toward -Z).
    // The back wall trim is fine because the camera never looks at its back,
    // but for the front wall the interior face is the one you see.
    // The drawBox helper renders all six faces, so no extra quad is needed.
}

// ---------------------------------------------------------------------------
// Outdoor scene — skybox + ground dressing
// ---------------------------------------------------------------------------
void drawOutdoorScene() {
    glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDepthMask(GL_FALSE);
    drawSkybox();
    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_2D);

    // ── Grass ground plane — three depth layers for colour variation ───────
    // Far grass (slightly yellowish, desaturated by atmospheric haze)
    glBegin(GL_QUADS);
        glColor3f(0.34f, 0.52f, 0.24f);
        glVertex3f(-22.0f, -0.02f, 19.9f);
        glVertex3f( 22.0f, -0.02f, 19.9f);
        glColor3f(0.38f, 0.58f, 0.26f);
        glVertex3f( 22.0f, -0.02f, 12.0f);
        glVertex3f(-22.0f, -0.02f, 12.0f);
    glEnd();

    // Mid grass
    glBegin(GL_QUADS);
        glColor3f(0.38f, 0.58f, 0.26f);
        glVertex3f(-22.0f, -0.02f, 12.0f);
        glVertex3f( 22.0f, -0.02f, 12.0f);
        glColor3f(0.44f, 0.64f, 0.28f);
        glVertex3f( 22.0f, -0.02f,  7.0f);
        glVertex3f(-22.0f, -0.02f,  7.0f);
    glEnd();

    // Near grass (richest green, closest to the room)
    glBegin(GL_QUADS);
        glColor3f(0.44f, 0.64f, 0.28f);
        glVertex3f(-22.0f, -0.02f,  7.0f);
        glVertex3f( 22.0f, -0.02f,  7.0f);
        glColor3f(0.40f, 0.60f, 0.24f);
        glVertex3f( 22.0f, -0.02f,  5.02f);
        glVertex3f(-22.0f, -0.02f,  5.02f);
    glEnd();

    // ── Garden path (stone/gravel strip leading from the door) ────────────
    glBegin(GL_QUADS);
        glColor3f(0.58f, 0.54f, 0.48f);
        glVertex3f(-1.8f, -0.01f,  5.04f);
        glVertex3f( 1.8f, -0.01f,  5.04f);
        glColor3f(0.52f, 0.48f, 0.43f);
        glVertex3f( 3.2f, -0.01f, 11.5f);
        glVertex3f(-3.2f, -0.01f, 11.5f);
    glEnd();

    // Path edge darkening strips (simulate depth between grass and path)
    glColor3f(0.28f, 0.40f, 0.18f);
    glBegin(GL_QUADS);
        glVertex3f(-1.8f,  -0.015f,  5.04f);
        glVertex3f(-1.55f, -0.015f,  5.04f);
        glVertex3f(-2.90f, -0.015f, 11.5f);
        glVertex3f(-3.2f,  -0.015f, 11.5f);
    glEnd();
    glBegin(GL_QUADS);
        glVertex3f( 1.55f, -0.015f,  5.04f);
        glVertex3f( 1.8f,  -0.015f,  5.04f);
        glVertex3f( 3.2f,  -0.015f, 11.5f);
        glVertex3f( 2.90f, -0.015f, 11.5f);
    glEnd();

    glPopAttrib();
}

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

} // namespace

void initRoom() {
    initSkyboxTextures();
    worldItems.clear();
    drawerOpen = false;
    codeBoxUnlocked = false;
    codeBoxOpenProgress = 0.0f;
    codeBoxInput.clear();

    worldItems.push_back(Item(
        "Door", 0.0f, 1.25f, 5.0f, 2.0f,
        "open",
        "The door is locked tight for now."
    ));
    worldItems.push_back(Item(
        "Highlighted Book", 4.15f, 1.85f, -3.14f, 1.1f,
        "read",
        "Book note: 'The room rewards careful observation.'"
    ));
    worldItems.push_back(Item(
        "Dustbin Paper", 4.50f, 0.38f, 4.52f, 1.0f,
        "inspect",
        "The paper says: 'Books belong on shelves... so why is one not?'"
    ));
    worldItems.push_back(Item(
        "Light Switch", -4.55f, 1.25f, 1.26f, 0.9f,
        "toggle",
        "Left click to flip the room light."
    ));
    worldItems.push_back(Item(
        "Desk Drawer", -2.90f, 0.84f, -2.30f, 1.15f,
        "open",
        "A shallow drawer is built into the table."
    ));
    worldItems.push_back(Item(
        "Drawer Book", -2.90f, 0.85f, -2.08f, 0.75f,
        "read",
        "The secret code to your destination is u4u03."
    ));
    worldItems.push_back(Item(
        "Code Box", 3.90f, 0.84f, 1.45f, 1.75f,
        "inspect",
        "A large wooden chest waits for a three-letter code."
    ));
    worldItems.push_back(Item(
        "Brass Key", 3.94f, 0.28f, 1.72f, 1.08f,
        "pickup",
        ""
    ));
}

void drawRoom() {
    const unsigned int floorTexture = loadExternalTexture("resources/Mantel.ppm");
    drawOutdoorScene();

    if (floorTexture != 0) {
        drawExternalTexturedQuad(
            -5,0,-5,   5,0,-5,   5,0,5,   -5,0,5,
            1.0f, 1.0f, 1.0f, floorTexture, 0.55f
        );
    } else {
        drawQuad(
            -5,0,-5,   5,0,-5,   5,0,5,   -5,0,5,
            0.44f, 0.41f, 0.38f, TEX_FLOOR, 0.48f
        );
    }

    drawQuad(
        -5,4,-5,   5,4,-5,   5,4,5,   -5,4,5,
        0.78f, 0.76f, 0.70f, TEX_PLASTER, 0.65f
    );

    drawQuad(
        -5,0,-5,   5,0,-5,   5,4,-5,   -5,4,-5,
        0.58f, 0.54f, 0.46f, TEX_WALLPAPER, 0.55f
    );

    drawQuad(
        -5,0,-5,   -5,0,5,   -5,4,5,   -5,4,-5,
        0.56f, 0.50f, 0.42f, TEX_WALLPAPER, 0.55f
    );

    drawQuad(
        5,0,-5,   5,0,5,   5,4,5,   5,4,-5,
        0.56f, 0.50f, 0.42f, TEX_WALLPAPER, 0.55f
    );

    drawBox(
        -5.0f, 0.0f, kFrontWallInnerZ,  -1.0f, 4.0f, kFrontWallOuterZ,
        0.60f, 0.55f, 0.47f, TEX_WALLPAPER, 0.55f
    );

    drawBox(
        1.0f, 0.0f, kFrontWallInnerZ,   5.0f, 4.0f, kFrontWallOuterZ,
        0.60f, 0.55f, 0.47f, TEX_WALLPAPER, 0.55f
    );

    drawBox(
        -1.0f, 2.5f, kFrontWallInnerZ,   1.0f, 4.0f, kFrontWallOuterZ,
        0.60f, 0.55f, 0.47f, TEX_WALLPAPER, 0.55f
    );

    drawRoomTrim();

    drawCeilingLightFixture();
    drawLightSwitch();
    drawCodeBox();
}

void drawDoor() {
    Item* door = getItemByName("Door");
    bool  isOpen = door && door->pickedUp;
    drawDoorShadow(isOpen);

    glPushMatrix();
        glTranslatef(-1.0f, 0.0f, 5.0f);
        glRotatef(isOpen ? -90.0f : 0.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(1.0f, 0.0f, 0.0f);

        float brightness = isOpen ? 0.75f : 0.6f;
        drawQuad(
            -1,0,0,   1,0,0,   1,2.5,0,   -1,2.5,0,
            brightness, 0.36f, 0.12f, TEX_WOOD_DARK, 1.1f
        );

        float hr = isOpen ? 0.5f : 1.0f;
        float hg = isOpen ? 0.5f : 0.8f;
        float hb = isOpen ? 0.5f : 0.0f;
        drawQuad(
            0.6f, 1.1f, 0.01f,
            0.8f, 1.1f, 0.01f,
            0.8f, 1.3f, 0.01f,
            0.6f, 1.3f, 0.01f,
            hr, hg, hb, TEX_PLASTIC, 4.0f
        );

        drawQuad(
            0.67f, 0.85f, 0.01f,
            0.73f, 0.85f, 0.01f,
            0.73f, 0.95f, 0.01f,
            0.67f, 0.95f, 0.01f,
            0.2f, 0.2f, 0.2f, TEX_METAL, 4.0f
        );

        drawBox(0.72f, 1.06f, -0.02f, 0.84f, 1.18f, 0.05f,
                0.86f, 0.70f, 0.22f, TEX_BRASS, 5.0f);

    glPopMatrix();

    drawBox(
        -1.15f, 0.0f, kDoorFrameInnerZ,  -1.0f, 2.65f, kDoorFrameOuterZ,
        0.30f, 0.18f, 0.05f, TEX_WOOD_DARK, 0.8f
    );
    drawBox(
        1.0f, 0.0f, kDoorFrameInnerZ,   1.15f, 2.65f, kDoorFrameOuterZ,
        0.30f, 0.18f, 0.05f, TEX_WOOD_DARK, 0.8f
    );
    drawBox(
        -1.15f, 2.52f, kDoorFrameInnerZ,   1.15f, 2.66f, kDoorFrameOuterZ,
        0.30f, 0.18f, 0.05f, TEX_WOOD_DARK, 0.8f
    );
}
