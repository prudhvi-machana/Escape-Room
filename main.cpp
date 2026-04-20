#include <GL/glut.h>
#include "camera/camera.h"
#include "room/room.h"
#include "utils/utils.h"
#include "app/render.h"

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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glClearColor(0.70f, 0.86f, 0.98f, 1.0f);

    initTextures();
    initRoom();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouseButton);
    glutPassiveMotionFunc(mouseMotion);
    glutIdleFunc([]{ glutPostRedisplay(); });

    glutSetCursor(GLUT_CURSOR_NONE);
    glutWarpPointer(winW / 2, winH / 2);

    glutMainLoop();
    return 0;
}
