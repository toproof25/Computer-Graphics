# 컴퓨터 그래픽스 보너스 프로젝트 과제


## 1. gluLookAt 함수 시야 변경

```cpp
    // 태양계 위에서 보기 추가
    gluLookAt(
        0, 0, 0.1,
        0.0, -100.0, 0.0,
        0.0, 1.0, .0
    );
```
## 2. 태양 주변에 상, 하, 좌, 우, 앞, 뒤 구분을 위한 박스 생성
```cpp

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
    // ... 생략 ...

    // 상하좌우앞뒤 구분 사각형 추가
    DrawDirectionBox();
}
```


## 3. 키보드 q, w 입력 시 시야 변경
- 위에서 보는 시야와 옆에서 보는 시야의 gluLookAt 파라미터 값들을 지정해준다
```cpp
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
```

## 4. 자동 공전 토글 추가
```cpp
// 지구, 달이 계속 공전하도록 추가 
void TogleRevolution() {
    if (isEarthRevolution) { // 키 1 버튼
        Day = (Day + 1) % 360;
    }
    if (isMoonRevolution) { // 키 2 버튼
        Time = (Time + 2) % 360;
    }
}
```

## 5. 마우스 클릭-이동 시 간단한 시야 변경
```cpp
// 마우스 이동 이벤트 좌표를 변수에 저장
void MyMouseMove(GLint X, GLint Y) {
    // 중앙이 0이 되도록 설정
    eyex = -((X - 250) / 250.0);
    eyey = (Y - 250) / 250.0;
    eyez = 0;
    printf("%lf %lf\n", eyex, eyey);
    glutPostRedisplay();
}
```

## 6. glutIdleFunc / glutTimerFunc 를 사용하여 천천히 시야 변경
- Timer 함수와 TimerFunc으로 화면이 지속적으로 바뀌도록 구현

### 키보드 입력 시 타켓 시점으로 이동
- 키보드로 정한 시야에 가까워지도록 타켓 값들이 조절됨
- 매우 근사한 수치로 가까워지면 시야 변경을 종료
```cpp
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
```

### glutTimerFunc에 Timer함수를 등록
```cpp
void Timer(int value) {
    TogleRevolution();
    SetLookAtValue();
    glutPostRedisplay();  // 화면 새로 그리기
    glutTimerFunc(16, Timer, 0);  // 다시 Timer를 실행 (계속해서 자신을 호출하여 반복)
}

int main(int argc, char** argv) {
    // ... 생략 ...

    //glutIdleFunc(MyIdle); // Idle 콜백 아래 TimerFunc 사용

    glutTimerFunc(16, Timer, 0);  // 화면을 계속 그리기 (시점 변경하는 모습을 자세하게 보기 위해)

}
```

