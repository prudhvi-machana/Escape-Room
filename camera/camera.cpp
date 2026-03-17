#include <GL/glut.h>
#include <cmath>
#include "camera.h"

float camX = 0.0f, camY = 1.0f, camZ = 5.0f;
float angleY = 0.0f;
float angleX = 0.0f;
int winW = 900, winH = 600;
bool warping = false;

void keyboard(unsigned char key, int x, int y) {
    float speed = 0.3f;
    float radY  = angleY * 3.14159f / 180.0f;

    switch(key) {
        case 'w': case 'W':
            camX += sin(radY) * speed;
            camZ -= cos(radY) * speed;
            break;
        case 's': case 'S':
            camX -= sin(radY) * speed;
            camZ += cos(radY) * speed;
            break;
        case 'a': case 'A':
            camX -= cos(radY) * speed;
            camZ -= sin(radY) * speed;
            break;
        case 'd': case 'D':
            camX += cos(radY) * speed;
            camZ += sin(radY) * speed;
            break;
        case 27:
            exit(0);
    }
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    switch(key) {
        case GLUT_KEY_LEFT:
            angleY -= 5.0f; break;
        case GLUT_KEY_RIGHT:
            angleY += 5.0f; break;
        case GLUT_KEY_UP:
            angleX -= 3.0f;
            if(angleX < -60.0f) angleX = -60.0f;
            break;
        case GLUT_KEY_DOWN:
            angleX += 3.0f;
            if(angleX > 60.0f) angleX = 60.0f;
            break;
    }
    glutPostRedisplay();
}

void mouseMotion(int x, int y) {
    if(warping) {
        warping = false;
        return;
    }

    int centerX = winW / 2;
    int centerY = winH / 2;
    int dx = x - centerX;
    int dy = y - centerY;

    if(dx == 0 && dy == 0) return;

    float sensitivity = 0.15f;
    angleY += dx * sensitivity;
    angleX += dy * sensitivity;

    if(angleX < -60.0f) angleX = -60.0f;
    if(angleX >  60.0f) angleX =  60.0f;

    warping = true;
    glutWarpPointer(centerX, centerY);
    glutPostRedisplay();
}

void mouseButton(int button, int state, int x, int y) {
    float speed = 0.5f;
    float radY  = angleY * 3.14159f / 180.0f;

    if(button == 3 && state == GLUT_DOWN) {
        camX += sin(radY) * speed;
        camZ -= cos(radY) * speed;
        glutPostRedisplay();
    }
    if(button == 4 && state == GLUT_DOWN) {
        camX -= sin(radY) * speed;
        camZ += cos(radY) * speed;
        glutPostRedisplay();
    }
}