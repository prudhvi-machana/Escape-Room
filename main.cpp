#include <GL/glut.h>
#include <cmath>
#include "camera/camera.h"
#include "room/room.h"
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float radY = angleY * 3.14159f / 180.0f;
    float radX = angleX * 3.14159f / 180.0f;

    float lookX = camX + sin(radY) * cos(radX);
    float lookY = camY - sin(radX);
    float lookZ = camZ - cos(radY) * cos(radX);

    gluLookAt(
        camX, camY, camZ,
        lookX, lookY, lookZ,
        0, 1, 0
    );

    drawRoom();
    drawDoor();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    winW = w; winH = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)w/h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(winW, winH);
    glutCreateWindow("Escape Room");

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouseButton);
    glutPassiveMotionFunc(mouseMotion);

    glutSetCursor(GLUT_CURSOR_NONE);
    glutWarpPointer(winW/2, winH/2);

    glutMainLoop();
    return 0;
}