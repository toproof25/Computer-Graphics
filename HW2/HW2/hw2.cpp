#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <iostream>

static int Day = 0, Time = 0;

// gluLookAt 변수들
GLdouble eyex = 0, eyey = 0, eyez = 0;
GLdouble centerx = 0, centery = 0, centerz = -1;
GLdouble upx = 0, upy = 1, upz = 0;

// 목표 gluLookAt 변수들
GLdouble target_eyex = 0, target_eyey = 0, target_eyez = 0;
GLdouble target_centerx = 0, target_centery = 0, target_centerz = -1;
GLdouble target_upx = 0, target_upy = 1, target_upz = 0;

// 마우스 이동 좌표값 저장
GLdouble preX = 0.0, preY = 0.0;

// 시점 변화 토글
bool isChangeView = false;

// 자구, 달 공전 토글
bool isEarthRevolution = false;
bool isMoonRevolution = false;

// 시점 변화 속도
const GLdouble step = 0.03;


void DrawDirectionBox() {
    // 상 빨강 박스
    glPushMatrix();
    glTranslatef(0.0, 0.3, 0.0);  // 상
    glColor3f(1.0, 0.0, 0.0);
    glutSolidCube(0.05);
    glPopMatrix();

    // 하 파랑 박스
    glPushMatrix();
    glTranslatef(0.0, -0.3, 0.0);  // 하
    glColor3f(0.0, 0.0, 1.0); 
    glutSolidCube(0.05);
    glPopMatrix();

    // 좌 초록 박스
    glPushMatrix();
    glTranslatef(-0.3, 0.0, 0.0);  // 좌
    glColor3f(0.0, 1.0, 0.0);
    glutSolidCube(0.05);
    glPopMatrix();

    // 우 노랑 박스
    glPushMatrix();
    glTranslatef(0.3, 0.0, 0.0);  // 우
    glColor3f(1.0, 1.0, 0.0);
    glutSolidCube(0.05);
    glPopMatrix();

    // 앞 민트 박스
    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.3);  // 앞
    glColor3f(0.0, 1.0, 1.0);
    glutSolidCube(0.05);
    glPopMatrix();

    // 뒤 그레이 박스
    glPushMatrix();
    glTranslatef(0.0, 0.0, -0.3);  // 뒤
    glColor3f(0.5, 0.5, 0.5);
    glutSolidCube(0.05);
    glPopMatrix();
}

void MyDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(
        eyex, eyey, eyez,
        centerx, centery, centerz,
        upx, upy, upz
    );

    // 상하좌우앞뒤 구분 사각형 추가
    DrawDirectionBox();

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

        // 자동 공전 토클
        case '1':
            isEarthRevolution = !isEarthRevolution;
            break;
        case '2':
            isMoonRevolution = !isMoonRevolution;
            break;

        case 'q':
            // 기본 시점 (옆에서 본 모습)
            target_eyex = 0; target_eyey = 0; target_eyez = 0;
            target_centerx = 0; target_centery = 0; target_centerz = -1;
            target_upx = 0; target_upy = 1; target_upz = 0;
            isChangeView = true;
            glutPostRedisplay();
            break;
        case 'w':
            // 위에서 보는 시점
            target_eyex = 0; target_eyey = 0; target_eyez = 0.01;
            target_centerx = 0; target_centery = -1; target_centerz = 0;
            target_upx = 0; target_upy = 1; target_upz = 0;
            isChangeView = true;
            glutPostRedisplay();
            break;
        case 'e': // 시점 이동 종료
            isChangeView = false;
            glutPostRedisplay();
            break;

        case 'a': // 반대편 보기
            centerz *= -1;
            glutPostRedisplay();
            break;

        default:
            break;
    }
}

// 지구, 달이 계속 공전하도록 추가 
void TogleRevolution() {
    if (isEarthRevolution) { // 키 1 버튼
        Day = (Day + 1) % 360;
    }
    if (isMoonRevolution) { // 키 2 버튼
        Time = (Time + 2) % 360;
    }
}


void SetLookAtValue() {
    // 천천히 시점 변화 = 현재값 + (목표값-현재값*속도) // 조금씩 이동
    if (isChangeView) {
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
            abs(target_eyez - eyez) < 0.001 &&

            abs(target_centerx - centerx) < 0.001 &&
            abs(target_centery - centery) < 0.001 &&
            abs(target_centerz - centerz) < 0.001 
            )
        {
            isChangeView = false;
        }
    }
}

void MyIdle() {
    // Idle로 하면 속도가 매우 빠름
    // 조금 천천히 변경됐으면 좋겠어서 찾아보니 glutTimerFune()을 적용
    TogleRevolution();
    SetLookAtValue();
    glutPostRedisplay();  // 화면 새로 그리기
}

void Timer(int value) {
    TogleRevolution();
    SetLookAtValue();
    glutPostRedisplay();  // 화면 새로 그리기
    glutTimerFunc(16, Timer, 0);  // 다시 Timer를 실행 (계속해서 자신을 호출하여 반복)
}


// 마우스 이동 이벤트 좌표를 변수에 저장
void MyMouseMove(GLint X, GLint Y) {
    // 중앙이 0이 되도록 설정
    eyex = -((X - 250) / 250.0);
    eyey = (Y - 250) / 250.0;
    eyez = 0;
    printf("%lf %lf\n", eyex, eyey);
    glutPostRedisplay();
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
    glutMotionFunc(MyMouseMove); // 클릭 후 이동하면 시점 변경
    //glutIdleFunc(MyIdle); // Idle 콜백
    glutTimerFunc(16, Timer, 0);  // 화면을 계속 그리기 (시점 변경하는 모습을 자세하게 보기 위해)
    glutMainLoop();
    return 0;
}
