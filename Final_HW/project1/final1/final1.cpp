#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>

#include <algorithm>
#include <cmath>
#include <iostream>

// 나무, 바닥, 입간판
GLuint treeDisplayList;
GLuint groundDisplayList;
GLuint signboardDisplayList;

// 윈도우 크기
const int window_width = 1920;
const int window_height = 1080;

// 도움말 토글
bool help = true;

// 카메라 위치, 방향
float cameraX = 0.0f;
float cameraY = 1.0f;
float cameraZ = 5.0f;
float cameraYaw = 0.0f;    // 수평 회전
float cameraPitch = 0.0f;  // 수직 회전

// 마우스 관련 변수
int lastX = window_width/2;  // 윈도우 중앙을 0으로
int lastY = window_height/2;

bool firstMoveMouse = true; // 처음 움직일 때 lastX, lastY을 저장하기 위해

const float mouseSpeed = 0.005f; //   마우스 속도

// 키 상태 - 키에 모든 배열을 false로 초기화 -> W, A, S, D 키에 대해서 true로 설정
bool keys[256] = { false };
bool W = false, A = false, S = false, D = false;

// 이동 관련 변수
const float moveSpeed = 0.001f;

// 파이
const float PI = 3.14; 

bool morning = true;  // 아침 or 저녁 여부
GLfloat morningLight[] = { 0.5f, 1.0f, 0.5f, 0.0f };
GLfloat morningAmbient[] = { 0.3f, 0.3f, 0.4f, 1.0f };  
GLfloat morningDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f }; 

GLfloat eveningLight[] = { -0.5f, 0.2f, -0.5f, 0.0f };  
GLfloat eveningAmbient[] = { 0.4f, 0.3f, 0.2f, 1.0f };  
GLfloat eveningDiffuse[] = { 1.0f, 0.8f, 0.5f, 1.0f };  

// 나무 위치 
const int numTrees = 100; // 나무 개수
const float treePositions[numTrees][2] = {
    { -14.0f,  12.5f}, { 10.5f, -14.2f}, {  3.0f, -13.0f}, { -8.0f,  14.5f}, {  9.0f,   7.0f},
    { -15.0f,   4.0f}, { 12.0f,   6.5f}, { -13.5f, -5.0f}, {  7.5f, -12.0f}, { -2.0f,  10.0f},
    { 11.0f,   3.5f}, { -6.0f,  -8.0f}, {  5.5f,  11.5f}, { -4.0f, -14.0f}, {  3.5f,  15.0f},
    { -11.0f,   0.5f}, { 14.5f,  -9.0f}, { -5.5f,   7.0f}, { 12.5f, -12.0f}, { -10.0f,  14.5f},
    {  8.5f,  -6.0f}, { -3.5f,   8.0f}, {  7.0f,   9.5f}, { -14.5f, -10.0f}, { 13.0f, -11.0f},
    { -12.0f,   4.5f}, {  4.0f, -15.0f}, { -7.5f,  13.5f}, { 11.5f,   2.0f}, {  6.5f, -14.0f},
    {  3.0f,  12.0f}, { -10.5f,  14.0f}, {  2.0f,   5.5f}, {  9.5f, -13.5f}, { -8.5f,  15.0f},
    { 14.0f,   6.0f}, { 12.0f,  -2.5f}, {  5.5f,  13.0f}, { -15.0f,  -3.0f}, { 10.5f,  -9.0f},
    { -13.0f,  -7.5f}, {  6.0f,  10.5f}, {  8.0f,  -5.5f}, { -7.0f,  11.0f}, { 13.5f,   0.0f},
    {  1.5f,   6.5f}, {  4.5f, -10.0f}, { -9.5f,   8.0f}, { -14.5f,   3.0f}, {  7.5f, -11.5f},
    { -5.0f,  14.5f}, { 15.0f,  -6.5f}, { -3.0f,   5.0f}, {  9.0f,   8.5f}, { -12.5f, -13.0f},
    { 11.0f,  -4.5f}, {  8.5f,  14.0f}, { -4.5f, -12.0f}, {  3.5f,  15.0f}, { -10.0f,  13.0f},
    {  2.5f,   6.0f}, { 12.0f, -10.5f}, { -6.5f,   8.0f}, {  5.0f,  -3.5f}, { 14.5f,  -2.0f},
    {  0.0f,  10.0f}, { -15.0f, -14.0f}, {  7.0f,  12.5f}, { -13.0f,   9.0f}, { 11.5f,   4.5f},
    {  6.5f, -12.0f}, { -8.5f,  10.0f}, {  3.0f,  -6.5f}, { -14.0f,   0.5f}, { 15.0f,   9.5f},
    { -5.0f,   7.5f}, {  9.0f, -13.5f}, { -4.0f,  11.0f}, { 12.5f,  -8.0f}, {  8.0f,   3.5f},
    { -9.5f,   6.0f}, { 14.5f, -10.0f}, {  4.0f,  15.0f}, { -6.0f,  -9.5f}, { 13.0f,   2.0f},
    {  7.5f,  -4.0f}, { -11.5f,  13.5f}, {  6.0f,   8.0f}, { -14.5f,   4.5f}, { 10.5f,  -7.0f}
};


