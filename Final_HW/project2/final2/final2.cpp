﻿#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>

#include <algorithm>
#include <cmath>
#include <iostream>

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>

#include <filesystem>


// 텍스처 파일 불러오기
typedef struct _AUX_RGBImageRec {
    GLint sizeX, sizeY;
    unsigned char* data;
} AUX_RGBImageRec;

unsigned int MyTextureObject[6];
AUX_RGBImageRec* pTextureImage[6];

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

// 텍스처 로드 함수
int LoadGLTextures(const std::string& directory) {
    int textureIndex = 0;

    // 디렉토리 내 BMP 파일 탐색
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.path().extension() == ".bmp" && textureIndex < 15) {
            std::string filepath = entry.path().string();

            // 텍스처 이미지 로드
            AUX_RGBImageRec* texture = LoadBMP(filepath.c_str());
            if (texture) {
                glGenTextures(1, &MyTextureObject[textureIndex]);
                glBindTexture(GL_TEXTURE_2D, MyTextureObject[textureIndex]);
                glTexImage2D(GL_TEXTURE_2D, 0, 3, texture->sizeX, texture->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, texture->data);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                // 텍스처 메모리 해제
                delete[] texture->data;
                delete texture;

                textureIndex++;
            }
        }
    }

    return textureIndex; // 성공적으로 로드된 텍스처 개수 반환
}



// 윈도우 크기
const int window_width = 800;
const int window_height = 600;

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
const float moveSpeed = 0.1f;

// 파이
const float PI = 3.14;

//********************************************************************************************************************

void CreateText(float x, float y, float z, void* font, const std::string& text);

GLfloat defaultMaterial[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // 기본 값

// 시간 관련 변수
float hour = 0.0f;  // 시침
float minute = 0.0f;  // 분침
float second = 0.0f;  // 초침

// 시계 본체
void CreateClock() {

    // 재질 설정
    GLfloat backMaterial[] = { 1.0f, 1.0f, 0.6f, 1.0f };  // 옆뒷면
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, defaultMaterial);

    // 텍스처 활성화
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, MyTextureObject[0]);

    // 시계 본체
    glTranslatef(0.0f, 0.0f, -1.0f);  // 위치 조정

    // 앞면 
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.3f, 0.0f, 0.15f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.3f, 0.0f, 0.15f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.3f, 1.0f, 0.15f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.3f, 1.0f, 0.15f);
    glEnd();


    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, backMaterial); // 옆 뒤 재질 변경
    glBindTexture(GL_TEXTURE_2D, MyTextureObject[4]);
    glBegin(GL_QUADS);
    // 뒷면 
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.3f, 0.0f, -0.15f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.3f, 0.0f, -0.15f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.3f, 1.0f, -0.15f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.3f, 1.0f, -0.15f);

    // 왼쪽 면 
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.3f, 0.0f, -0.15f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.3f, 0.0f, 0.15f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.3f, 1.0f, 0.15f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.3f, 1.0f, -0.15f);

    // 오른쪽 면 
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.3f, 0.0f, -0.15f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.3f, 0.0f, 0.15f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.3f, 1.0f, 0.15f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.3f, 1.0f, -0.15f);

    // 윗면 
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.3f, 1.0f, 0.15f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.3f, 1.0f, 0.15f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.3f, 1.0f, -0.15f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.3f, 1.0f, -0.15f);

    // 아랫면 
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.3f, 0.0f, 0.15f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.3f, 0.0f, 0.15f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.3f, 0.0f, -0.15f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.3f, 0.0f, -0.15f);
    glEnd();
}

