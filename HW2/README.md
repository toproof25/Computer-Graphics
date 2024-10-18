# 컴퓨터 그래픽스 보너스 프로젝트 과제
---

## 1. gluLookAt 함수 시야 변경

```cpp
    // 태양계 위에서 보기 추가
    gluLookAt(
        0, 0, 0.1,
        0.0, -100.0, 0.0,
        0.0, 1.0, .0
    );
```

## 2. 키보드 q, w 입력 시 시야 변경
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

## 천천히 시야 변경
- Timer 함수와 TimerFunc으로 화면이 지속적으로 바뀌도록 구현

### MyDisplay에서 천천히 시야 변경
- 키보드로 정한 시야에 가까워지도록 타켓 값들이 조절됨
- 매우 근사한 수치로 가까워지면 시야 변겨을 종료
```cpp
// 천천히 시점 변화
if (isAnimating) {
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
        abs(target_eyez - eyez) < 0.001) {
        isAnimating = false;
    }
}

gluLookAt(
    eyex, eyey, eyez,
    centerx, centery, centerz,
    upx, upy, upz
);
```


### Timer, TimerFunc 함수
```cpp
void Timer(int value) {
    glutPostRedisplay();  // 화면 새로 그리기
    glutTimerFunc(16, Timer, 0);  // 대략 60 프레임 호출
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
    glutTimerFunc(16, Timer, 0);  // 60프레임으로 화면을 계속 그리기 (시점 변경하는 모습을 자세하게 보기 위해)
    glutMainLoop();
    return 0;
}
```