// 디스플레이 리스트 - 입간판
void initSignboard() {
    signboardDisplayList = glGenLists(1);
    glNewList(signboardDisplayList, GL_COMPILE);


    // 입간판 재질 설정
    GLfloat signboardMaterial[] = { 0.55f, 0.27f, 0.07f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, signboardMaterial);

    // 입간판 기둥 (가로, 세로 좁고 높이 긴 6면체)
    glBegin(GL_QUADS);

    // 앞면
    glVertex3f(-0.1f, 0.0f, 0.1f);
    glVertex3f(0.1f, 0.0f, 0.1f);
    glVertex3f(0.1f, 0.5f, 0.1f);
    glVertex3f(-0.1f, 0.5f, 0.1f);

    // 뒷면
    glVertex3f(-0.1f, 0.0f, -0.1f);
    glVertex3f(0.1f, 0.0f, -0.1f);
    glVertex3f(0.1f, 0.5f, -0.1f);
    glVertex3f(-0.1f, 0.5f, -0.1f);

    // 왼쪽 면
    glVertex3f(-0.1f, 0.0f, -0.1f);
    glVertex3f(-0.1f, 0.0f, 0.1f);
    glVertex3f(-0.1f, 0.5f, 0.1f);
    glVertex3f(-0.1f, 0.5f, -0.1f);

    // 오른쪽 면
    glVertex3f(0.1f, 0.0f, -0.1f);
    glVertex3f(0.1f, 0.0f, 0.1f);
    glVertex3f(0.1f, 0.5f, 0.1f);
    glVertex3f(0.1f, 0.5f, -0.1f);

    // 윗면
    glVertex3f(-0.1f, 0.5f, 0.1f);
    glVertex3f(0.1f, 0.5f, 0.1f);
    glVertex3f(0.1f, 0.5f, -0.1f);
    glVertex3f(-0.1f, 0.5f, -0.1f);

    // 아랫면
    glVertex3f(-0.1f, 0.0f, 0.1f);
    glVertex3f(0.1f, 0.0f, 0.1f);
    glVertex3f(0.1f, 0.0f, -0.1f);
    glVertex3f(-0.1f, 0.0f, -0.1f);

    glEnd();

    // 입간판 화면 (세로 좁고 가로, 높이 긴 6면체)
    glBegin(GL_QUADS);

    // 앞면 (텍스처 적용)
    glVertex3f(-0.6f, 0.5f, 0.1f);
    glVertex3f(0.6f, 0.5f, 0.1f);
    glVertex3f(0.6f, 1.0f, 0.1f);
    glVertex3f(-0.6f, 1.0f, 0.1f);

    // 뒷면 (텍스처 적용)
    glVertex3f(-0.6f, 0.5f, -0.1f);
    glVertex3f(0.6f, 0.5f, -0.1f);
    glVertex3f(0.6f, 1.0f, -0.1f);
    glVertex3f(-0.6f, 1.0f, -0.1f);

    // 왼쪽 면
    glVertex3f(-0.6f, 0.5f, -0.1f);
    glVertex3f(-0.6f, 0.5f, 0.1f);
    glVertex3f(-0.6f, 1.0f, 0.1f);
    glVertex3f(-0.6f, 1.0f, -0.1f);

    // 오른쪽 면
    glVertex3f(0.6f, 0.5f, -0.1f);
    glVertex3f(0.6f, 0.5f, 0.1f);
    glVertex3f(0.6f, 1.0f, 0.1f);
    glVertex3f(0.6f, 1.0f, -0.1f);

    // 윗면
    glVertex3f(-0.6f, 1.0f, 0.1f);
    glVertex3f(0.6f, 1.0f, 0.1f);
    glVertex3f(0.6f, 1.0f, -0.1f);
    glVertex3f(-0.6f, 1.0f, -0.1f);

    // 아랫면
    glVertex3f(-0.6f, 0.5f, 0.1f);
    glVertex3f(0.6f, 0.5f, 0.1f);
    glVertex3f(0.6f, 0.5f, -0.1f);
    glVertex3f(-0.6f, 0.5f, -0.1f);

    glEnd();
    glDisable(GL_TEXTURE_2D);

    glEndList();
}

