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