// 시계 바늘
void CreateClockHands() {
    // 초침, 분침, 시침의 길이 설정
    GLfloat secondMaterial[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    GLfloat minuteMaterial[] = { 0.0f, 1.0f, 0.0f, 1.0f };
    GLfloat hourMaterial[] = { 0.0f, 0.0f, 1.0f, 1.0f };

    // 초침
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, secondMaterial);
    glPushMatrix();
    glRotatef(second * 6.0f, 0.0f, 0.0f, 1.0f);  // 초침은 6도씩 회전

    glLineWidth(3.0f);  // 초침의 두께를 3으로 설정
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.25f, 0.0f);  // 초침 끝
    glEnd();
    glPopMatrix();

    // 분침
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, minuteMaterial);
    glPushMatrix();
    glRotatef(minute * 6.0f, 0.0f, 0.0f, 1.0f);  // 분침은 6도씩 회전

    glLineWidth(5.0f);  // 분침의 두께를 5로 설정
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.2f, 0.0f);  // 분침 끝
    glEnd();
    glPopMatrix();

    // 시침
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, hourMaterial);
    glPushMatrix();
    glRotatef(hour * 30.0f, 0.0f, 0.0f, 1.0f);  // 시침은 30도씩 회전

    glLineWidth(7.0f);  // 시침의 두께를 7로 설정
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.15f, 0.0f);  // 시침 끝
    glEnd();
    glPopMatrix();

    second += 1.0f;  // 시간 상 빠르게 회전하기 위해 보정

    // 초침이 60초가 되면
    if (second >= 60.0f) {
        second = 0.0f;
        minute += 1.0f;  // 분침 한 번 회전

        // 분침이 60분이 되면
        if (minute >= 60.0f) {
            minute = 0.0f;
            hour += 1.0f;  // 시침 한 번 회전

            if (hour >= 12.0f) {
                hour = 0.0f;
            }
        }
    }
}

// 벽
void CreateWall() {

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, defaultMaterial);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, MyTextureObject[3]);

    glBegin(GL_QUADS);  // 사각형 면을 그리기 위한 GL_QUADS 사용

    // 벽의 앞면
    glNormal3f(0, 1, 0);  // 법선 벡터 설정 (위쪽 방향)
    glTexCoord2f(0.0f, 1.0f);  glVertex3f(-15, 0, -15);  // 왼쪽 하단
    glTexCoord2f(1.0f, 1.0f);  glVertex3f(15, 0, -15);   // 오른쪽 하단
    glTexCoord2f(1.0f, 0.0f);  glVertex3f(15, 0, 15);    // 오른쪽 상단
    glTexCoord2f(0.0f, 0.0f);  glVertex3f(-15, 0, 15);   // 왼쪽 상단

    glEnd();
    glDisable(GL_TEXTURE_2D);
}


