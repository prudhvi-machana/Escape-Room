#include <GL/glut.h>
#include <cmath>
#include "camera.h"
#include "../utils/utils.h"

// --- Constants ---
static const float PI          = 3.14159265f;
static const float DEG2RAD     = PI / 180.0f;
static const float MOVE_SPEED  = 0.15f;
static const float LOOK_SENS   = 0.15f;
static const float PITCH_LIMIT = 89.0f;
static const float INDOOR_SIDE_LIMIT = 4.60f;
static const float INDOOR_BACK_LIMIT = -4.60f;
static const float INDOOR_FRONT_LIMIT = 4.60f;
static const float PLAYER_RADIUS = 0.22f;
static const float CHEST_MIN_X = 3.18f;
static const float CHEST_MAX_X = 4.58f;
static const float CHEST_MIN_Z = 0.92f;
static const float CHEST_MAX_Z = 2.28f;
static const float CHEST_BLOCK_OPEN_PROGRESS = 0.72f;
static const float FRONT_WALL_MIN_Z = 4.82f;
static const float FRONT_WALL_MAX_Z = 5.0f;
static const float LEFT_FRONT_WALL_MIN_X = -5.0f;
static const float LEFT_FRONT_WALL_MAX_X = -1.0f;
static const float RIGHT_FRONT_WALL_MIN_X = 1.0f;
static const float RIGHT_FRONT_WALL_MAX_X = 5.0f;

float camX = 0.0f, camY = 1.0f, camZ = 0.0f;
float angleY = 180.0f;
float angleX = 0.0f;
int   winW = 900, winH = 600;
bool  warping       = false;
bool  mouseCaptured = true;

// --- Helpers ---
void pushOutOfAABB(float prevX, float prevZ,
                   float& x, float& z,
                   float minX, float maxX,
                   float minZ, float maxZ) {
    const float expandedMinX = minX - PLAYER_RADIUS;
    const float expandedMaxX = maxX + PLAYER_RADIUS;
    const float expandedMinZ = minZ - PLAYER_RADIUS;
    const float expandedMaxZ = maxZ + PLAYER_RADIUS;

    if (x < expandedMinX || x > expandedMaxX || z < expandedMinZ || z > expandedMaxZ) {
        return;
    }

    const bool prevInsideX = prevX >= expandedMinX && prevX <= expandedMaxX;
    const bool prevInsideZ = prevZ >= expandedMinZ && prevZ <= expandedMaxZ;

    if (!prevInsideX) {
        x = (prevX < expandedMinX) ? expandedMinX : expandedMaxX;
        return;
    }
    if (!prevInsideZ) {
        z = (prevZ < expandedMinZ) ? expandedMinZ : expandedMaxZ;
        return;
    }

    const float pushLeft = std::fabs(x - expandedMinX);
    const float pushRight = std::fabs(expandedMaxX - x);
    const float pushFront = std::fabs(z - expandedMinZ);
    const float pushBack = std::fabs(expandedMaxZ - z);

    if (std::fmin(pushLeft, pushRight) < std::fmin(pushFront, pushBack)) {
        x = (pushLeft < pushRight) ? expandedMinX : expandedMaxX;
    } else {
        z = (pushFront < pushBack) ? expandedMinZ : expandedMaxZ;
    }
}

void clampToRoom(float& x, float& y, float& z) {
    if (z < INDOOR_BACK_LIMIT) z = INDOOR_BACK_LIMIT;

    Item* door = getItemByName("Door");
    const bool doorOpen = door && door->pickedUp;
    if (!doorOpen) {
        if (x < -INDOOR_SIDE_LIMIT) x = -INDOOR_SIDE_LIMIT;
        if (x >  INDOOR_SIDE_LIMIT) x =  INDOOR_SIDE_LIMIT;
        if (z > INDOOR_FRONT_LIMIT) z = INDOOR_FRONT_LIMIT;
    } else if (z <= INDOOR_FRONT_LIMIT) {
        if (x < -INDOOR_SIDE_LIMIT) x = -INDOOR_SIDE_LIMIT;
        if (x >  INDOOR_SIDE_LIMIT) x =  INDOOR_SIDE_LIMIT;
    } else {
        if (z < 5.15f && (x < -1.0f || x > 1.0f)) {
            z = INDOOR_FRONT_LIMIT;
        }
        if (x < -12.0f) x = -12.0f;
        if (x >  12.0f) x =  12.0f;
        if (z > 18.0f) z = 18.0f;
    }

    if (y < 0.35f) y = 0.35f;
    if (y > 3.60f) y = 3.60f;
}

void moveCamera(float dx, float dy, float dz) {
    const float prevX = camX;
    const float prevZ = camZ;
    float nextX = camX + dx;
    float nextY = camY + dy;
    float nextZ = camZ + dz;
    clampToRoom(nextX, nextY, nextZ);

    Item* door = getItemByName("Door");
    const bool doorOpen = door && door->pickedUp;
    if (doorOpen) {
        pushOutOfAABB(
            prevX, prevZ, nextX, nextZ,
            LEFT_FRONT_WALL_MIN_X, LEFT_FRONT_WALL_MAX_X,
            FRONT_WALL_MIN_Z, FRONT_WALL_MAX_Z
        );
        pushOutOfAABB(
            prevX, prevZ, nextX, nextZ,
            RIGHT_FRONT_WALL_MIN_X, RIGHT_FRONT_WALL_MAX_X,
            FRONT_WALL_MIN_Z, FRONT_WALL_MAX_Z
        );
    }

    if (!codeBoxUnlocked || codeBoxOpenProgress < CHEST_BLOCK_OPEN_PROGRESS) {
        pushOutOfAABB(prevX, prevZ, nextX, nextZ, CHEST_MIN_X, CHEST_MAX_X, CHEST_MIN_Z, CHEST_MAX_Z);
    }
    camX = nextX;
    camY = nextY;
    camZ = nextZ;
}

