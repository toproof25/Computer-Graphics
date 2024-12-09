﻿#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>

#include <algorithm>
#include <cmath>
#include <iostream>

#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>

typedef struct _AUX_RGBImageRec {
    GLint sizeX, sizeY;
    unsigned char* data;
} AUX_RGBImageRec;

unsigned int MyTextureObject[5];
AUX_RGBImageRec* pTextureImage[5];  //텍스쳐 저장 공간을 가리키는 포인터

// BMP파일을 읽어 AUX_RGBImageRec 구조체에 저장하는 LoadBMP 함수
AUX_RGBImageRec* LoadBMP(const char* filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return nullptr;
    }

    BITMAPFILEHEADER bfh;
    file.read(reinterpret_cast<char*>(&bfh), sizeof(BITMAPFILEHEADER));

    if (bfh.bfType != 0x4D42) { // "BM"
        std::cerr << "Not a BMP file: " << filename << std::endl;
        return nullptr;
    }

    BITMAPINFOHEADER bih;
    file.read(reinterpret_cast<char*>(&bih), sizeof(BITMAPINFOHEADER));

    if (bih.biBitCount != 24) {
        std::cerr << "Only 24-bit BMPs are supported." << std::endl;
        return nullptr;
    }

    AUX_RGBImageRec* texture = new AUX_RGBImageRec;
    texture->sizeX = bih.biWidth;
    texture->sizeY = bih.biHeight;

    int imageSize = texture->sizeX * texture->sizeY * 3;
    texture->data = new unsigned char[imageSize];

    // Move file pointer to the beginning of bitmap data
    file.seekg(bfh.bfOffBits, std::ios::beg);

    // Calculate padding
    int padding = (4 - (texture->sizeX * 3) % 4) % 4;

    // Read the bitmap data
    std::vector<unsigned char> row(texture->sizeX * 3 + padding);
    for (int y = texture->sizeY - 1; y >= 0; y--) { // BMP is stored bottom-to-top
        file.read(reinterpret_cast<char*>(row.data()), row.size());
        for (int x = 0; x < texture->sizeX; x++) {
            int i = (y * texture->sizeX + x) * 3;
            texture->data[i] = row[x * 3 + 2];     // Red
            texture->data[i + 1] = row[x * 3 + 1]; // Green
            texture->data[i + 2] = row[x * 3];     // Blue
        }
    }

    file.close();
    return texture;
}

int LoadGLTextures(char* szFilePath, int i) {  // 파일 로드 텍스쳐로 변환
    int Status = FALSE;

    glClearColor(0.0, 0.0, 0.0, 0.5);
    memset(pTextureImage, 0, sizeof(void*) * 10); // NULL로 초기화

    // 텍스쳐 이미지 로드
    if (pTextureImage[i] = LoadBMP((char*)szFilePath)) {  
        Status = TRUE; 
        glGenTextures(1, &MyTextureObject[i]);  
        glBindTexture(GL_TEXTURE_2D, MyTextureObject[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, 3,
            pTextureImage[i]->sizeX, pTextureImage[i]->sizeY,
            0, GL_RGB, GL_UNSIGNED_BYTE, pTextureImage[i]->data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glEnable(GL_TEXTURE_2D);
    }

    // 메모리 해제
    if (pTextureImage[i]) {
        if (pTextureImage[i]->data) {  
            free(pTextureImage[i]->data);  
        }
        free(pTextureImage[i]); 
    }

    return Status;
}


//********************************************************************************************************************

// 나무, 바닥, 입간판
GLuint treeDisplayList;
GLuint groundDisplayList;

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
int lastX = window_width / 2;  // 윈도우 중앙을 0으로
int lastY = window_height / 2;

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
GLfloat morningLight[] = { 0.5f, 1.0f, 0.5f, 0.0f }; // 방향성을 가진 방향 광원
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

void CreatSignboard() {
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

    // 텍스처 활성화는 입간판 화면을 그리기 직전에
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, MyTextureObject[0]);

    // 입간판 화면 (세로 좁고 가로, 높이 긴 6면체) 
    glBegin(GL_QUADS);

    // 앞면 (텍스쳐)
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.6f, 0.5f, 0.1f);   
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.6f, 0.5f, 0.1f);  
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.6f, 1.0f, 0.1f);  
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.6f, 1.0f, 0.1f);  

    // 뒷면 (텍스쳐)
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.6f, 0.5f, -0.1f);  
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.6f, 0.5f, -0.1f);  
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.6f, 1.0f, -0.1f);  
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.6f, 1.0f, -0.1f);  


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
    glEndList();

    glDisable(GL_TEXTURE_2D);

}