void Skybox() {
    GLfloat TEX_SIZE = 3;

    glEnable(GL_TEXTURE_2D);
    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
    glDisable(GL_DEPTH_TEST);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glColor3f(0, 0, 0);

    glBindTexture(GL_TEXTURE_2D, MyTextureObject[1]);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-TEX_SIZE, -TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(1.0, 0.0); glVertex3f(TEX_SIZE, -TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(1.0, 1.0); glVertex3f(TEX_SIZE, TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(0.0, 1.0); glVertex3f(-TEX_SIZE, TEX_SIZE, -TEX_SIZE);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, MyTextureObject[1]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(TEX_SIZE, -TEX_SIZE, TEX_SIZE);
    glTexCoord2f(1.0, 0.0); glVertex3f(-TEX_SIZE, -TEX_SIZE, TEX_SIZE);
    glTexCoord2f(1.0, 1.0); glVertex3f(-TEX_SIZE, TEX_SIZE, TEX_SIZE);
    glTexCoord2f(0.0, 1.0); glVertex3f(TEX_SIZE, TEX_SIZE, TEX_SIZE);
    glEnd();

    // 좌
    glBindTexture(GL_TEXTURE_2D, MyTextureObject[1]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(TEX_SIZE, -TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(1.0, 0.0); glVertex3f(TEX_SIZE, -TEX_SIZE, TEX_SIZE);
    glTexCoord2f(1.0, 1.0); glVertex3f(TEX_SIZE, TEX_SIZE, TEX_SIZE);
    glTexCoord2f(0.0, 1.0); glVertex3f(TEX_SIZE, TEX_SIZE, -TEX_SIZE);
    glEnd();

    // 우
    glBindTexture(GL_TEXTURE_2D, MyTextureObject[1]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-TEX_SIZE, -TEX_SIZE, TEX_SIZE);
    glTexCoord2f(1.0, 0.0); glVertex3f(-TEX_SIZE, -TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(1.0, 1.0); glVertex3f(-TEX_SIZE, TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(0.0, 1.0); glVertex3f(-TEX_SIZE, TEX_SIZE, TEX_SIZE);
    glEnd();

    // 위
    glBindTexture(GL_TEXTURE_2D, MyTextureObject[1]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-TEX_SIZE, TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(1.0, 0.0); glVertex3f(TEX_SIZE, TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(1.0, 1.0); glVertex3f(TEX_SIZE, TEX_SIZE, TEX_SIZE);
    glTexCoord2f(0.0, 1.0); glVertex3f(-TEX_SIZE, TEX_SIZE, TEX_SIZE);
    glEnd();

    // 아래
    glBindTexture(GL_TEXTURE_2D, MyTextureObject[1]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-TEX_SIZE, -TEX_SIZE, TEX_SIZE);
    glTexCoord2f(1.0, 0.0); glVertex3f(TEX_SIZE, -TEX_SIZE, TEX_SIZE);
    glTexCoord2f(1.0, 1.0); glVertex3f(TEX_SIZE, -TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(0.0, 1.0); glVertex3f(-TEX_SIZE, -TEX_SIZE, -TEX_SIZE);
    glEnd();

    glDepthMask(GL_TRUE);

    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);  // 텍스처 환경 초기화
}



void CreateStreetLamp() {

    // 텍스처 활성화
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, defaultMaterial);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, MyTextureObject[5]);
    GLUquadric* quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);  // 원기둥 텍스처 매핑

    glPushMatrix();
    glTranslatef(0, 0, 0); // 원점에서 시작
    glRotatef(-90, 1, 0, 0); // X축 기준 90도 회전
    gluCylinder(quad, 0.1, 0.1, 5.0, 32, 32); // 밑면 반지름 0.5, 윗면 반지름 0.5, 높이 5
    glPopMatrix();
    gluDeleteQuadric(quad);

    glPushMatrix();
    glTranslatef(1, 5, 0); // 기둥 위쪽 끝으로 이동
    glRotatef(90, 1, 0, 0); // X축 기준 90도 회전

    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glVertex3f(-1, -0.1, 0); // 직사각형 한 면의 네 점 정의
    glVertex3f(1, -0.1, 0);
    glVertex3f(1, 0.1, 0);
    glVertex3f(-1, 0.1, 0);
    glEnd();

    glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}


// 디스플레이
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


    // 가로등
    for (int i = 0; i < 5; ++i) {
        glPushMatrix();
        glTranslatef(-3, 0, i * 3); // X, Z 위치로 이동
        //glRotatef(-95, 0, 1, 0);
        CreateStreetLamp();
        glPopMatrix();
    }

    // 가로등
    for (int i = 0; i < 5; ++i) {
        glPushMatrix();
        glTranslatef(3, 0, i * 3); // X, Z 위치로 이동
        glRotatef(180, 0, 1, 0);
        CreateStreetLamp();
        glPopMatrix();
    }

    // 시계 렌더링
    glPushMatrix();
    glTranslatef(0, 0, 0); // X, Z 위치로 이동
    CreateClock();

    glTranslatef(0, 0.75, 0.155); // X, Z 위치로 이동
    glRotatef(180, 0, 1, 0);  // Y축 기준으로 90도 회전
    CreateClockHands();
    glPopMatrix();

    // 벽 렌더링
    glPushMatrix();
    glTranslatef(0, 0, 15);
    glRotatef(90, 1, 0, 0);
    CreateWall();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0, -15);
    glRotatef(90, 1, 0, 0);
    CreateWall();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(15, 0, 0);
    glRotatef(90, 1, 0, 0);
    glRotatef(90, 0, 0, 1);
    CreateWall();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-15, 0, 0);
    glRotatef(90, 1, 0, 0);
    glRotatef(90, 0, 0, 1);
    CreateWall();
    glPopMatrix();


    // 바닥 렌더링
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, defaultMaterial);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, MyTextureObject[2]);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glTexCoord2f(0.0f, 1.0f);  glVertex3f(-15, 0, -15);
    glTexCoord2f(1.0f, 1.0f);  glVertex3f(15, 0, -15);
    glTexCoord2f(1.0f, 0.0f);  glVertex3f(15, 0, 15);
    glTexCoord2f(0.0f, 0.0f);  glVertex3f(-15, 0, 15);
    glEnd();
    glDisable(GL_TEXTURE_2D);


    // 도움말 출력
    if (help) {
        GLfloat signboardMaterial[] = { 1.0f, 0.0f, 0.07f, 1.0f };
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, signboardMaterial);
        CreateText(0.0f, 1.5f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Clock");
        CreateText(0.0f, 1.4f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Move with W, A, S, D");
        CreateText(0.0f, 1.3f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Rotate with the mouse");
    }



    glutSwapBuffers();
}

// OpenGL 초기화 함수
void initLight() {
    GLfloat pointLightPos[] = { 0.0f, 6.0f, 0.0f, 1.0f }; // 점광원 
    GLfloat ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat diffuseLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glClearColor(0.7f, 0.9f, 1.0f, 1.0f);

    glLightfv(GL_LIGHT0, GL_POSITION, pointLightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);

}

// 화면에 텍스트 출력
void CreateText(float x, float y, float z, void* font, const std::string& text) {
    glRasterPos3f(x, y, z);
    for (char c : text) {
        glutBitmapCharacter(font, c);
    }
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
    case 'h': case'H':
        help = !help;
        break;


        // 이동
    case 'w': case'W':
        W = true;
        break;

    case 's': case'S':
        S = true;
        break;

    case 'a': case'A':
        A = true;
        break;

    case 'd': case'D':
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
    cameraX = std::max(-15.0f, std::min(cameraX, 15.0f));
    cameraZ = std::max(-15.0f, std::min(cameraZ, 15.0f));

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

// 콜백 함수: 타이머
void timer(int value) {
    MyCamera();
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);    // 16ms마다 timer 호출
}


int main(int argc, char** argv) {


    // GLUT 초기화
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(window_width, window_height);
    glutCreateWindow("컴퓨터 그래픽스 기말 과제 프로젝트 2) 시계");

    // 마우스 숨기기
    glutSetCursor(GLUT_CURSOR_NONE);


    /*
        실행 파일이 있는 경로로 텍스처 로드
        기존에 명령 인수에 넣고 로드하였으나, exe파일로 실행 시 명령 인수가 실행이 안됨 (argc값이 안나옴)
        현재 폴더에 있는 bmp파일을 불러와서 텍스처로 사용
    */
    std::string exeDirectory = std::filesystem::current_path().string();
    int loadedTextures = LoadGLTextures(exeDirectory);

    // OpenGL 초기화
    initLight();

    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);



    // 콜백 함수 등록
    glutDisplayFunc(MyDisplay);
    glutReshapeFunc(MyReshape);
    glutKeyboardFunc(MyKeyboard); // 키보드 입력 시
    glutKeyboardUpFunc(MyKeyboardUp); // 키보드 땠을 때
    glutPassiveMotionFunc(MyMouseMove);  // 마우스
    //glutIdleFunc(idle);
    glutTimerFunc(16, timer, 0); // 16ms 간격으로 timer 호출

    glutMainLoop();



}