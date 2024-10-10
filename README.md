# 컴퓨터 그래픽스 프로젝트1 과제
---

## 1. 바닥에 탁자 세우기

수업 시간에 배운 정점 배열을 활용하여 테이블 상판과 다리 4개를 조합하여 테이블을 그렸습니다.

- TableVertices, TableColors, TableVertexList 배열을 이용하여 테이블 상판을 그림

- TableLegVertices, TableLegColors, TableLegVertexList 배열을 이용하여 테이블 다리를 그림

- 다리같은 경우에는 하나를 완성하고, glPushMatrix, glTranslatef를 이용하여 테이블 모서리쪽으로 좌표를 이동하면서 4개를 이동하는 방식. 왼쪽 위, 오른쪽 위, 왼쪽 아래, 오른쪽 아래로 좌표를 이동하면서 정점 배열과 glDrawElements으로 그렸습니다

- 테이블을 그리는 코드는 디스플레이 리스트를 이용하여 그렸습니다.



## 2. 탁자 위에 기하학적 물체를 모델링

- 찻주전자를 불러와서 테이블 위쪽으로 위치하였습니다



## 3. 키보드 입력 시 

- w를 입력하면 와이어 프레임 모드로 glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 에서 GL_LINE, GL_FILL을 설정하여 w가 눌릴 때 마다 Wireframed값에 따라 와이어프레임 모드를 설정하였습니다.

- 키보드 e를 입력하면 ViewMode변수를 따로 두어 glLookAt에 z값을 1, -1로 변경합니다. 물체의 반대편을 보기 위해서 추가해보았습니다.



## 4. 마우스를 이용하여 시점 변경

- MyMouseMove함수에서 지속적으로 마우스 좌표값을 받아옵니다.

- gluLookAt 함수에서 적절한 연산을 통해 x, y값을 설정하여 마우스를 클릭하면서 이동할 경우 그려진 물체를 다양한 각도로 설정할 수 있도록 하였습니다.

- 좌클릭을 땔 경우 마지막 좌표 값을 저장하여 변경했던 각도를 유지하도록 작성하였습니다.

마우스 좌표값 저장   
![image](https://github.com/user-attachments/assets/5bfd446d-a27b-465a-853f-2357609ce924)

gluLookAt에 x, y값 설정   
![image](https://github.com/user-attachments/assets/5e328627-85c5-462f-968f-43edced9b76c)



깃허브 주소 : https://github.com/toproof25/Computer-Graphics
