#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <iostream>

static int Day = 0, Time = 0;

GLdouble eyex = 0, eyey = 0, eyez = 0;
GLdouble centerx = 0, centery = 0, centerz = -1;
GLdouble upx = 0, upy = 1, upz = 0;

GLdouble target_eyex = 0, target_eyey = 0, target_eyez = 0;
GLdouble target_centerx = 0, target_centery = 0, target_centerz = -1;
GLdouble target_upx = 0, target_upy = 1, target_upz = 0;

// 시점 변화 토글
bool isAnimating = false;

// 속도
const GLdouble step = 0.01;

void MyDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 천천히 시점 변화
    if (isAnimating) {
        eyex += (target_eyex - eyex) * step;
        eyey += (target_eyey - eyey) * step;
        eyez += (target_eyez - eyez) * step;

        centerx += (target_centerx - centerx) * step;
        centery += (target_centery - centery) * step;
        centerz += (target_centerz - centerz) * step;

        upx += (target_upx - upx) * step;
        upy += (target_upy - upy) * step;
        upz += (target_upz - upz) * step;

        // 목표값과 유사한 실수일 경우 종료 (즉 시점 변화가 끝나면)
        if (abs(target_eyex - eyex) < 0.001 &&
            abs(target_eyey - eyey) < 0.001 &&
            abs(target_eyez - eyez) < 0.001) {
            isAnimating = false;
        }
    }

    gluLookAt(
        eyex, eyey, eyez,
        centerx, centery, centerz,
        upx, upy, upz
    );

    // 기존 코드
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

    case 'q':
        // 기본 시점 (옆에서 본 모습)
        target_eyex = 0; target_eyey = 0; target_eyez = 0;
        target_centerx = 0; target_centery = 0; target_centerz = -1;
        target_upx = 0; target_upy = 1; target_upz = 0;
        isAnimating = true;
        glutPostRedisplay();
        break;

    case 'w':
        // 위에서 보는 시점
        target_eyex = 0; target_eyey = 0; target_eyez = 0.1;
        target_centerx = 0; target_centery = -1; target_centerz = 0;
        target_upx = 0; target_upy = 1; target_upz = 0;
        isAnimating = true;
        glutPostRedisplay();
        break;

    default:
        break;
    }
}

void Timer(int value) {
    glutPostRedisplay();  // 화면 새로 그리기
    glutTimerFunc(16, Timer, 0);  // 대략 60 프레임 호출
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
    glutTimerFunc(16, Timer, 0);  // 60프레임으로 화면을 계속 그리기 (시점 변경하는 모습을 자세하게 보기 위해)
    glutMainLoop();
    return 0;
}
