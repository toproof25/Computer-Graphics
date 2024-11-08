### 문제 3번
본문 6장의 소스 코드를 참고해 제자리에서 튀거나, 움직이면서 튀는 탄성 공(Elastic Ball)의 모습을 애니메이션으로 작성해보라. 바닥은 얇은 6면체로 모델링할 수 있다. 이 경우 공의 속 도는 중력 가속도 공식 또는 이와 유사한 공식을 활용해 가속 또는 감속시켜야 하며, 이는 단 위 시간에 공이 이동한 거리로 조정된다 이를 위해서는 이동 변환 행렬의 값을 조정해야 한다. 가능하다면 입력에 의해 시점 변환, 속도 등을 조절할수 있도록 해야 한다. W를 누르면 와이어 프레임 모델로, []를 누르면 솔리드 모델로 변환되어야 한다 공의 탄성을 감안했을 때, 원래 둥근 공이 바닥에 닿으면 좌우로 펼쳐져야 하며 이는 원구를 그리는
glut() 함수에 좌우 방향의 확대 축소를 적용할 수 있다. 또 확대 축소 행렬의 해당 요소가 시간에 따라 변하도록 파라미터화함으로써 애니메이션할 수 있다. 역으로 공이 바닥에서 튀어오르는 순간에는 공이 점차 상하로 길어지도록할수 있다.


## 변수 설정
```cpp
// gluLookAt 변수들
GLdouble eyex = 1, eyey = 1, eyez = 3;
GLdouble centerx = 0, centery = 0, centerz = 0;
GLdouble upx = 0, upy = 1, upz = 0;

// 재질 설정 변수 (조명으로 인해 재질을 바꿔줘야 물체색이 변함)
GLfloat floor_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat ball_color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat shadow_color[] = { 0.0f, 0.0f, 0.0f, 1.0f };


// 공 위치 변수
float ballX = 0.0f;            // 공 X 위치
float ballY = 1.0f;            // 공 Y 위치
float ballZ = 0.0f;            // 공 Z 위치

// 공 크기 변수
float ballRadius = 0.1f;       // 공 반지름
float scaleX = 1.0f;           // X 스케일
float scaleY = 1.0f;           // Y 스케일

// 속도, 중력, 탄성 등
float velocityY = 0.0f;        // 떨어지는 속도
float preVelocityY = 1.0f;        // 직전에 떨어진 속도
float gravity = -0.005f;      // 중력 가속도
float elasticity = 0.75f;      // 탄성
float floorHeight = -0.5f;     // 바닥 높이
float jumpVelocity = 0.1f;     // 점프 속도

bool wireframe = false;        // 와이어프레임 모드
bool isFloor = false;          // 바닥에 닿았는지
bool isShadow = true;          // 그림자 출력 여부
```


## 조명 설정 및 초기화 설정
```cpp
// 조명 설정 - 과제1 코드 참고
void InitLight() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat light_ambient[] = { 0.3, 0.3, 0.3, 1.0 };
    GLfloat light_position[] = { 3, 6, 3.0, 1.0 };

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
}

// 초기화 함수
void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    InitLight();

    // 투영 행렬
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 800.0 / 600.0, 0.1, 100.0); // 원근 투영으로 3D로 보기 위해 (시야각, 가로세로비율, 가까운 거 잘라내기 먼거 잘라내기
}
```

## 타이머, 키보드, 마우스 콜백 함수 설정
- 타이머 함수에 FallingBall을 지속적으로 호출하여 공이 중력에 의해 떨어지는 애니메이션을 구현
- 키보드는 w, s를 통해 와이어프래임 모드를 설정
- q를 누르면 그림자를 제거하고, 바닥에 공이 있을 경우 e를 누르면 점프하는 애니메이션 추가
```cpp
// 타이머 함수
void Timer(int value) {
    FallingBall();
    glutPostRedisplay();
    glutTimerFunc(Time, Timer, 0);
}

// 키보드 입력 함수
void MyKeyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'w':
        wireframe = true;
        break;
    case 's':
        wireframe = false;
        break;
    case 'q':
        isShadow = !isShadow;
        break;

    case 'e': // 점프 - 공이 바닥에 있을 경우에만 발동
        if (ballY <= floorHeight + ballRadius) velocityY = jumpVelocity;
        break;
    }

    glutPostRedisplay();
}
```
- 마우스 이동 시 시점이 변하도록 설정
- 휠을 통해 eyez값을 조절하여 줌인 줌아웃 같은 효과를 설정함
```cpp
// 마우스 이동 이벤트 좌표를 변수에 저장
void MyMouseMove(GLint X, GLint Y) {
    // 중앙이 0이 되도록 설정
    float w = Width * 0.5;
    float h = Hight * 0.5;

    eyex = -((X - w) / w);
    eyey = (Y - h) / h;
    glutPostRedisplay();
}

// 마우스 휠로 가까이, 멀리 볼 수 있도록
void MyMouseButton(int button, int state, int x, int y) {
    if (button == 3)  // 마우스 휠 업
        eyez -= 0.1f;
    else if (button == 4) // 마우스 휠 다운
        eyez += 0.1f;
    glutPostRedisplay();
}

```

