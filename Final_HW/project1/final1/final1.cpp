#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <cmath>

// 디스플레이 리스트 ID
GLuint displayListId;

// 원뿔과 원기둥, 바닥을 그리는 함수
void createDisplayList() {
    displayListId = glGenLists(1); // 디스플레이 리스트 생성
    glNewList(displayListId, GL_COMPILE); // 리스트 시작

    // 바닥 그리기 (6면체)
    glColor3f(0.549f, 0.227f, 0.086f); // 흙색
    glBegin(GL_QUADS);
    // 바닥 면
    glVertex3f(-2.0f, -2.0f, 0.0f);
    glVertex3f(2.0f, -2.0f, 0.0f);
    glVertex3f(2.0f, 2.0f, 0.0f);
    glVertex3f(-2.0f, 2.0f, 0.0f);
    glEnd();

    // 원기둥 그리기
    glColor3f(0.545f, 0.271f, 0.075f); // 갈색
    GLUquadric* quadric = gluNewQuadric();
    gluCylinder(quadric, 0.5, 0.5, 1.0, 32, 32); // 원기둥 그리기
    gluDisk(quadric, 0.0, 0.5, 32, 1); // 바닥 그리기
    glTranslatef(0.0f, 0.0f, 1.0f); // 원기둥의 끝으로 이동
    gluDisk(quadric, 0.0, 0.5, 32, 1); // 원기둥의 상단 그리기

    // 원뿔 그리기
    glColor3f(0.0f, 1.0f, 0.0f); // 초록색
    glTranslatef(0.0f, 0.0f, 1.0f); // 원기둥의 끝에서 이동
    gluCylinder(quadric, 0.0, 0.5, 1.0, 32, 32); // 원뿔 그리기
    gluDisk(quadric, 0.0, 0.5, 32, 1); // 원뿔의 바닥 그리기

    gluDeleteQuadric(quadric); // 자원 해제
    glEndList(); // 리스트 종료
}

// 디스플레이 콜백 함수
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -5.0f); // 카메라 위치 조정

    glCallList(displayListId); // 디스플레이 리스트 호출

    glutSwapBuffers();
}

// 초기화 함수
void init() {
    glEnable(GL_DEPTH_TEST); // 깊이 테스트 활성화
    createDisplayList(); // 디스플레이 리스트 생성

    // 투영 행렬
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 800.0 / 600.0, 0.1, 100.0); // 원근 투영으로 3D로 보기 위해 (시야각, 가로세로비율, 가까운 거 잘라내기 먼거 잘라내기
}

// 메인 함수
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Cone and Cylinder with Ground");

    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}