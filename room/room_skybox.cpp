#include "room_internal.h"

#include <cctype>
#include <fstream>
#include <limits>
#include <string>
#include <vector>

namespace room_internal {

GLuint gSkyboxTextures[SKYBOX_FACE_COUNT] = {};
bool gSkyboxLoaded = false;
GLuint gFloorTexture = 0;

namespace {

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

}  // namespace

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

void drawOutdoorScene() {
    glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDepthMask(GL_FALSE);
    drawSkybox();
    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_2D);

    glBegin(GL_QUADS);
        glColor3f(0.34f, 0.52f, 0.24f);
        glVertex3f(-22.0f, -0.02f, 19.9f);
        glVertex3f( 22.0f, -0.02f, 19.9f);
        glColor3f(0.38f, 0.58f, 0.26f);
        glVertex3f( 22.0f, -0.02f, 12.0f);
        glVertex3f(-22.0f, -0.02f, 12.0f);
    glEnd();

    glBegin(GL_QUADS);
        glColor3f(0.38f, 0.58f, 0.26f);
        glVertex3f(-22.0f, -0.02f, 12.0f);
        glVertex3f( 22.0f, -0.02f, 12.0f);
        glColor3f(0.44f, 0.64f, 0.28f);
        glVertex3f( 22.0f, -0.02f,  7.0f);
        glVertex3f(-22.0f, -0.02f,  7.0f);
    glEnd();

    glBegin(GL_QUADS);
        glColor3f(0.44f, 0.64f, 0.28f);
        glVertex3f(-22.0f, -0.02f,  7.0f);
        glVertex3f( 22.0f, -0.02f,  7.0f);
        glColor3f(0.40f, 0.60f, 0.24f);
        glVertex3f( 22.0f, -0.02f,  5.02f);
        glVertex3f(-22.0f, -0.02f,  5.02f);
    glEnd();

    glBegin(GL_QUADS);
        glColor3f(0.58f, 0.54f, 0.48f);
        glVertex3f(-1.8f, -0.01f,  5.04f);
        glVertex3f( 1.8f, -0.01f,  5.04f);
        glColor3f(0.52f, 0.48f, 0.43f);
        glVertex3f( 3.2f, -0.01f, 11.5f);
        glVertex3f(-3.2f, -0.01f, 11.5f);
    glEnd();

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

}  // namespace room_internal