// 디스플레이 리스트 - 나무
void initTree() {
    treeDisplayList = glGenLists(1);
    glNewList(treeDisplayList, GL_COMPILE);

    // 재질 설정
    GLfloat brownMaterial[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // 갈색 줄기
    GLfloat greenMaterial[] = { 0.0f, 0.5f, 0.0f, 1.0f };    // 초록색 나뭇잎

    // 텍스처 활성화
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, MyTextureObject[2]);

    // 줄기 (원기둥)
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, brownMaterial);
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    GLUquadric* quadric = gluNewQuadric();
    gluQuadricTexture(quadric, GL_TRUE);  // 원기둥 텍스처 매핑
    gluCylinder(quadric, 0.2, 0.2, 1.0, 20, 1);
    gluDeleteQuadric(quadric);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    // 텍스처 활성화
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, MyTextureObject[1]);

    // 윗부분 나뭇잎 (첫 번째 원뿔) 
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, greenMaterial);
    glPushMatrix();
    glTranslatef(0, 1.0, 0);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* coneQuadric1 = gluNewQuadric();
    gluQuadricTexture(coneQuadric1, GL_TRUE);  // 원뿔 텍스처 매핑 
    gluCylinder(coneQuadric1, 0.6, 0.0, 1.0, 20, 1);
    gluDeleteQuadric(coneQuadric1);
    glPopMatrix();

    // 아랫부분 나뭇잎 (두 번째 원뿔)
    glPushMatrix();
    glTranslatef(0, 1.5, 0);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* coneQuadric2 = gluNewQuadric();
    gluQuadricTexture(coneQuadric2, GL_TRUE);  // 원뿔 텍스처 매핑 
    gluCylinder(coneQuadric2, 0.8, 0.0, 1.0, 20, 1);
    gluDeleteQuadric(coneQuadric2);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

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



