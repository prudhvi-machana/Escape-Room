// camera/camera.h
#pragma once

extern float camX, camY, camZ;
extern float angleY, angleX;
extern int   winW, winH;
extern bool  warping, mouseCaptured;

void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void mouseMotion(int x, int y);
void mouseButton(int button, int state, int x, int y);
void forward3D(float& fx, float& fy, float& fz);
void rightXZ(float& rx, float& rz);
void clampPitch();