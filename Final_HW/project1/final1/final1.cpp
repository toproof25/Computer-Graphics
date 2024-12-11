#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <algorithm>
#include <cmath>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// 나무와 바닥을 위한 디스플레이 리스트
GLuint treeDisplayList;
GLuint groundDisplayList;
GLuint signboardDisplayList;

// 윈도우 크기
const int window_width = 1600;
const int window_height = 900;

// 도움말 토글
bool help = true;

// 카메라 위치 및 방향 변수
float cameraX = 0.0f;
float cameraY = 2.0f;
float cameraZ = 5.0f;
float cameraYaw = 0.0f;    // 수평 회전 (y축 기준)
float cameraPitch = 0.0f;  // 수직 회전 (x축 기준)

// 마우스 관련 변수
int lastX = window_width/2;  // 윈도우 중앙 X
int lastY = window_height/2;  // 윈도우 중앙 Y
bool firstMouse = true;
const float mouseSensitivity = 0.001f;

// 키 상태 추적 변수
bool keys[256] = { false };

// 이동 관련 변수
const float moveSpeed = 0.001f;

// 파이
const float PI = 3.1415; 


// 텍스처 로드 및 생성
unsigned int texture;
int width, height, nrChannels;
unsigned char* data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);

float vertices[] = {
    // 위치              // 컬러             // 텍스처 좌표
    0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 우측 상단
    0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 우측 하단
   -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 좌측 하단
   -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 좌측 상단
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
    glVertex3f(0.1f, 1.0f, 0.1f);
    glVertex3f(-0.1f, 1.0f, 0.1f);

    // 뒷면
    glVertex3f(-0.1f, 0.0f, -0.1f);
    glVertex3f(0.1f, 0.0f, -0.1f);
    glVertex3f(0.1f, 1.0f, -0.1f);
    glVertex3f(-0.1f, 1.0f, -0.1f);

    // 왼쪽 면
    glVertex3f(-0.1f, 0.0f, -0.1f);
    glVertex3f(-0.1f, 0.0f, 0.1f);
    glVertex3f(-0.1f, 1.0f, 0.1f);
    glVertex3f(-0.1f, 1.0f, -0.1f);

    // 오른쪽 면
    glVertex3f(0.1f, 0.0f, -0.1f);
    glVertex3f(0.1f, 0.0f, 0.1f);
    glVertex3f(0.1f, 1.0f, 0.1f);
    glVertex3f(0.1f, 1.0f, -0.1f);

    // 윗면
    glVertex3f(-0.1f, 1.0f, 0.1f);
    glVertex3f(0.1f, 1.0f, 0.1f);
    glVertex3f(0.1f, 1.0f, -0.1f);
    glVertex3f(-0.1f, 1.0f, -0.1f);

    // 아랫면
    glVertex3f(-0.1f, 0.0f, 0.1f);
    glVertex3f(0.1f, 0.0f, 0.1f);
    glVertex3f(0.1f, 0.0f, -0.1f);
    glVertex3f(-0.1f, 0.0f, -0.1f);

    glEnd();

    glBindTexture(GL_TEXTURE_2D, texture);


    // 입간판 화면 (세로 좁고 가로, 높이 긴 6면체)
    glBegin(GL_QUADS);

    // 앞면 (텍스처 적용)
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.6f, 1.0f, 0.1f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.6f, 1.0f, 0.1f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.6f, 1.5f, 0.1f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.6f, 1.5f, 0.1f);

    // 뒷면 (텍스처 적용)
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.6f, 1.0f, -0.1f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.6f, 1.0f, -0.1f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.6f, 1.5f, -0.1f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.6f, 1.5f, -0.1f);

    // 왼쪽 면
    glVertex3f(-0.6f, 1.0f, -0.1f);
    glVertex3f(-0.6f, 1.0f, 0.1f);
    glVertex3f(-0.6f, 1.5f, 0.1f);
    glVertex3f(-0.6f, 1.5f, -0.1f);

    // 오른쪽 면
    glVertex3f(0.6f, 1.0f, -0.1f);
    glVertex3f(0.6f, 1.0f, 0.1f);
    glVertex3f(0.6f, 1.5f, 0.1f);
    glVertex3f(0.6f, 1.5f, -0.1f);

    // 윗면
    glVertex3f(-0.6f, 1.5f, 0.1f);
    glVertex3f(0.6f, 1.5f, 0.1f);
    glVertex3f(0.6f, 1.5f, -0.1f);
    glVertex3f(-0.6f, 1.5f, -0.1f);

    // 아랫면
    glVertex3f(-0.6f, 1.0f, 0.1f);
    glVertex3f(0.6f, 1.0f, 0.1f);
    glVertex3f(0.6f, 1.0f, -0.1f);
    glVertex3f(-0.6f, 1.0f, -0.1f);

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
    glVertex3f(-10, 0, -10);
    glVertex3f(10, 0, -10);
    glVertex3f(10, 0, 10);
    glVertex3f(-10, 0, 10);
    glEnd();

    glEndList();
}

