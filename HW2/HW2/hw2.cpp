#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <iostream>

static int Day = 0, Time = 0;


void MyDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 태양계 위에서 보기 추가
    gluLookAt(
        0, 0, 0.1,
        0.0, -100.0, 0.0,
        0.0, 1.0, .0
    );


    glColor3f(1.0, 0.3, 0.3);
    glutWireSphere(0.2, 20, 16);

    glPushMatrix();
    glRotatef((GLfloat)Day, 0.0, 1.0, 0.0);
    glTranslatef(0.7, 0.0, 0.0);
    glRotatef((GLfloat)Time, 0.0, 1.0, 0.0);
    glColor3f(0.5, 0.6, 0.7);
    glutWireSphere(0.1, 10, 8);

    glPushMatrix();
    glRotatef((GLfloat)Time, 0.0, 1.0, 0.0);
    glTranslatef(0.2, 0.0, 0.0);
    glColor3f(0.9, 0.8, 0.2);
    glutWireSphere(0.04, 10, 8);

    glPopMatrix();
    glPopMatrix();

    glutSwapBuffers();
}

void MyKeyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'd':
        Day = (Day + 10) % 360;
        glutPostRedisplay();
        break;
    case 't':
        Time = (Time + 5) % 360;
        glutPostRedisplay();
        break;

    default:
        break;
    }

}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("보너스 태양계 OpenGL");
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glutDisplayFunc(MyDisplay);
    glutKeyboardFunc(MyKeyboard);
    glutMainLoop();
    return 0;
}