#include <GL/glut.h>
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
AUX_RGBImageRec* pTextureImage[5];  //�ؽ��� ���� ������ ����Ű�� ������

// BMP������ �о� AUX_RGBImageRec ����ü�� �����ϴ� LoadBMP �Լ�
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

int LoadGLTextures(char* szFilePath, int i) {  // ���� �ε� �ؽ��ķ� ��ȯ
    int Status = FALSE;

    glClearColor(0.0, 0.0, 0.0, 0.5);
    memset(pTextureImage, 0, sizeof(void*) * 10); // NULL�� �ʱ�ȭ

    // �ؽ��� �̹��� �ε�
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

    // �޸� ����
    if (pTextureImage[i]) {
        if (pTextureImage[i]->data) {
            free(pTextureImage[i]->data);
        }
        free(pTextureImage[i]);
    }

    return Status;
}

// ������ ũ��
const int window_width = 1920;
const int window_height = 1080;

// ���� ���
bool help = true;

// ī�޶� ��ġ, ����
float cameraX = 0.0f;
float cameraY = 1.0f;
float cameraZ = 5.0f;
float cameraYaw = 0.0f;    // ���� ȸ��
float cameraPitch = 0.0f;  // ���� ȸ��

// ���콺 ���� ����
int lastX = window_width / 2;  // ������ �߾��� 0����
int lastY = window_height / 2;

bool firstMoveMouse = true; // ó�� ������ �� lastX, lastY�� �����ϱ� ����

const float mouseSpeed = 0.005f; //   ���콺 �ӵ�

// Ű ���� - Ű�� ��� �迭�� false�� �ʱ�ȭ -> W, A, S, D Ű�� ���ؼ� true�� ����
bool keys[256] = { false };
bool W = false, A = false, S = false, D = false;

// �̵� ���� ����
const float moveSpeed = 0.001f;

// ����
const float PI = 3.14;

//********************************************************************************************************************

void CreateText(float x, float y, float z, void* font, const std::string& text);

GLfloat defaultMaterial[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // �⺻ ��

// �ð� ���� ����
float hour = 0.0f;  // ��ħ
float minute = 0.0f;  // ��ħ
float second = 0.0f;  // ��ħ

// �ð� ��ü
void CreateClock() {

    // �ո�
    GLfloat frontMaterial[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // �ո�
    GLfloat backMaterial[] = { 1.0f, 1.0f, 0.6f, 1.0f };  // ���޸�

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, frontMaterial);

    // �ؽ�ó Ȱ��ȭ�� �԰��� ȭ���� �׸��� ������
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, MyTextureObject[0]);

    // �ð� �׵θ� �׸��� (�׸� �ð�)
    glTranslatef(0.0f, 0.0f, -1.0f);  // ��ġ ����
    // ���簢�� �ð� (����, ���� ���� ���� �� 6��ü) 
    glBegin(GL_QUADS);
        // �ո� 
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.3f, 0.0f, 0.15f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(0.3f, 0.0f, 0.15f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.3f, 1.0f, 0.15f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.3f, 1.0f, 0.15f);
    glEnd();


    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, backMaterial); // �� �� ���� ����
    glBindTexture(GL_TEXTURE_2D, MyTextureObject[3]);
    glBegin(GL_QUADS);
        // �޸� 
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.3f, 0.0f, -0.15f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(0.3f, 0.0f, -0.15f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.3f, 1.0f, -0.15f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.3f, 1.0f, -0.15f);

        // ���� �� 
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.3f, 0.0f, -0.15f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.3f, 0.0f, 0.15f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.3f, 1.0f, 0.15f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.3f, 1.0f, -0.15f);

        // ������ �� 
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.3f, 0.0f, -0.15f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(0.3f, 0.0f, 0.15f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.3f, 1.0f, 0.15f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.3f, 1.0f, -0.15f);

        // ���� 
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.3f, 1.0f, 0.15f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(0.3f, 1.0f, 0.15f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.3f, 1.0f, -0.15f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.3f, 1.0f, -0.15f);

        // �Ʒ��� 
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.3f, 0.0f, 0.15f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(0.3f, 0.0f, 0.15f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.3f, 0.0f, -0.15f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.3f, 0.0f, -0.15f);
    glEnd();
}

