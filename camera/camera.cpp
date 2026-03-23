#include <GL/glut.h>
#include <cmath>
#include "camera.h"
#include "../utils/utils.h"   // ← add this for worldItems, inventory, etc.

// --- Constants ---
static const float PI          = 3.14159265f;
static const float DEG2RAD     = PI / 180.0f;
static const float MOVE_SPEED  = 0.15f;
static const float LOOK_SENS   = 0.15f;
static const float PITCH_LIMIT = 89.0f;

float camX = 0.0f, camY = 1.0f, camZ = 5.0f;
float angleY = 0.0f;
float angleX = 0.0f;
int   winW = 900, winH = 600;
bool  warping       = false;
bool  mouseCaptured = true;

// --- Helpers ---
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
    float fx, fy, fz, rx, rz;
    forward3D(fx, fy, fz);
    rightXZ(rx, rz);

    switch (key) {
        // --- Movement ---
        case 'w': case 'W':
            camX += fx * MOVE_SPEED;
            camY += fy * MOVE_SPEED;
            camZ += fz * MOVE_SPEED;
            break;
        case 's': case 'S':
            camX -= fx * MOVE_SPEED;
            camY -= fy * MOVE_SPEED;
            camZ -= fz * MOVE_SPEED;
            break;
        case 'a': case 'A':
            camX -= rx * MOVE_SPEED;
            camZ -= rz * MOVE_SPEED;
            break;
        case 'd': case 'D':
            camX += rx * MOVE_SPEED;
            camZ += rz * MOVE_SPEED;
            break;
        case ' ':
            camY += MOVE_SPEED; break;
        case 'c': case 'C':
            camY -= MOVE_SPEED; break;

        // --- Interact ---
        case 'e': case 'E': {
            int idx = getNearbyItem(camX, camY, camZ);
            if (idx >= 0) {
                Item& item = worldItems[idx];

                if (item.name == "Key") {
                    inventory.push_back(item.name);
                    item.pickedUp = true;
                    setHudMessage("Picked up: Key");

                } else if (item.name == "Chest") {
                    if (hasInInventory("Key")) {
                        item.pickedUp = true;
                        setHudMessage("Chest opened! Something inside...");
                    } else {
                        setHudMessage("The chest is locked. Find the key first.");
                    }

                } else if (item.name == "Door") {
                    if (hasInInventory("Key")) {
                        item.pickedUp = true;
                        setHudMessage("You escaped! Press ESC to quit.", 9999.0f);
                        printf("\n Congratulations! You escaped the room!\n");
                    } else {
                        setHudMessage("The door is locked. You need a key.");
                    }
                }
            }
            break;
        }

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
    switch (key) {
        case GLUT_KEY_LEFT:  angleY -= 5.0f; break;
        case GLUT_KEY_RIGHT: angleY += 5.0f; break;
        case GLUT_KEY_UP:    angleX -= 3.0f; clampPitch(); break;
        case GLUT_KEY_DOWN:  angleX += 3.0f; clampPitch(); break;
    }
    glutPostRedisplay();
}

void mouseMotion(int x, int y) {
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

    if (button == GLUT_LEFT_BUTTON && !mouseCaptured) {
        mouseCaptured = true;
        glutSetCursor(GLUT_CURSOR_NONE);
        return;
    }

    if (button == 3) { camY += 0.5f; glutPostRedisplay(); }
    if (button == 4) { camY -= 0.5f; glutPostRedisplay(); }
}