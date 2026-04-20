#ifndef ROOM_INTERNAL_H
#define ROOM_INTERNAL_H

#include <GL/glut.h>

namespace room_internal {

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

extern GLuint gSkyboxTextures[SKYBOX_FACE_COUNT];
extern bool gSkyboxLoaded;
extern GLuint gFloorTexture;

void initSkyboxTextures();
void drawOutdoorScene();
void drawDoorShadow(bool isOpen);
void drawCeilingLightFixture();
void drawLightSwitch();
void drawRoomTrim();
void drawCodeBox();

}  // namespace room_internal

#endif