// �ð� �ٴ�
void CreateClockHands() {
    // ��ħ, ��ħ, ��ħ�� ���� ����
    float secondLength = 0.25f;
    float minuteLength = 0.2f;
    float hourLength = 0.15f;

    GLfloat secondMaterial[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    GLfloat minuteMaterial[] = { 0.0f, 1.0f, 0.0f, 1.0f };
    GLfloat hourMaterial[] = { 0.0f, 0.0f, 1.0f, 1.0f };
    

    // ��ħ
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, secondMaterial);
    glPushMatrix();
    glRotatef(second * 6.0f, 0.0f, 0.0f, 1.0f);  // ��ħ�� 6���� ȸ��

    glLineWidth(3.0f);  // ��ħ�� �β��� 3���� ����
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, secondLength, 0.0f);  // ��ħ ��
    glEnd();
    glPopMatrix();

    // ��ħ
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, minuteMaterial);
    glPushMatrix();
    glRotatef(minute * 6.0f, 0.0f, 0.0f, 1.0f);  // ��ħ�� 6���� ȸ��

    glLineWidth(5.0f);  // ��ħ�� �β��� 5�� ����
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, minuteLength, 0.0f);  // ��ħ ��
    glEnd();
    glPopMatrix();

    // ��ħ
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, hourMaterial);
    glPushMatrix();
    glRotatef(hour * 30.0f, 0.0f, 0.0f, 1.0f);  // ��ħ�� 30���� ȸ��

    glLineWidth(7.0f);  // ��ħ�� �β��� 7�� ����
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, hourLength, 0.0f);  // ��ħ ��
    glEnd();
    glPopMatrix();


    
    second += 0.01f;  // ��ħ 1�ʴ� 1����

    // ��ħ�� 60�ʰ� �Ǹ�
    if (second >= 60.0f) { 
        second = 0.0f;
        minute += 1.0f;  // ��ħ �� �� ȸ��

        // ��ħ�� 60���� �Ǹ�
        if (minute >= 60.0f) { 
            minute = 0.0f;
            hour += 1.0f;  // ��ħ �� �� ȸ��

            if (hour >= 12.0f) {
                hour = 0.0f;
            }
        }
    }
}

// ��
void CreateWall() {

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, defaultMaterial);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, MyTextureObject[1]);

    glBegin(GL_QUADS);  // �簢�� ���� �׸��� ���� GL_QUADS ���

    // ���� �ո�
    glNormal3f(0, 1, 0);  // ���� ���� ���� (���� ����)
    glTexCoord2f(0.0f, 1.0f);  glVertex3f(-15, 0, -15);  // ���� �ϴ�
    glTexCoord2f(1.0f, 1.0f);  glVertex3f(15, 0, -15);   // ������ �ϴ�
    glTexCoord2f(1.0f, 0.0f);  glVertex3f(15, 0, 15);    // ������ ���
    glTexCoord2f(0.0f, 0.0f);  glVertex3f(-15, 0, 15);   // ���� ���

    // ���� �޸�
    glNormal3f(0, -1, 0);  // ���� ���� ���� (�Ʒ��� ����)
    glTexCoord2f(0.0f, 1.0f);  glVertex3f(-15, 1, -15);  // ���� ���
    glTexCoord2f(1.0f, 1.0f);  glVertex3f(15, 1, -15);   // ������ ���
    glTexCoord2f(1.0f, 0.0f);  glVertex3f(15, 1, 15);    // ������ �ϴ�
    glTexCoord2f(0.0f, 0.0f);  glVertex3f(-15, 1, 15);   // ���� �ϴ�

    // ���� ���� ��
    glNormal3f(-1, 0, 0);  // ���� ���� ���� (���� ����)
    glTexCoord2f(0.0f, 1.0f);  glVertex3f(-15, 0, -15);   // �ո� ���� �ϴ�
    glTexCoord2f(1.0f, 1.0f);  glVertex3f(-15, 1, -15);  // �޸� ���� ���
    glTexCoord2f(1.0f, 0.0f);  glVertex3f(-15, 1, 15);   // �޸� ���� �ϴ�
    glTexCoord2f(0.0f, 0.0f);  glVertex3f(-15, 0, 15);    // �ո� ���� ���

    // ���� ������ ��
    glNormal3f(1, 0, 0);   // ���� ���� ���� (������ ����)
    glTexCoord2f(0.0f, 1.0f);  glVertex3f(15, 0, -15);   // �ո� ������ �ϴ�
    glTexCoord2f(1.0f, 1.0f);  glVertex3f(15, 1, -15);  // �޸� ������ ���
    glTexCoord2f(1.0f, 0.0f);  glVertex3f(15, 1, 15);   // �޸� ������ �ϴ�
    glTexCoord2f(0.0f, 0.0f);  glVertex3f(15, 0, 15);    // �ո� ������ ���

    // ���� ����
    glNormal3f(0, 0, -1);   // ���� ���� ���� (���� ����)
    glTexCoord2f(0.0f, 1.0f);  glVertex3f(-15, 1, -15);   // ���� ���
    glTexCoord2f(1.0f, 1.0f);  glVertex3f(15, 1, -15);    // ������ ���
    glTexCoord2f(1.0f, 0.0f);  glVertex3f(15, 1, 15);     // ������ �ϴ�
    glTexCoord2f(0.0f, 0.0f);  glVertex3f(-15, 1, 15);    // ���� �ϴ�

    // ���� �Ʒ���
    glNormal3f(0, 0, 1);    // ���� ���� ���� (���� ����)
    glTexCoord2f(0.0f, 1.0f);  glVertex3f(-15, 0, -15);    // ���� �ϴ�
    glTexCoord2f(1.0f, 1.0f);  glVertex3f(15, 0, -15);     // ������ �ϴ�
    glTexCoord2f(1.0f, 0.0f);  glVertex3f(15, 0, 15);      // ������ ���
    glTexCoord2f(0.0f, 0.0f);  glVertex3f(-15, 0, 15);     // ���� ���

    glEnd();
    glDisable(GL_TEXTURE_2D);
}


