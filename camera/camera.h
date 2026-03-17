#ifndef CAMERA_H
#define CAMERA_H

extern float camX, camY, camZ;
extern float angleY, angleX;
extern int winW, winH;

void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void mouseMotion(int x, int y);
void mouseButton(int button, int state, int x, int y);

#endif