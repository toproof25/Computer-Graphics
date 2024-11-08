## 문제 2번
http://nehe.gamedev.net/lesson.asp?index=01의 하단 3DShapes 중 하나를 선택하여 소스코드를 변형하되 입체에 대한 모델 변환(이동, 회전, 크기 조절)과 시점 변환을 가하는 소스코드를 추가하여 시연해보라.


## 회전 스피드, 이동 좌표를 설정 변수
```cpp
float turn_speed = 0.0f; // 회전 스피드 조절
float move_x = 0.0f, move_y = 0.0f, move_z = 0.0f; // 이동 좌표 조절
```

## 키보드 입력으로 변수 값 설정
```cpp
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
```

## 이동 및 회전 연산 코드에 변수값 연산 추가
```cpp
// 피라미드 그리기
glTranslatef(-1.5f + move_x, 0.0f + move_y, -6.0f + move_z); // 피라미드 위치 이동
```
```cpp
// 큐브 그리기
glTranslatef(1.5f + move_x, 0.0f + move_y, -7.0f + move_z); // 큐브 위치 이동
```
```cpp
 rtri += 0.1f + turn_speed; // 피라미드 회전 업데이트
 rquad -= 0.075f + turn_speed; // 큐브 회전 업데이트
```


## 시점 마우스 이동으로 변경하기 추가
```cpp
// 마우스 이동 이벤트 좌표를 변수에 저장
void MyMouseMove(GLint X, GLint Y) {
    // 중앙이 0이 되도록 설정
    eyex = -((X - 320) / 320.0);
    eyey = (Y - 240) / 240.0;
    glutPostRedisplay();
}
```

```cpp
// gluLookAt 변수들
GLdouble eyex = 0, eyey = 0, eyez = 1;
GLdouble centerx = 0, centery = 0, centerz = -1;
GLdouble upx = 0, upy = 1, upz = 0;

gluLookAt(
    eyex, eyey, eyez,
    centerx, centery, centerz,
    upx, upy, upz
);
```
