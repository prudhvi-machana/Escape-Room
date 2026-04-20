#include <GL/glut.h>
#include "../camera/camera.h"
#include "../room/room.h"
#include "../objects/objects.h"
#include "../utils/utils.h"
#include "../utils/lighting.h"
#include "render.h"

namespace {

int gLastTime = 0;

void setupLights() {
    GLfloat roomAmbient[] = {
        roomLightOn ? 0.24f : 0.02f,
        roomLightOn ? 0.22f : 0.02f,
        roomLightOn ? 0.19f : 0.03f,
        1.0f
    };
    GLfloat roomDiffuse[] = {
        roomLightOn ? 1.25f : 0.0f,
        roomLightOn ? 1.16f : 0.0f,
        roomLightOn ? 1.02f : 0.0f,
        1.0f
    };
    GLfloat roomSpecular[] = {
        roomLightOn ? 0.95f : 0.0f,
        roomLightOn ? 0.88f : 0.0f,
        roomLightOn ? 0.76f : 0.0f,
        1.0f
    };

    GLfloat light0Pos[]  = { lighting::kCeilingLightX, lighting::kCeilingLightY, lighting::kCeilingLightZ, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  roomAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  roomDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, roomSpecular);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.45f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION,   0.03f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION,0.004f);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0f);

    GLfloat light1Pos[]  = { lighting::kDeskLightX, lighting::kDeskLightY, lighting::kDeskLightZ, 1.0f };
    GLfloat light1Dir[]  = { 0.2f, -0.95f, -0.25f };
    GLfloat light1Amb[]  = { 0.05f, 0.04f, 0.02f, 1.0f };
    GLfloat light1Diff[] = { 1.15f, 0.96f, 0.70f, 1.0f };
    GLfloat light1Spec[] = { 0.55f, 0.45f, 0.30f, 1.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1Dir);
    glLightfv(GL_LIGHT1, GL_AMBIENT,  light1Amb);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  light1Diff);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1Spec);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.70f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION,   0.08f);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION,0.02f);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 42.0f);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 8.0f);
}

} // namespace

void display() {
    const int now = glutGet(GLUT_ELAPSED_TIME);
    const float delta = (now - gLastTime) / 1000.0f;
    gLastTime = now;
    updateHud(delta);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float viewX = 0.0f;
    float viewY = 0.0f;
    float viewZ = 0.0f;
    forward3D(viewX, viewY, viewZ);

    gluLookAt(camX, camY, camZ,
              camX + viewX, camY + viewY, camZ + viewZ,
              0, 1, 0);
    setupLights();

    drawRoom();
    drawDoor();
    drawTable();
    drawChair();
    drawDustbin();
    drawBookshelf();
    drawBed();
    drawPainting();

    drawHUD(winW, winH);
    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h <= 0) h = 1;
    winW = w;
    winH = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, static_cast<double>(w) / static_cast<double>(h), 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}