void Skybox() {
    int TEX_SIZE = 3;

    glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
    glDisable(GL_DEPTH_TEST);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glColor3f(0, 0, 0);
    
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

    // ��
    glBindTexture(GL_TEXTURE_2D, MyTextureObject[4]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(TEX_SIZE, -TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(1.0, 0.0); glVertex3f(TEX_SIZE, -TEX_SIZE, TEX_SIZE);
    glTexCoord2f(1.0, 1.0); glVertex3f(TEX_SIZE, TEX_SIZE, TEX_SIZE);
    glTexCoord2f(0.0, 1.0); glVertex3f(TEX_SIZE, TEX_SIZE, -TEX_SIZE);
    glEnd();

    // ��
    glBindTexture(GL_TEXTURE_2D, MyTextureObject[4]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-TEX_SIZE, -TEX_SIZE, TEX_SIZE);
    glTexCoord2f(1.0, 0.0); glVertex3f(-TEX_SIZE, -TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(1.0, 1.0); glVertex3f(-TEX_SIZE, TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(0.0, 1.0); glVertex3f(-TEX_SIZE, TEX_SIZE, TEX_SIZE);
    glEnd();

    // ��
    glBindTexture(GL_TEXTURE_2D, MyTextureObject[4]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-TEX_SIZE, TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(1.0, 0.0); glVertex3f(TEX_SIZE, TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(1.0, 1.0); glVertex3f(TEX_SIZE, TEX_SIZE, TEX_SIZE);
    glTexCoord2f(0.0, 1.0); glVertex3f(-TEX_SIZE, TEX_SIZE, TEX_SIZE);
    glEnd();

    // �Ʒ�
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
    glDisable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);  // �ؽ�ó ȯ�� �ʱ�ȭ
}


// ���÷���
void MyDisplay() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    Skybox();

    // ī�޶� ���� ���� ���
    float lookX = sin(cameraYaw) * cos(cameraPitch);
    float lookY = sin(cameraPitch);
    float lookZ = -cos(cameraYaw) * cos(cameraPitch);

    gluLookAt(
        cameraX, cameraY, cameraZ,                          // ī�޶� ��ġ
        cameraX + lookX, cameraY + lookY, cameraZ + lookZ,  // �ٶ󺸴� ����
        0.0, 1.0, 0.0                                       // ���� ����
    );


    // �ð� ������
    glPushMatrix();
        glTranslatef(0, 0, 0); // X, Z ��ġ�� �̵�
        CreateClock();

        glTranslatef(0, 0.75, 0.155); // X, Z ��ġ�� �̵�
        glRotatef(180, 0, 1, 0);  // Y�� �������� 90�� ȸ��
        CreateClockHands();
    glPopMatrix();

    // �� ������
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


    // �ٴ� ������
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


    // ���� ���
    if (help) {
        GLfloat signboardMaterial[] = { 1.0f, 0.0f, 0.07f, 1.0f };
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, signboardMaterial);
        CreateText(0.0f, 1.5f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Park Scenes");
        CreateText(0.0f, 1.4f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Move with W, A, S, D");
        CreateText(0.0f, 1.3f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Rotate with the mouse");
    }



    glutSwapBuffers();
}

// OpenGL �ʱ�ȭ �Լ�
void initLight() {
    GLfloat pointLightPos[] = { 0.0f, 6.0f, 0.0f, 1.0f }; // ������ 
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

// ȭ�鿡 �ؽ�Ʈ ���
void CreateText(float x, float y, float z, void* font, const std::string& text) {
    glRasterPos3f(x, y, z);
    for (char c : text) {
        glutBitmapCharacter(font, c);
    }
}

// ������ ũ�� ���� �ݹ� �Լ�
void MyReshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // ���콺 ��ġ �߾����� �ʱ�ȭ
    lastX = w / 2;
    lastY = h / 2;
    glutWarpPointer(lastX, lastY);
}

// �̵�, ����, ���� - Ű���� �Է� �ݹ� �Լ�
void MyKeyboard(unsigned char key, int x, int y) {

    switch (key)
    {
        // ����
    case 'q': case'Q': case '\033':
        exit(0);
        break;

        // ���� ���
    case 'h': case'H':
        help = !help;
        break;


        // �̵�
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

// �̵� ���߱� - �̵����¸� true�� idle�̺�Ʈ���� �̵���ġ ��� �� ����
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

// ī�޶� ������Ʈ �Լ�
void MyCamera() {

    // ī�޶� �� �� ����
    float lookX = sin(cameraYaw) * cos(cameraPitch);
    float lookZ = -cos(cameraYaw) * cos(cameraPitch);

    // �� �� �̵�
    if (W) {
        cameraX += lookX * moveSpeed;
        cameraZ += lookZ * moveSpeed;
    }
    if (S) {
        cameraX -= lookX * moveSpeed;
        cameraZ -= lookZ * moveSpeed;
    }

    // �� �� �̵� - ī�޶� ���� 90�� �������� �̵�
    if (A) {
        cameraX -= sin(cameraYaw + PI / 2) * moveSpeed;
        cameraZ += cos(cameraYaw + PI / 2) * moveSpeed;
    }
    if (D) {
        cameraX += sin(cameraYaw + PI / 2) * moveSpeed;
        cameraZ -= cos(cameraYaw + PI / 2) * moveSpeed;
    }

    // �ٴ� ��������
    cameraX = std::max(-15.0f, std::min(cameraX, 15.0f));
    cameraZ = std::max(-15.0f, std::min(cameraZ, 15.0f));

    glutPostRedisplay();
}

// ���콺 ������ �ݹ� �Լ�
void MyMouseMove(int x, int y) {

    // ó�� ������ �� ���콺 ��ġ ����
    if (firstMoveMouse) {
        lastX = x;
        lastY = y;
        firstMoveMouse = false;
    }

    // ���콺 �̵� ���
    float xoffset = x - (float)lastX;
    float yoffset = (float)lastY - y;  // �������� ��� (y ��ǥ�� ���� ������ �۾���)

    lastX = x;
    lastY = y;

    // ���콺 �ӵ��� �°� ��ġ ����
    xoffset *= mouseSpeed;
    yoffset *= mouseSpeed;

    cameraYaw += xoffset;   // ���� 
    cameraPitch += yoffset; // ���� 
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
        // GLUT �ʱ�ȭ
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
        glutInitWindowSize(window_width, window_height);
        glutCreateWindow("��ǻ�� �׷��Ƚ� �⸻ ���� ������Ʈ 2) �ð�");

        // ���콺 �����
        glutSetCursor(GLUT_CURSOR_NONE);

        // �� BMP ������ �ؽ�ó�� �ε�
        for (int i = 1; i < argc; i++) {
            if (LoadGLTextures(argv[i], i - 1)) {
                glEnable(GL_TEXTURE_2D);
            }
            else {
                printf("Texture loading failed for %s\n", argv[i]);
                exit(1);
            }
        }

        // OpenGL �ʱ�ȭ
        initLight();

        glEnable(GL_TEXTURE_2D);
        glShadeModel(GL_SMOOTH);
        glClearDepth(1.0);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);



        // �ݹ� �Լ� ���
        glutDisplayFunc(MyDisplay);
        glutReshapeFunc(MyReshape);
        glutKeyboardFunc(MyKeyboard); // Ű���� �Է� ��
        glutKeyboardUpFunc(MyKeyboardUp); // Ű���� ���� ��
        glutPassiveMotionFunc(MyMouseMove);  // ���콺
        glutIdleFunc(idle);

        glutMainLoop();

    }
}