## MyDisplay
- gluLookAt으로 시점 설정
- 바닥에 하양 6면체 그리기
- 바닥 위에 빨강 공 그리기
- 빨강 공에 그림자를 그리기
```cpp
// 씬 그리기 함수
void MyDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 카메라 위치 설정
    gluLookAt(
        eyex, eyey, eyez, // 위치
        centerx, centery, centerz, // 중심
        upx, upy, upz // 업
    );

    // 바닥 - 하양
    glPushMatrix();
    glTranslatef(0.0f, floorHeight - 0.05f, 0.0f);
    glScalef(2.0f, 0.1f, 2.0f);
    glColor3f(0.6f, 0.6f, 0.6f);

    // 재질 설정 - 찾아보니 조명으로 glColor이 적용 안될 수 있음
    glMaterialfv(GL_FRONT, GL_DIFFUSE, floor_color);

    glutSolidCube(1.0f);
    glPopMatrix();

    // 공 - 빨강
    glPushMatrix();
    glTranslatef(ballX, ballY, ballZ);
    glScalef(scaleX, scaleY, 1.0f);
    glColor3f(1.0f, 0.0f, 0.0f);

    // 재질 설정 - 찾아보니 조명으로 glColor이 적용 안될 수 있음
    glMaterialfv(GL_FRONT, GL_DIFFUSE, ball_color);

    if (wireframe) { glutWireSphere(ballRadius, 20, 20); } // 와이어로 그리기
    else { glutSolidSphere(ballRadius, 20, 20); } // 그냥 그리기
    glPopMatrix();

    // 그림자 - 검정
    if (isShadow) {
        glPushMatrix();
        glMaterialfv(GL_FRONT, GL_DIFFUSE, shadow_color); // 재질 설정 - 찾아보니 조명으로 glColor이 적용 안될 수 있음
        glTranslatef(ballX, floorHeight + 0.001f, ballZ);  // 그림자 위치
        glScalef(1.2f, 0.01f, 1.2f);  // 공을 납작하게
        glutSolidSphere(ballRadius, 20, 20);  // 공 생성
        glPopMatrix();
    }


    glutSwapBuffers();
}
```

## 떨어지는 공 애니메이션 구현
```cpp
// 공 업데이트 함수
void FallingBall() {
    preVelocityY = velocityY; // 이전 속도 저장
    velocityY += gravity; // 현재 속도 계산
    ballY += velocityY; // 위치 계산

    // 바닥에 닿을 때
    if (ballY - ballRadius <= floorHeight) {
        ballY = floorHeight + ballRadius;
        velocityY = -velocityY * elasticity; // 공이 다시 튀어오를 때 탄성만큼 속도 감소

        // 공이 땅에 닿으면서 계속해서 튀어오를 수 있는 힘이 있을 때
        if (isFloor == false && fabs(velocityY - preVelocityY) >= 0.005f) {
            isFloor = true;
            scaleX = 1.3f; // X 방향으로 납작해짐
            scaleY = 0.5f; // Y 방향으로 납작해짐
        }
    }

    // 공이 납작해진 후 다시 원래대로 돌아오기
    if (isFloor) {
        scaleX += (1.0f - scaleX) * 0.25f; // 원래 크기로 복원
        scaleY += (1.0f - scaleY) * 0.25f;

        // 공 크기가 복구되면 멈추기
        if (fabs(scaleX - 1.0f) <= 0.01f && fabs(scaleY - 1.0f) <= 0.01f) {
            isFloor = false;
        }
    }

    glutPostRedisplay();
}
```
