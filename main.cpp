#include <GL/glut.h>
#include <cmath>
#include "camera/camera.h"
#include "room/room.h"
#include "objects/objects.h"
#include "utils/utils.h"          // ← ADD

static int lastTime = 0;          // ← ADD for delta time

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

    drawRoom();
    drawDoor();
    drawTable();
    drawChair();
    drawDustbin();
    drawBookshelf();

    // Proximity prompt (only when no timed message is showing)
    if (hudMessageTimer <= 0.0f) {
        int nearby = getNearbyItem(camX, camY, camZ);
        if (nearby >= 0) {
            Item& item = worldItems[nearby];
            hudMessage = "Press E to " + item.action + " " + item.name;
        } else {
            hudMessage = "";
        }
    }

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
    glEnable(GL_BLEND);                                    // ← ADD
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    // ← ADD
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

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