// 디스플레이 리스트 - 나무
void initTree() {

    //
    treeDisplayList = glGenLists(1);
    glNewList(treeDisplayList, GL_COMPILE);

    // 재질 설정
    GLfloat brownMaterial[] = { 0.55f, 0.27f, 0.07f, 1.0f };  // 갈색 줄기
    GLfloat greenMaterial[] = { 0.0f, 0.5f, 0.0f, 1.0f };    // 초록색 나뭇잎

    // 줄기 (원기둥)
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, brownMaterial);
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    GLUquadric* quadric = gluNewQuadric();
    gluCylinder(quadric, 0.2, 0.2, 1.0, 20, 1);
    gluDeleteQuadric(quadric);
    glPopMatrix();

    // 윗부분 나뭇잎 (첫 번째 원뿔) 
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, greenMaterial);
    glPushMatrix();
    glTranslatef(0, 1.0, 0);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* coneQuadric1 = gluNewQuadric();
    gluCylinder(coneQuadric1, 0.6, 0.0, 1.0, 20, 1);
    gluDeleteQuadric(coneQuadric1);
    glPopMatrix();

    // 아랫부분 나뭇잎 (두 번째 원뿔)
    glPushMatrix();
    glTranslatef(0, 1.5, 0);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* coneQuadric2 = gluNewQuadric();
    gluCylinder(coneQuadric2, 0.8, 0.0, 1.0, 20, 1);
    gluDeleteQuadric(coneQuadric2);
    glPopMatrix();

    glEndList();
}

// 디스플레이 리스트 - 바닥
void initGround() {
    groundDisplayList = glGenLists(1);
    glNewList(groundDisplayList, GL_COMPILE);

    // 지면 재질 설정
    GLfloat groundMaterial[] = { 0.4f, 0.6f, 0.4f, 1.0f };  // 잔디 색상
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, groundMaterial);

    // 큰 6면체 바닥 생성
    glBegin(GL_QUADS);
    // 바닥면
    glNormal3f(0, 1, 0);
    glVertex3f(-15, 0, -15);
    glVertex3f(15, 0, -15);
    glVertex3f(15, 0, 15);
    glVertex3f(-15, 0, 15);
    glEnd();

    glEndList();
}

// OpenGL 초기화 함수
void initLight() {
    // 렌더링 설정
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    if (morning) {
        glClearColor(0.7f, 0.9f, 1.0f, 1.0f);
        // 빛 위치 및 색 설정
        glLightfv(GL_LIGHT0, GL_POSITION, morningLight);
        glLightfv(GL_LIGHT0, GL_AMBIENT, morningAmbient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, morningDiffuse);
    }
    else {
        glClearColor(0.9f, 0.6f, 0.4f, 1.0f);
        // 빛 위치 및 색 설정
        glLightfv(GL_LIGHT0, GL_POSITION, eveningLight);
        glLightfv(GL_LIGHT0, GL_AMBIENT, eveningAmbient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, eveningDiffuse);
    }

}

// 화면에 텍스트 출력
void CreateText(float x, float y, float z, void* font, const std::string& text) {
    glRasterPos3f(x, y, z);
    for (char c : text) {
        glutBitmapCharacter(font, c);
    }
}

// 디스플레이 콜백 함수
void MyDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // 카메라 방향 벡터 계산
    float lookX = sin(cameraYaw) * cos(cameraPitch);
    float lookY = sin(cameraPitch);
    float lookZ = -cos(cameraYaw) * cos(cameraPitch);

    gluLookAt(
        cameraX, cameraY, cameraZ,                          // 카메라 위치
        cameraX + lookX, cameraY + lookY, cameraZ + lookZ,  // 바라보는 지점
        0.0, 1.0, 0.0                                       // 위쪽 방향
    );


    // 바닥 렌더링
    glCallList(groundDisplayList);

    // 입간판 렌더링
    glPushMatrix();
    glTranslatef(3, 0, 3); // X, Z 위치로 이동
    glCallList(signboardDisplayList);
    glPopMatrix();

    // 나무 렌더링
    for (int i = 0; i < numTrees; ++i) {
        glPushMatrix();
        glTranslatef(treePositions[i][0], 0, treePositions[i][1]); // X, Z 위치로 이동
        glCallList(treeDisplayList); // 나무 렌더링
        glPopMatrix();
    }

    // 도움말 출력
    if (help) {
        CreateText(2.9f, 0.8f, 3.2f, GLUT_BITMAP_HELVETICA_18, "Squirrel Dangerous Zone!");
        CreateText(0.0f, 1.9f, 1.5f, GLUT_BITMAP_HELVETICA_18, "Park Scenes");
        CreateText(0.0f, 1.8f, 1.5f, GLUT_BITMAP_HELVETICA_18, "Move with W, A, S, D");
        CreateText(0.0f, 1.7f, 1.5f, GLUT_BITMAP_HELVETICA_18, "Rotate with the mouse");
        CreateText(0.0f, 1.6f, 1.5f, GLUT_BITMAP_HELVETICA_18, "Press L to switch between day and night");
        CreateText(0.0f, 1.5f, 1.5f, GLUT_BITMAP_HELVETICA_18, "Press H to toggle the help menu");
    }

    glutSwapBuffers();
}