// OpenGL 초기화 함수
void initGL() {

    // 렌더링 설정
    glClearColor(0.7f, 0.9f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // 빛 설정
    GLfloat lightPosition[] = { 1.0f, 1.0f, 1.0f, 0.0f };
    GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    // 빛 위치 및 색 설정
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
}

// 화면에 텍스트 출력
void renderBitmapString(float x, float y, void* font, const std::string& text) {
    glRasterPos2f(x, y);
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
    glCallList(signboardDisplayList);

    // 나무 렌더링
    glPushMatrix();
    glTranslatef(5, 0, 0);
    glCallList(treeDisplayList);
    glPopMatrix();

    // 나무 렌더링
    glPushMatrix();
    glTranslatef(-5, 0, 0);
    glCallList(treeDisplayList);
    glPopMatrix();

    if (help) {
        renderBitmapString(-1.0f, 2.9f, GLUT_BITMAP_HELVETICA_18, "Park Scenes");
        renderBitmapString(-1.0f, 2.8f, GLUT_BITMAP_HELVETICA_18, "Take a walk in the trees, the squirrel's sign, and the morning sun");
    }

    glutSwapBuffers();
}

// 윈도우 크기 변경 콜백 함수
void MyReshape(int w, int h) {

    // 뷰포트 및 투영 행렬 설정
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
    keys[key] = true;

    // 도움말 토클
    if (key == 'h')
        help = !help;

    // 종료
    else if (key == 'q')
        exit(0);
}

// 이동 멈추기 - 이동상태면 true고 idle이벤트에서 이동위치 계산 후 적용
void MyKeyboardUp(unsigned char key, int x, int y) {
    keys[key] = false;
}


// 카메라 업데이트 함수
void updateCamera() {
    // 카메라 방향 벡터 계산
    float lookX = sin(cameraYaw) * cos(cameraPitch);
    float lookZ = -cos(cameraYaw) * cos(cameraPitch);

    // 이동 벡터 계산
    if (keys['w']) {
        cameraX += lookX * moveSpeed;
        cameraZ += lookZ * moveSpeed;
    }
    if (keys['s']) {
        cameraX -= lookX * moveSpeed;
        cameraZ -= lookZ * moveSpeed;
    }
    if (keys['a']) {
        cameraX -= sin(cameraYaw + PI / 2) * moveSpeed;
        cameraZ += cos(cameraYaw + PI / 2) * moveSpeed;
    }
    if (keys['d']) {
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
    if (firstMouse) {
        lastX = x;
        lastY = y;
        firstMouse = false;
    }

    float xoffset = x - lastX;
    float yoffset = lastY - y;  // 역순으로 계산 (y 좌표는 위로 갈수록 작아짐)

    lastX = x;
    lastY = y;

    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    cameraYaw += xoffset;   // 수평 회전
    cameraPitch += yoffset; // 수직 회전

    // 피치 각도 제한 (고개를 너무 들거나 숙이는 것 방지)
    if (cameraPitch > PI / 2) cameraPitch = PI / 2;
    if (cameraPitch < -PI / 2) cameraPitch = -PI / 2;
}

void idle() {
    updateCamera();
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
    initGL();
    initTree();
    initGround();
    initSignboard();

    // 콜백 함수 등록
    glutDisplayFunc(MyDisplay);
    glutReshapeFunc(MyReshape);
    glutKeyboardFunc(MyKeyboard);
    glutKeyboardUpFunc(MyKeyboardUp);
    glutPassiveMotionFunc(MyMouseMove);  // 마우스 움직임 콜백
    glutIdleFunc(idle);


    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // 텍스처 wrapping/filtering 옵션 설정(현재 바인딩된 텍스처 객체에 대해)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        //glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);


    glutMainLoop();

}