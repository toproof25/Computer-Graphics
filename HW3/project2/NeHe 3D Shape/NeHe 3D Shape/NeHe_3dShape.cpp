#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>

// gluLookAt 변수들
GLdouble eyex = 0, eyey = 0, eyez = 1;
GLdouble centerx = 0, centery = 0, centerz = -1;
GLdouble upx = 0, upy = 1, upz = 0;


GLfloat rtri = 0.0f;   // 피라미드 회전 각도
GLfloat rquad = 0.0f;  // 큐브 회전 각도

float turn_speed = 0.0f; // 회전 스피드 조절
float move_x = 0.0f, move_y = 0.0f, move_z = 0.0f; // 이동 좌표 조절

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity(); // 현재 행렬을 단위 행렬로 초기화

    gluLookAt(
        eyex, eyey, eyez,
        centerx, centery, centerz,
        upx, upy, upz
    );

    glPushMatrix(); // 현재 변환 행렬을 저장
    // 피라미드 그리기
    glTranslatef(-1.5f + move_x, 0.0f + move_y, -6.0f + move_z); // 피라미드 위치 이동
    glRotatef(rtri, 0.0f, 1.0f, 0.0f); // 피라미드 회전
    glBegin(GL_TRIANGLES); // 삼각형 그리기 시작
        // 피라미드의 앞면 그리기
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 1.0f, 0.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-1.0f, -1.0f, 1.0f);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(1.0f, -1.0f, 1.0f);

        // 피라미드의 오른쪽 면
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 1.0f, 0.0f);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(1.0f, -1.0f, 1.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(1.0f, -1.0f, -1.0f);

        // 피라미드의 뒷면
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 1.0f, 0.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(1.0f, -1.0f, -1.0f);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(-1.0f, -1.0f, -1.0f);

        // 피라미드의 왼쪽 면
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 1.0f, 0.0f);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(-1.0f, -1.0f, -1.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-1.0f, -1.0f, 1.0f);
    glEnd(); // 피라미드 종료
    glPopMatrix(); // 변환 행렬 복원

    // 큐브 그리기
    //glLoadIdentity(); // 현재 행렬 초기화
    glPushMatrix(); // 현재 변환 행렬을 저장
    glTranslatef(1.5f + move_x, 0.0f + move_y, -7.0f + move_z); // 큐브 위치 이동
    glRotatef(rquad, 1.0f, 1.0f, 1.0f); // 큐브 회전
    glBegin(GL_QUADS); // 사각형 그리기 시작
        // 큐브 각 면 그리기
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(1.0f, 1.0f, -1.0f);
        glVertex3f(-1.0f, 1.0f, -1.0f);
        glVertex3f(-1.0f, 1.0f, 1.0f);
        glVertex3f(1.0f, 1.0f, 1.0f);

        glColor3f(1.0f, 0.5f, 0.0f); // 아랫면 
        glVertex3f(1.0f, -1.0f, 1.0f);
        glVertex3f(-1.0f, -1.0f, 1.0f);
        glVertex3f(-1.0f, -1.0f, -1.0f);
        glVertex3f(1.0f, -1.0f, -1.0f);

        glColor3f(1.0f, 0.0f, 0.0f); // 앞면 
        glVertex3f(1.0f, 1.0f, 1.0f);
        glVertex3f(-1.0f, 1.0f, 1.0f);
        glVertex3f(-1.0f, -1.0f, 1.0f);
        glVertex3f(1.0f, -1.0f, 1.0f);

        glColor3f(1.0f, 1.0f, 0.0f); // 뒷면 
        glVertex3f(1.0f, -1.0f, -1.0f);
        glVertex3f(-1.0f, -1.0f, -1.0f);
        glVertex3f(-1.0f, 1.0f, -1.0f);
        glVertex3f(1.0f, 1.0f, -1.0f);

        glColor3f(0.0f, 0.0f, 1.0f); // 왼쪽 면 
        glVertex3f(-1.0f, 1.0f, 1.0f);
        glVertex3f(-1.0f, 1.0f, -1.0f);
        glVertex3f(-1.0f, -1.0f, -1.0f);
        glVertex3f(-1.0f, -1.0f, 1.0f);

        glColor3f(1.0f, 0.0f, 1.0f); // 오른쪽 면 
        glVertex3f(1.0f, 1.0f, -1.0f);
        glVertex3f(1.0f, 1.0f, 1.0f);
        glVertex3f(1.0f, -1.0f, 1.0f);
        glVertex3f(1.0f, -1.0f, -1.0f);
    glEnd(); // 사각형 종료
    glPopMatrix(); // 변환 행렬 복원

    rtri += 0.1f + turn_speed; // 피라미드 회전 업데이트
    rquad -= 0.075f + turn_speed; // 큐브 회전 업데이트


    glutSwapBuffers();
}

void MyKeyboard(unsigned char key, int x, int y) {
    switch (key) {
    case '+':
        turn_speed += 0.01f;
        printf("피라미드 회전 스피드(양수:오른쪽, 음수:왼쪽) : %.2f\n", 0.1f + turn_speed);
        printf("큐브 회전 스피드(양수:오른쪽, 음수:왼쪽) : %.2f\n\n", 0.075f + turn_speed);
        break;
    case '-':
        turn_speed -= 0.01f;
        printf("피라미드 회전 스피드(양수:오른쪽, 음수:왼쪽) : %.2f\n", 0.1f + turn_speed);
        printf("큐브 회전 스피드(양수:오른쪽, 음수:왼쪽) : %.2f\n\n", 0.075f + turn_speed);
        break;


    case 'q':
        move_z += 0.1f;
        printf("이동 좌표 값 move_z (x: %.2f \t y: %.2f \t z: %.2f)\n", move_x, move_y, move_z);
        break;
    case 'e':
        move_z -= 0.1f;
        printf("이동 좌표 값 move_z (x: %.2f \t y: %.2f \t z: %.2f)\n", move_x, move_y, move_z);
        break;
    case 'a':
        move_x -= 0.1f;
        printf("이동 좌표 값 move_x (x: %.2f \t y: %.2f \t z: %.2f)\n", move_x, move_y, move_z);
        break;
    case 'd':
        move_x += 0.1f;
        printf("이동 좌표 값 move_x (x: %.2f \t y: %.2f \t z: %.2f)\n", move_x, move_y, move_z);
        break;
    case 'w':
        move_y += 0.1f;
        printf("이동 좌표 값 move_y (x: %.2f \t y: %.2f \t z: %.2f)\n", move_x, move_y, move_z);
        break;
    case 's':
        move_y -= 0.1f;
        printf("이동 좌표 값 move_y (x: %.2f \t y: %.2f \t z: %.2f)\n", move_x, move_y, move_z);
        break;

    default:
        break;
    }


}

// 마우스 이동 이벤트 좌표를 변수에 저장
void MyMouseMove(GLint X, GLint Y) {
    // 중앙이 0이 되도록 설정
    eyex = -((X - 320) / 320.0);
    eyey = (Y - 240) / 240.0;
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    // 초기 설정
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(640, 480);
    glutCreateWindow("과제2-문제2번");

    // 화면 설정
    glMatrixMode(GL_PROJECTION); // 투영 행렬
    glLoadIdentity();
    gluPerspective(45.0, (double)640 / 480, 1.0, 100.0); 
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);

    // 콜백 함수 등록
    glutKeyboardFunc(MyKeyboard);
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutMotionFunc(MyMouseMove);

    glutMainLoop();
    return 0;
}