inline void forwardXZ(float& fx, float& fz) {
    fx =  sinf(angleY * DEG2RAD);
    fz = -cosf(angleY * DEG2RAD);
}

inline void forward3D(float& fx, float& fy, float& fz) {
    float cosP = cosf(angleX * DEG2RAD);
    fx =  sinf(angleY * DEG2RAD) * cosP;
    fy = -sinf(angleX * DEG2RAD);
    fz = -cosf(angleY * DEG2RAD) * cosP;
}

inline void rightXZ(float& rx, float& rz) {
    rx =  cosf(angleY * DEG2RAD);
    rz =  sinf(angleY * DEG2RAD);
}

void clampPitch() {
    if (angleX < -PITCH_LIMIT) angleX = -PITCH_LIMIT;
    if (angleX >  PITCH_LIMIT) angleX =  PITCH_LIMIT;
}

// --- Keyboard ---
void keyboard(unsigned char key, int x, int y) {
    if (codeBoxOverlayActive) {
        if (handleCodeBoxKeypress(key)) {
            glutPostRedisplay();
            return;
        }
    }

    if (drawerBookOverlayActive) {
        if (handleDrawerBookOverlayInput()) {
            glutPostRedisplay();
            return;
        }
    }

    float fx, fy, fz, rx, rz;
    forward3D(fx, fy, fz);
    rightXZ(rx, rz);

    switch (key) {
        // --- Movement ---
        case 'w': case 'W':
            moveCamera(fx * MOVE_SPEED, fy * MOVE_SPEED, fz * MOVE_SPEED);
            break;
        case 's': case 'S':
            moveCamera(-fx * MOVE_SPEED, -fy * MOVE_SPEED, -fz * MOVE_SPEED);
            break;
        case 'a': case 'A':
            moveCamera(-rx * MOVE_SPEED, 0.0f, -rz * MOVE_SPEED);
            break;
        case 'd': case 'D':
            moveCamera(rx * MOVE_SPEED, 0.0f, rz * MOVE_SPEED);
            break;
        case ' ':
            moveCamera(0.0f, MOVE_SPEED, 0.0f); break;
        case 'c': case 'C':
            moveCamera(0.0f, -MOVE_SPEED, 0.0f); break;
        case 'e': case 'E':
            interactWithNearbyItem(camX, camY, camZ);
            break;

        // --- Mouse toggle ---
        case '\t':
            mouseCaptured = !mouseCaptured;
            glutSetCursor(mouseCaptured ? GLUT_CURSOR_NONE : GLUT_CURSOR_INHERIT);
            break;

        case 27: exit(0);
    }
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    if (isAnyOverlayActive()) {
        if (handleDrawerBookOverlayInput()) {
            glutPostRedisplay();
        }
        glutPostRedisplay();
        return;
    }

    switch (key) {
        case GLUT_KEY_LEFT:  angleY -= 5.0f; break;
        case GLUT_KEY_RIGHT: angleY += 5.0f; break;
        case GLUT_KEY_UP:    angleX -= 3.0f; clampPitch(); break;
        case GLUT_KEY_DOWN:  angleX += 3.0f; clampPitch(); break;
    }
    glutPostRedisplay();
}

void mouseMotion(int x, int y) {
    if (isAnyOverlayActive()) return;
    if (!mouseCaptured) return;
    if (warping) { warping = false; return; }

    int cx = winW / 2, cy = winH / 2;
    int dx = x - cx,   dy = y - cy;
    if (dx == 0 && dy == 0) return;

    angleY += dx * LOOK_SENS;
    angleX += dy * LOOK_SENS;
    clampPitch();

    warping = true;
    glutWarpPointer(cx, cy);
    glutPostRedisplay();
}

void mouseButton(int button, int state, int x, int y) {
    if (state != GLUT_DOWN) return;

    if (drawerBookOverlayActive) {
        if (handleDrawerBookOverlayInput()) {
            glutPostRedisplay();
        }
        return;
    }

    if (codeBoxOverlayActive) {
        hideCodeBoxOverlay();
        glutPostRedisplay();
        return;
    }

    if (button == GLUT_LEFT_BUTTON && mouseCaptured) {
        interactWithNearbyItem(camX, camY, camZ);
        glutPostRedisplay();
        return;
    }

    if (button == GLUT_LEFT_BUTTON && !mouseCaptured) {
        mouseCaptured = true;
        glutSetCursor(GLUT_CURSOR_NONE);
        return;
    }

    if (button == 3) { moveCamera(0.0f, 0.5f, 0.0f); glutPostRedisplay(); }
    if (button == 4) { moveCamera(0.0f, -0.5f, 0.0f); glutPostRedisplay(); }
}