// 윈도우 크기 변경 콜백 함수
void MyReshape(int w, int h) {
    // 뷰포트, 투영 행렬 설정
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);

    // 마우스 위치 중앙으로 초기화
    lastX = w / 2;
    lastY = h / 2;
    glutWarpPointer(lastX, lastY);
}

// 이동, 도움말, 종료 - 키보드 입력 콜백 함수
void MyKeyboard(unsigned char key, int x, int y) {

    switch (key)
    {
        // 종료
        case 'q': case'Q': case '\033': 
            exit(0);
            break;

        // 도움말 토글
        case 'h': 
            help = !help;
            break;

        // 아침, 저녁 바꾸기
        case 'l':
            morning = !morning;
            initLight();
            break;
        
        // 이동
        case 'w':
            W = true;
            break;

        case 's':
            S = true;
            break;

        case 'a':
            A = true;
            break;

        case 'd':
            D = true;
            break;
    }

}

// 이동 멈추기 - 이동상태면 true고 idle이벤트에서 이동위치 계산 후 적용
void MyKeyboardUp(unsigned char key, int x, int y) {
    switch (key)
    {
        case 'w':
            W = false;
            break;

        case 's':
            S = false;
            break;

        case 'a':
            A = false;
            break;

        case 'd':
            D = false;
            break;
    }

}

// 카메라 업데이트 함수
void MyCamera() {

    // 카메라 앞 뒤 방향
    float lookX = sin(cameraYaw) * cos(cameraPitch);
    float lookZ = -cos(cameraYaw) * cos(cameraPitch);

    // 앞 뒤 이동
    if (W) {
        cameraX += lookX * moveSpeed; 
        cameraZ += lookZ * moveSpeed;
    }
    if (S) {
        cameraX -= lookX * moveSpeed;
        cameraZ -= lookZ * moveSpeed;
    }

    // 좌 우 이동 - 카메라 기준 90도 방향으로 이동
    if (A) {
        cameraX -= sin(cameraYaw + PI / 2) * moveSpeed; 
        cameraZ += cos(cameraYaw + PI / 2) * moveSpeed;
    }
    if (D) {
        cameraX += sin(cameraYaw + PI / 2) * moveSpeed;
        cameraZ -= cos(cameraYaw + PI / 2) * moveSpeed;
    }

    // 바닥 못나가게
    cameraX = std::max(-10.0f, std::min(cameraX, 10.0f));
    cameraZ = std::max(-10.0f, std::min(cameraZ, 10.0f));

    glutPostRedisplay();
}

// 마우스 움직임 콜백 함수
void MyMouseMove(int x, int y) {

    // 처음 움직일 때 마우스 위치 저장
    if (firstMoveMouse) {
        lastX = x;
        lastY = y;
        firstMoveMouse = false;
    }

    // 마우스 이동 계산
    float xoffset = x - (float)lastX;
    float yoffset = (float)lastY - y;  // 역순으로 계산 (y 좌표는 위로 갈수록 작아짐)

    lastX = x;
    lastY = y;

    // 마우스 속도에 맞게 위치 설정
    xoffset *= mouseSpeed;
    yoffset *= mouseSpeed;

    cameraYaw += xoffset;   // 수평 
    cameraPitch += yoffset; // 수직 
}

void idle() {
    MyCamera();
}

int main(int argc, char** argv) {
    
    // GLUT 초기화
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(window_width, window_height);
    glutCreateWindow("컴퓨터 그래픽스 기말 과제 프로젝트 1) 공원 걷기");

    // 마우스 숨기기
    glutSetCursor(GLUT_CURSOR_NONE);

    // OpenGL 초기화
    initLight();
    initTree();
    initGround();
    initSignboard();

    // 콜백 함수 등록
    glutDisplayFunc(MyDisplay);
    glutReshapeFunc(MyReshape); 
    glutKeyboardFunc(MyKeyboard); // 키보드 입력 시
    glutKeyboardUpFunc(MyKeyboardUp); // 키보드 땠을 때
    glutPassiveMotionFunc(MyMouseMove);  // 마우스
    glutIdleFunc(idle);

    glutMainLoop();

}