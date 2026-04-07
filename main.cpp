#include <GL/glut.h>
#include <cmath>
#include "camera/camera.h"
#include "room/room.h"
#include "objects/objects.h"
#include "utils/utils.h"          // ← ADD
#include "utils/lighting.h"

static int lastTime = 0;          // ← ADD for delta time

namespace {

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

    // Main ceiling fixture light.
    GLfloat light0Pos[]  = { lighting::kCeilingLightX, lighting::kCeilingLightY, lighting::kCeilingLightZ, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  roomAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  roomDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, roomSpecular);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.45f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION,   0.03f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION,0.004f);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0f);

    // Desk lamp cone light.
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
    // Delta time for HUD timer
    int   now   = glutGet(GLUT_ELAPSED_TIME);
    float delta = (now - lastTime) / 1000.0f;
    lastTime    = now;
    updateHud(delta);             // ← ADD

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float radY  = angleY * 3.14159f / 180.0f;
    float radX  = angleX * 3.14159f / 180.0f;
    float lookX = camX + sinf(radY) * cosf(radX);
    float lookY = camY - sinf(radX);
    float lookZ = camZ - cosf(radY) * cosf(radX);

    gluLookAt(camX, camY, camZ, lookX, lookY, lookZ, 0, 1, 0);
    setupLights();

    drawRoom();
    drawDoor();
    drawTable();
    drawChair();
    drawBookshelf();
    drawBed();
    drawPainting();

    drawHUD(winW, winH);          // ← ADD (always last)
    glutSwapBuffers();
}

void reshape(int w, int h) {
    winW = w; winH = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)w / h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(winW, winH);
    glutCreateWindow("Escape Room");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    GLfloat sceneAmbient[] = { 0.08f, 0.08f, 0.09f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, sceneAmbient);
    glEnable(GL_BLEND);                                    // ← ADD
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    // ← ADD
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glClearColor(0.14f, 0.13f, 0.12f, 1.0f);

    initTextures();
    initRoom();                                            // ← ADD

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouseButton);
    glutPassiveMotionFunc(mouseMotion);
    glutIdleFunc([]{ glutPostRedisplay(); });               // ← ADD (key bob)

    glutSetCursor(GLUT_CURSOR_NONE);
    glutWarpPointer(winW / 2, winH / 2);

    lastTime = glutGet(GLUT_ELAPSED_TIME);                 // ← ADD
    glutMainLoop();
    return 0;
}