void Skybox() {
    int TEX_SIZE = 3;

    glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
    glDisable(GL_DEPTH_TEST);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    //glColor3f(0, 0, 0);

    glBindTexture(GL_TEXTURE_2D, MyTextureObject[4]);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-TEX_SIZE, -TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(1.0, 0.0); glVertex3f(TEX_SIZE, -TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(1.0, 1.0); glVertex3f(TEX_SIZE, TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(0.0, 1.0); glVertex3f(-TEX_SIZE, TEX_SIZE, -TEX_SIZE);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, MyTextureObject[4]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(TEX_SIZE, -TEX_SIZE, TEX_SIZE);
    glTexCoord2f(1.0, 0.0); glVertex3f(-TEX_SIZE, -TEX_SIZE, TEX_SIZE);
    glTexCoord2f(1.0, 1.0); glVertex3f(-TEX_SIZE, TEX_SIZE, TEX_SIZE);
    glTexCoord2f(0.0, 1.0); glVertex3f(TEX_SIZE, TEX_SIZE, TEX_SIZE);
    glEnd();

    // 좌
    glBindTexture(GL_TEXTURE_2D, MyTextureObject[4]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(TEX_SIZE, -TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(1.0, 0.0); glVertex3f(TEX_SIZE, -TEX_SIZE, TEX_SIZE);
    glTexCoord2f(1.0, 1.0); glVertex3f(TEX_SIZE, TEX_SIZE, TEX_SIZE);
    glTexCoord2f(0.0, 1.0); glVertex3f(TEX_SIZE, TEX_SIZE, -TEX_SIZE);
    glEnd();

    // 우
    glBindTexture(GL_TEXTURE_2D, MyTextureObject[4]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-TEX_SIZE, -TEX_SIZE, TEX_SIZE);
    glTexCoord2f(1.0, 0.0); glVertex3f(-TEX_SIZE, -TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(1.0, 1.0); glVertex3f(-TEX_SIZE, TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(0.0, 1.0); glVertex3f(-TEX_SIZE, TEX_SIZE, TEX_SIZE);
    glEnd();

    // 위
    glBindTexture(GL_TEXTURE_2D, MyTextureObject[4]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-TEX_SIZE, TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(1.0, 0.0); glVertex3f(TEX_SIZE, TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(1.0, 1.0); glVertex3f(TEX_SIZE, TEX_SIZE, TEX_SIZE);
    glTexCoord2f(0.0, 1.0); glVertex3f(-TEX_SIZE, TEX_SIZE, TEX_SIZE);
    glEnd();

    // 아래
    glBindTexture(GL_TEXTURE_2D, MyTextureObject[4]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-TEX_SIZE, -TEX_SIZE, TEX_SIZE);
    glTexCoord2f(1.0, 0.0); glVertex3f(TEX_SIZE, -TEX_SIZE, TEX_SIZE);
    glTexCoord2f(1.0, 1.0); glVertex3f(TEX_SIZE, -TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(0.0, 1.0); glVertex3f(-TEX_SIZE, -TEX_SIZE, -TEX_SIZE);
    glEnd();

    glDepthMask(GL_TRUE);

    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);  // 텍스처 환경 초기화
    glDisable(GL_TEXTURE_2D);
}


// 디스플레이 콜백 함수
void MyDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    Skybox();

    // 카메라 방향 벡터 계산
    float lookX = sin(cameraYaw) * cos(cameraPitch);
    float lookY = sin(cameraPitch);
    float lookZ = -cos(cameraYaw) * cos(cameraPitch);

    gluLookAt(
        cameraX, cameraY, cameraZ,                          // 카메라 위치
        cameraX + lookX, cameraY + lookY, cameraZ + lookZ,  // 바라보는 지점
        0.0, 1.0, 0.0                                       // 위쪽 방향
    );

    // 입간판 렌더링
    glPushMatrix();
    glTranslatef(3, 0, 3); // X, Z 위치로 이동
    CreatSignboard();
    glPopMatrix();

    // 바닥 렌더링
    //glCallList(groundDisplayList);
    // 텍스처 활성화는 입간판 화면을 그리기 직전에
    // 기존 재질
    GLfloat defaultMaterial[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // 기본 Ambient 값
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, defaultMaterial);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, MyTextureObject[1]);

    glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glTexCoord2f(0.0f, 1.0f);  glVertex3f(-15, 0, -15);
        glTexCoord2f(1.0f, 1.0f);  glVertex3f(15, 0, -15);
        glTexCoord2f(1.0f, 0.0f);  glVertex3f(15, 0, 15);
        glTexCoord2f(0.0f, 0.0f);  glVertex3f(-15, 0, 15);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // 나무 렌더링
    for (int i = 0; i < numTrees; ++i) {
        glPushMatrix();
        glTranslatef(treePositions[i][0], 0, treePositions[i][1]); // X, Z 위치로 이동
        glCallList(treeDisplayList); // 나무 렌더링
        glPopMatrix();
    }

    // 도움말 출력
    if (help) {
        GLfloat signboardMaterial[] = { 1.0f, 0.0f, 0.07f, 1.0f };
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, signboardMaterial);
        CreateText(0.0f, 1.5f, 1.5f, GLUT_BITMAP_HELVETICA_18, "Park Scenes");
        CreateText(0.0f, 1.4f, 1.5f, GLUT_BITMAP_HELVETICA_18, "Move with W, A, S, D");
        CreateText(0.0f, 1.3f, 1.5f, GLUT_BITMAP_HELVETICA_18, "Rotate with the mouse");
        CreateText(0.0f, 1.2f, 1.5f, GLUT_BITMAP_HELVETICA_18, "Press L to switch between day and night");
        CreateText(0.0f, 1.1f, 1.5f, GLUT_BITMAP_HELVETICA_18, "Press H to toggle the help menu");
    }

    glutSwapBuffers();
}

// 윈도우 크기 변경 콜백 함수
void MyReshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

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

    if (argc <= 1) {
        printf("\n%s\n\n", "Usage : TextureDLG3_Consol.exe [BMPFileName.bmp]");
        exit(1);
    }
    else {
        // GLUT 초기화
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
        glutInitWindowSize(window_width, window_height);
        glutCreateWindow("컴퓨터 그래픽스 기말 과제 프로젝트 1) 공원 걷기");

        // 마우스 숨기기
        glutSetCursor(GLUT_CURSOR_NONE);

        // 각 BMP 파일을 텍스처로 로드
        for (int i = 1; i < argc; i++) {
            if (LoadGLTextures(argv[i], i - 1)) {
                glEnable(GL_TEXTURE_2D);
            }
            else {
                printf("Texture loading failed for %s\n", argv[i]);
                exit(1);
            }
        }
        
        glEnable(GL_TEXTURE_2D);
        glShadeModel(GL_SMOOTH);
        glClearDepth(1.0);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

        // OpenGL 초기화
        initLight();
        initTree();

        // 콜백 함수 등록
        glutDisplayFunc(MyDisplay);
        glutReshapeFunc(MyReshape);
        glutKeyboardFunc(MyKeyboard); // 키보드 입력 시
        glutKeyboardUpFunc(MyKeyboardUp); // 키보드 땠을 때
        glutPassiveMotionFunc(MyMouseMove);  // 마우스
        glutIdleFunc(idle);

        glutMainLoop();
  
    }
}
